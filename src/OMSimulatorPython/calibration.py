# This file is part of OpenModelica.
#
# Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
# c/o Linköpings universitet, Department of Computer and Information Science,
# SE-58183 Linköping, Sweden.
#
# All rights reserved.
#
# THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
# THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
# VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
#
# The OpenModelica software and the OSMC (Open Source Modelica Consortium)
# Public License (OSMC-PL) are obtained from OSMC, either from the above
# address, from the URLs:
# http://www.openmodelica.org or
# https://github.com/OpenModelica/ or
# http://www.ida.liu.se/projects/OpenModelica,
# and in the OpenModelica distribution.
#
# GNU AGPL version 3 is obtained from:
# https://www.gnu.org/licenses/licenses.html#GPL
#
# This program is distributed WITHOUT ANY WARRANTY; without
# even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
# IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
#
# See the full OSMC Public License conditions for more details.

'''
Parameter calibration of SSP models against reference data.

The calibration problem is described by a Calibration activity of the
OpenSCALING UQ layered standard (namespace UQ_NS): the parameters to fit
(CalibrationTargets), the observed variables, the reference data (a CSV
file), the simulation setting and fixed parameter values (ParameterSet).
The standard does not (yet) define how observed variables map to the
reference data, their weights, the calibration time window or optimizer
options. These are read from an optional KeyValues element of type
KEY_VALUES_TYPE in the SimulationSetting of the activity:

  startTime, stopTime     calibration time window (default: the reference data up
                          to the stopTime of the SimulationSetting)
  scaling                 none (default) | range | std, per observed variable
  maxEvaluations          maximum number of model evaluations
  relativeTolerance       optimizer tolerance on cost, step and gradient (default 1e-8)
  diffStep                relative finite difference step for the Jacobian
  weight:<variable>       residual weight of an observed variable (default 1)
  dataName:<variable>     reference data column of an observed variable

Without dataName, the column whose header is the variable name, optionally
followed by "|unit", is used. The first column is the time.

The fit is a bounded nonlinear least squares problem solved with
scipy.optimize.least_squares; numpy and scipy are only needed here, so they
are imported on first use.
'''

import csv
import math
import time
from dataclasses import dataclass, field
from pathlib import Path

from lxml import etree as ET

from OMSimulator.capi import Capi, Status
from OMSimulator.cref import CRef

UQ_NS = 'http://openscaling.org/UQ1/UncertaintyQuantification'
SSV_NS = 'http://ssp-standard.org/SSP1/SystemStructureParameterValues'
SSC_NS = 'http://ssp-standard.org/SSP1/SystemStructureCommon'
UQ_MIME_TYPE = 'application/x-uncertainty-quantification'
KEY_VALUES_TYPE = 'org.openmodelica.omsimulator.calibration'
SCALINGS = ('none', 'range', 'std')

# residual of a simulation that failed, per sample; keeps the optimizer away from there
_FAILED_RESIDUAL = 1e10


def _uq(tag: str) -> str:
  return f'{{{UQ_NS}}}{tag}'


def _float(element, name: str, default=None):
  value = element.get(name)
  return default if value is None or value == '' else float(value)


@dataclass
class CalibrationTarget:
  name: str
  initialValue: float
  lowerBound: float = -math.inf
  upperBound: float = math.inf
  unit: str | None = None


@dataclass
class ObservedVariable:
  name: str
  unit: str | None = None
  dataName: str | None = None
  weight: float = 1.0


@dataclass
class CalibrationProblem:
  '''A Calibration activity of a UQ layered standard file.'''
  activityId: str | None
  targets: list[CalibrationTarget]
  observed: list[ObservedVariable]
  referenceFile: Path
  parameters: dict[str, float | int | bool | str] = field(default_factory=dict)
  startTime: float | None = None
  stopTime: float | None = None
  tolerance: float | None = None
  scaling: str = 'none'
  maxEvaluations: int | None = None
  relativeTolerance: float = 1e-8
  diffStep: float | None = None
  experimentPoint: str | None = None

  @staticmethod
  def fromXML(filename, activityId: str | None = None) -> 'CalibrationProblem':
    '''Read a Calibration activity; activityId may be omitted if the file has only one.

    Relative file references are resolved against the directory of filename.'''
    filename = Path(filename)
    base = filename.parent
    root = ET.parse(str(filename)).getroot()
    activities = root.findall(f'.//{_uq("Calibration")}')
    if activityId is not None:
      activities = [a for a in activities if a.get('id') == activityId]
      if not activities:
        raise ValueError(f"No Calibration activity with id '{activityId}' in {filename}")
    elif len(activities) != 1:
      ids = ', '.join(str(a.get('id')) for a in activities)
      raise ValueError(f"{filename} has {len(activities)} Calibration activities ({ids}); select one by id")
    activity = activities[0]

    targets = []
    for target in activity.findall(f'{_uq("CalibrationTargets")}/{_uq("Target")}'):
      name = target.get('parameterName')
      initial = _float(target, 'initialValue')
      lower = _float(target, 'lowerBound', -math.inf)
      upper = _float(target, 'upperBound', math.inf)
      if initial is None:
        if math.isinf(lower) or math.isinf(upper):
          raise ValueError(f"Calibration target '{name}' needs an initialValue or both bounds")
        initial = 0.5 * (lower + upper)
      if not lower <= initial <= upper:
        raise ValueError(f"Initial value {initial} of calibration target '{name}' is outside [{lower}, {upper}]")
      targets.append(CalibrationTarget(name, initial, lower, upper, target.get('unit')))
    if not targets:
      raise ValueError(f"Calibration activity '{activity.get('id')}' has no CalibrationTargets")

    observed = [ObservedVariable(v.get('name'), v.get('unit'))
                for v in _elements(activity, 'ObservedVariables', 'ObservedVariable', base)]
    if not observed:
      raise ValueError(f"Calibration activity '{activity.get('id')}' has no ObservedVariables")

    parameters = {}
    for parameter in _elements(activity.find(_uq('ParameterSet')), 'Parameters', 'Parameter', base):
      value = _ssvValue(parameter)
      if value is not None:
        parameters[parameter.get('name')] = value

    referenceFile = None
    for source in activity.findall(f'{_uq("ReferenceData")}/{_uq("DataSource")}'):
      if source.get('file') and (source.get('type') in (None, 'text/csv') or source.get('file').endswith('.csv')):
        referenceFile = base / source.get('file')
        break
    if referenceFile is None:
      raise ValueError(f"Calibration activity '{activity.get('id')}' has no CSV ReferenceData/DataSource")

    problem = CalibrationProblem(activity.get('id'), targets, observed, referenceFile, parameters)

    point = activity.find(f'.//{_uq("ExperimentPoints")}//{_uq("Point")}')
    if point is not None:
      problem.experimentPoint = point.get('id')

    setting = activity.find(_uq('SimulationSetting'))
    if setting is not None:
      problem.stopTime = _float(setting, 'stopTime')
      problem.tolerance = _float(setting, 'tolerance')
      for keyValues in setting.findall(_uq('KeyValues')):
        if keyValues.get('type') == KEY_VALUES_TYPE:
          for kv in keyValues.findall(_uq('KeyValue')):
            problem._setOption(kv.get('key'), kv.get('value'))
    return problem

  def _setOption(self, key: str, value: str):
    observed = {v.name: v for v in self.observed}
    if ':' in key:
      option, name = key.split(':', 1)
      if name not in observed:
        raise ValueError(f"Calibration option '{key}' refers to an unknown observed variable '{name}'")
      if option == 'weight':
        observed[name].weight = float(value)
      elif option == 'dataName':
        observed[name].dataName = value
      else:
        raise ValueError(f"Unknown calibration option '{key}'")
    elif key in ('startTime', 'stopTime', 'relativeTolerance', 'diffStep'):
      setattr(self, key, float(value))
    elif key == 'maxEvaluations':
      self.maxEvaluations = int(value)
    elif key == 'scaling':
      if value not in SCALINGS:
        raise ValueError(f"Calibration scaling must be one of {', '.join(SCALINGS)}, got '{value}'")
      self.scaling = value
    else:
      raise ValueError(f"Unknown calibration option '{key}'")


def _elements(parent, container: str, child: str, base: Path):
  '''The child elements of parent/container, read from container/@source if that is given.'''
  if parent is None:
    return []
  result = []
  for element in parent.findall(_uq(container)):
    result += element.findall(_uq(child))
    if element.get('source'):
      external = ET.parse(str(base / element.get('source'))).getroot()
      result += external.findall(f'.//{_uq(child)}')
  return result


def _ssvValue(parameter):
  for type_, convert in (('Real', float), ('Integer', int), ('Boolean', lambda v: v == 'true'), ('String', str)):
    element = parameter.find(f'{{{SSV_NS}}}{type_}')
    if element is not None and element.get('value') is not None:
      return convert(element.get('value'))
  return None


@dataclass
class ReferenceData:
  time: list[float]
  values: dict[str, list[float]]

  @staticmethod
  def fromCSV(filename, observed: list[ObservedVariable], startTime=None, stopTime=None) -> 'ReferenceData':
    '''Read the columns of the observed variables; the first column is the time.

    Rows with a repeated time point (events) keep the last value.'''
    with open(filename, newline='', encoding='utf-8') as file:
      rows = [row for row in csv.reader(file) if row]
    header = [h.strip() for h in rows[0]]

    columns = {}
    for variable in observed:
      if variable.dataName is not None:
        candidates = [variable.dataName]
      else:
        candidates = [h for h in header if h == variable.name or h.split('|')[0] == variable.name]
      if not candidates or candidates[0] not in header:
        raise ValueError(f"No column for observed variable '{variable.name}' in {filename}; "
                         f"columns are {', '.join(header)}; set it with the dataName:{variable.name} option")
      columns[variable.name] = header.index(candidates[0])

    data = {}
    for row in rows[1:]:
      t = float(row[0])
      if (startTime is not None and t < startTime) or (stopTime is not None and t > stopTime):
        continue
      data[t] = [float(row[i]) for i in columns.values()]
    times = sorted(data)
    values = {name: [data[t][i] for t in times] for i, name in enumerate(columns)}
    if len(times) < 2:
      raise ValueError(f"{filename} has less than two samples in the calibration time window")
    return ReferenceData(times, values)


@dataclass
class CalibrationResult:
  parameters: dict[str, float]
  initialParameters: dict[str, float]
  success: bool
  message: str
  evaluations: int
  failedEvaluations: int
  cost: float
  rmse: dict[str, float]
  time: list[float]
  simulated: dict[str, list[float]]
  reference: dict[str, list[float]]
  elapsed: float


class Calibration:
  '''Fits the targets of a CalibrationProblem on an instantiated model.'''

  def __init__(self, model, problem: CalibrationProblem, callback=None):
    '''callback(evaluation: int, parameters: dict, cost: float) is called after each model evaluation.'''
    self.model = model
    self.problem = problem
    self.callback = callback
    # the model starts at its own start time; the window only selects the samples that are compared
    startTime = model.getStartTime()
    if problem.startTime is not None:
      startTime = max(startTime, problem.startTime)
    self.reference = ReferenceData.fromCSV(problem.referenceFile, problem.observed, startTime, problem.stopTime)
    self.evaluations = 0
    self.failures = 0
    self._crefs = {}
    for name in [t.name for t in problem.targets] + [v.name for v in problem.observed] + list(problem.parameters):
      self._crefs[name] = self._resolve(name)

  def _resolve(self, name: str) -> CRef:
    '''The CRef of a dotted variable name, with or without the root system name in front.'''
    root = self.model.system.name
    full = name if name.startswith(root + '.') else f'{root}.{name}'
    if full in self.model.mappedCrefs and '.' not in full[len(root) + 1:]:
      return CRef(*full.split('.'))
    # the element is the longest proper prefix that is known; the rest is the FMU variable name
    parts = full.split('.')
    for k in range(len(parts) - 1, 1, -1):
      element = '.'.join(parts[:k])
      if element in self.model.mappedCrefs:
        _, status = Capi.getVariableType(f"{self.model.mappedCrefs[element]}.{'.'.join(parts[k:])}")
        if status == Status.ok:
          return CRef(*parts[:k], '.'.join(parts[k:]))
        break
    raise KeyError(f"Unknown variable '{name}' in the SSP model")

  def simulate(self, values) -> dict[str, list[float]]:
    '''Simulate with the targets set to values and return the observed variables on the reference time grid.'''
    model = self.model
    if model.fmuInitialized:
      model.reset()
    for name, value in self.problem.parameters.items():
      model.setValue(self._crefs[name], value)
    for target, value in zip(self.problem.targets, values):
      model.setValue(self._crefs[target.name], float(value))
    model.initialize()
    result = {v.name: [] for v in self.problem.observed}
    for t in self.reference.time:
      if t > model.getTime():
        model.stepUntil(t)
      for v in self.problem.observed:
        result[v.name].append(model.getValue(self._crefs[v.name]))
    return result

  def _scale(self, values: list[float]) -> float:
    import numpy as np
    if self.problem.scaling == 'range':
      scale = np.ptp(values)
    elif self.problem.scaling == 'std':
      scale = np.std(values)
    else:
      return 1.0
    return scale if scale > 0.0 else 1.0

  def residuals(self, values):
    import numpy as np
    self.evaluations += 1
    n = len(self.reference.time)
    try:
      simulated = self.simulate(values)
      r = np.concatenate([v.weight * (np.asarray(simulated[v.name]) - self.reference.values[v.name]) / self._scales[v.name]
                          for v in self.problem.observed])
      if not np.all(np.isfinite(r)):
        raise ArithmeticError('non-finite simulation result')
    except (RuntimeError, ArithmeticError):
      self.failures += 1
      r = np.full(n * len(self.problem.observed), _FAILED_RESIDUAL)
    if self.callback:
      self.callback(self.evaluations, self._named(values), 0.5 * float(r @ r))
    return r

  def _named(self, values) -> dict[str, float]:
    return {t.name: float(v) for t, v in zip(self.problem.targets, values)}

  def run(self) -> CalibrationResult:
    import numpy as np
    from scipy.optimize import least_squares

    problem = self.problem
    start = time.time()
    model = self.model
    model.setResultFile('')
    model.setStopTime(self.reference.time[-1])
    if problem.tolerance is not None:
      model.setTolerance(problem.tolerance)
    self._scales = {v.name: self._scale(self.reference.values[v.name]) for v in problem.observed}

    x0 = np.array([t.initialValue for t in problem.targets])
    bounds = ([t.lowerBound for t in problem.targets], [t.upperBound for t in problem.targets])
    tol = problem.relativeTolerance
    fit = least_squares(self.residuals, x0, bounds=bounds, method='trf', x_scale='jac',
                        ftol=tol, xtol=tol, gtol=tol, max_nfev=problem.maxEvaluations,
                        diff_step=problem.diffStep)

    simulated = self.simulate(fit.x)
    rmse = {v.name: float(np.sqrt(np.mean((np.asarray(simulated[v.name]) - self.reference.values[v.name]) ** 2)))
            for v in problem.observed}
    return CalibrationResult(self._named(fit.x), self._named(x0), bool(fit.success), fit.message,
                             self.evaluations, self.failures, float(fit.cost), rmse, self.reference.time, simulated,
                             self.reference.values, time.time() - start)


def writeResults(filename, problem: CalibrationProblem, result: CalibrationResult, ssvFile=None):
  '''Store the result in the ResultSet of the Calibration activity in filename.

  The RMSE of each observed variable becomes an ErrorMetric. If ssvFile is
  given, the calibrated values are written there as an SSP parameter set and
  the Results element refers to it; a relative ssvFile is taken relative to
  the directory of filename.'''
  filename = Path(filename)
  tree = ET.parse(str(filename))
  activity = next(a for a in tree.getroot().iter(_uq('Calibration'))
                  if problem.activityId is None or a.get('id') == problem.activityId)

  resultSet = activity.find(_uq('ResultSet'))
  if resultSet is None:
    # appended last: it takes over the tail of the previous last child, which gets the sibling indentation
    last = activity[-1] if len(activity) else None
    resultSet = ET.SubElement(activity, _uq('ResultSet'))
    if last is not None:
      resultSet.tail, last.tail = last.tail, (activity.text or '\n')
  for old in resultSet.findall(_uq('Results')):
    resultSet.remove(old)
  results = ET.SubElement(resultSet, _uq('Results'))
  if problem.experimentPoint is not None:
    results.set('point', problem.experimentPoint)
  if ssvFile is not None:
    ssvFile = Path(ssvFile)
    path = ssvFile if ssvFile.is_absolute() else filename.parent / ssvFile
    writeSSV(path, problem, result)
    results.set('source', ssvFile.as_posix())
    results.set('type', 'application/x-ssp-parameter-values')
  for v in problem.observed:
    ET.SubElement(results, _uq('ErrorMetric'), type='RMSE',
                  value=repr(result.rmse[v.name]), observedVariable=v.name)

  # indent only what was added, the rest of the file keeps its formatting
  ET.indent(resultSet, space='  ', level=len(list(resultSet.iterancestors())))
  tree.write(str(filename), encoding='utf-8', xml_declaration=True)


def writeSSV(filename, problem: CalibrationProblem, result: CalibrationResult):
  '''Write the calibrated values as an SSP parameter set.'''
  root = ET.Element(f'{{{SSV_NS}}}ParameterSet', nsmap={'ssc': SSC_NS, 'ssv': SSV_NS},
                    version='2.0', name=problem.activityId or 'calibration')
  parameters = ET.SubElement(root, f'{{{SSV_NS}}}Parameters')
  for target in problem.targets:
    parameter = ET.SubElement(parameters, f'{{{SSV_NS}}}Parameter', name=target.name)
    # no unit: in an SSV it has to refer to a unit defined in the same file
    ET.SubElement(parameter, f'{{{SSV_NS}}}Real', value=repr(result.parameters[target.name]))
  Path(filename).parent.mkdir(parents=True, exist_ok=True)
  ET.ElementTree(root).write(str(filename), encoding='utf-8', xml_declaration=True, pretty_print=True)


def findUQFiles(ssp) -> list[str]:
  '''The UQ files referenced by ssc:MetaData of the active variant of an SSP: of the SSD, its systems or its components.'''
  files = []
  pending = [ssp.activeVariant, ssp.activeVariant.system]
  while pending:
    element = pending.pop(0)
    for meta in getattr(element, 'metaDataResources', []):
      if meta.get('type') == UQ_MIME_TYPE and meta.get('source') and meta['source'] not in files:
        files.append(meta['source'])
    pending += list(getattr(element, 'elements', {}).values())
  return files


def calibrateSSP(ssp, activityId: str | None = None, uqFile: str | None = None, output=None, callback=None) -> CalibrationResult:
  '''Calibrate an SSP and write the result into its UQ file.

  ssp is an SSP object or the path to an .ssp file. uqFile is the path of the
  UQ file inside the SSP; by default it is the one referenced by the
  ssc:MetaData of type UQ_MIME_TYPE. The calibrated values are stored next to
  the UQ file in <activity>_calibrated.ssv. If output is given, the updated
  SSP is exported there.'''
  from OMSimulator.ssp import SSP
  if not isinstance(ssp, SSP):
    ssp = SSP(str(ssp))

  if uqFile is not None:
    path = Path(ssp.temp_dir) / uqFile
  else:
    candidates = findUQFiles(ssp)
    if not candidates:
      raise ValueError(f"The SSP references no UQ file (ssc:MetaData with type '{UQ_MIME_TYPE}')")
    path = Path(ssp.temp_dir) / candidates[0]
  problem = CalibrationProblem.fromXML(path, activityId)

  model = ssp.instantiate()
  try:
    result = Calibration(model, problem, callback).run()
  finally:
    if model.fmuInitialized:
      model.terminate()
    model.delete()

  writeResults(path, problem, result, ssvFile=f'{problem.activityId or "calibration"}_calibrated.ssv')
  if output is not None:
    ssp.export(str(output))
  return result
