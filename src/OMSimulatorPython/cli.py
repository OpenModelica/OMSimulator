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
Command line entry point for OMSimulator.

Allows running a model file directly, e.g.:
  OMSimulator test.ssp
  OMSimulator model.fmu

without having to write a driver script. All simulation settings
(start/stop time, result file, tolerance, ...) are taken from the
model itself; use --resultFile/--startTime/--stopTime/--tolerance/
--stepSize to override them. For FMUs that export both model exchange
and co-simulation, use --mode to pick which one to run. Pass --stripRoot
to drop the "model.root" prefix from exported signal names.

Files are validated against their schema (FMI for .fmu, SSP for .ssp)
before simulation; pass --validate to only validate the file and skip
simulation e.g.:
  OMSimulator --validate test.ssp
  OMSimulator --validate model.fmu
'''

import argparse
import sys
import warnings
from pathlib import Path

from OMSimulator import FMU, SSP, Capi
from OMSimulator.capi import Status
from OMSimulator.fmu import _FMU_KIND_STR


def _runSSP(path: Path, args: argparse.Namespace) -> None:
  if args.validate:
    # SSP(...) already validates the SSD plus any embedded FMU/SSV/SSM resources
    # against their schemas on construction, reporting failures via warnings.
    with warnings.catch_warnings(record=True) as caught:
      warnings.simplefilter('always')
      SSP(str(path))
    for w in caught:
      print(f"warning: {w.message}")
    if not caught:
      _logInfo(f"{path.name} conforms to the SSP-2.0 schema")
    return

  ssp = SSP(str(path))
  model = ssp.instantiate()

  if args.startTime is not None:
    model.setStartTime(args.startTime)
  if args.stopTime is not None:
    model.setStopTime(args.stopTime)
  if args.tolerance is not None:
    model.setTolerance(args.tolerance)
  if args.resultFile is not None:
    model.setResultFile(args.resultFile)

  model.initialize()
  model.simulate()
  model.terminate()
  model.delete()

def _logInfo(msg: str) -> None:
  '''Print a message using the "info: " style of the OMSimulator log output.'''
  lines = msg.split('\n')
  print(f"info: {lines[0]}", flush=True)
  for line in lines[1:]:
    print(f"     {line}", flush=True)

def _runFMU(path: Path, args: argparse.Namespace) -> None:
  fmu = FMU(str(path))  # parses modelDescription.xml and validates it against the FMI schema

  if args.validate:
    if fmu.valid:
      _logInfo(f"{path.name} conforms to the FMI-{fmu.fmiVersion} schema")
    return

  fmu.mode = args.mode
  # start/stop/tolerance/stepSize must be set before instantiate(): they need to reach
  # the FMU's fmi2SetupExperiment call, not just the solver's own bookkeeping, otherwise
  # the FMU's first recorded sample won't reflect the override (only later steps would).
  fmu.startTime = args.startTime
  fmu.stopTime = args.stopTime
  fmu.tolerance = args.tolerance
  fmu.stepSize = args.stepSize
  fmu.instantiate()  # applies the settings above (falling back to the FMU's DefaultExperiment)

  # instantiate() resolves the mode for an FMU exporting both kinds.
  if fmu.mode == 'cs':
    fmu.setSolver('ma')
  elif args.solver is not None:
    fmu.setSolver(args.solver)
  fmu.setResultFile(args.resultFile or f"{path.stem}_res.mat")

  exp = fmu.appliedExperiment
  kind_str = _FMU_KIND_STR.get(fmu.mode, fmu.mode)
  _logInfo(
    '*** FMU Simulation Info ***\n'
    f'- model     : {fmu.modelName} ({kind_str})\n'
    f'- fmiVersion: {fmu.fmiVersion}\n'
    f'- startTime : {exp["startTime"]:.6f}\n'
    f'- stopTime  : {exp["stopTime"]:.6f}\n'
    f'- tolerance : {exp["tolerance"]:.6g}\n'
    f'- stepSize  : {exp["stepSize"]:.6g}'
  )

  fmu.initialize()
  fmu.simulate()
  fmu.terminate()
  fmu.delete()

_HANDLERS = {
  '.ssp': _runSSP,
  '.fmu': _runFMU,
}

# Forwarded verbatim to the native flag parser (Flags.h), which owns their real
# defaults. clearAllOptions is first because it resets whatever came before it.
_NATIVE_OPTIONS = (
  'clearAllOptions', 'addParametersToCSV', 'algLoopSolver', 'CVODEMaxErrTestFails',
  'CVODEMaxNLSFailures', 'CVODEMaxNLSIterations', 'CVODEMaxSteps', 'deleteTempFiles',
  'directionalDerivatives', 'dumpAlgLoops', 'emitEvents', 'ignoreInitialUnknowns',
  'initialStepSize', 'inputExtrapolation', 'intervals', 'logFile', 'logLevel',
  'master', 'maxEventIteration', 'maxLoopIteration', 'minimumStepSize', 'numProcs',
  'progressBar', 'realTime', 'skipCSVHeader', 'solverStats', 'stripRoot',
  'tempDir', 'timeout', 'wallTime', 'workingDir', 'zeroNominal',
)

# re_void in Flags.h: rejected unless passed without a value.
_NATIVE_VOID_OPTIONS = frozenset({'clearAllOptions'})


def _addOption(parser, name, default, help, **kwargs):
  '''Declare an option documenting a default that argparse must not apply.

  A value argparse filled in would reach the native flag parser or the FMU as an
  explicit setting and override the model's own.
  '''
  parser.add_argument(name, default=None, help=f'{help} (default: {default})', **kwargs)


def _nativeArg(option: str, value: str | None = None) -> str:
  '''Render one argument for std::quoted, the native flag tokenizer.

  It only strips quotes that open a token, so the whole argument goes inside
  them, not just the value.
  '''
  arg = option if value is None else f'{option}={value}'
  return '"' + arg.replace('\\', '\\\\').replace('"', '\\"') + '"'


def main(argv=None) -> int:
  parser = argparse.ArgumentParser(prog='OMSimulator', description=__doc__)
  parser.add_argument('model', type=Path, help='Path to a .ssp or .fmu file to simulate')
  parser.add_argument('--validate', action='store_true', help='Only validate the file against its schema; do not simulate')
  parser.add_argument('--version', action='version', version=f'{Capi.getVersion()}')
  _addOption(parser, '--addParametersToCSV', 'false', 'Export parameters to a .csv file', action=argparse.BooleanOptionalAction)
  _addOption(parser, '--algLoopSolver', 'kinsol', 'Specifies the loop solver method used for algebraic loops spanning multiple components', choices=['fixedpoint', 'kinsol'])
  parser.add_argument('--clearAllOptions', action='store_true', default=None, help='Reset all flags to their default values')
  _addOption(parser, '--CVODEMaxErrTestFails', 100, 'Maximum number of error test failures for CVODE', type=int)
  _addOption(parser, '--CVODEMaxNLSFailures', 100, 'Maximum number of nonlinear convergence failures for CVODE', type=int)
  _addOption(parser, '--CVODEMaxNLSIterations', 5, 'Maximum number of nonlinear solver iterations for CVODE', type=int)
  _addOption(parser, '--CVODEMaxSteps', 1000, 'Maximum number of steps for CVODE', type=int)
  _addOption(parser, '--deleteTempFiles', 'true', 'Delete temporary files as soon as they are no longer needed', action=argparse.BooleanOptionalAction)
  _addOption(parser, '--directionalDerivatives', 'true', 'Use directional derivatives to calculate the Jacobian for algebraic loops', action=argparse.BooleanOptionalAction)
  _addOption(parser, '--dumpAlgLoops', 'false', 'Dump information for algebraic loops', action=argparse.BooleanOptionalAction)
  _addOption(parser, '--emitEvents', 'true', 'Emit events during simulation', action=argparse.BooleanOptionalAction)
  _addOption(parser, '--ignoreInitialUnknowns', 'false', 'Ignore initial unknowns from the modelDescription.xml', action=argparse.BooleanOptionalAction)
  _addOption(parser, '--initialStepSize', 1e-6, 'Specify the initial step size', type=float)
  _addOption(parser, '--inputExtrapolation', 'false', 'Enable input extrapolation using derivative information', action=argparse.BooleanOptionalAction)
  _addOption(parser, '--intervals', 500, 'Specify the number of communication points (arg > 1)', type=int)
  _addOption(parser, '--logFile', 'stdout', 'Specify the log file')
  _addOption(parser, '--logLevel', 0, 'Set the log level (0: default, 1: debug, 2: debug+trace)', type=int)
  _addOption(parser, '--master', 'ma', 'Specify the master algorithm (ma)')
  _addOption(parser, '--maxEventIteration', 100, 'Specify the maximum number of iterations for handling a single event', type=int)
  _addOption(parser, '--maxLoopIteration', 10, 'Specify the maximum number of iterations for solving algebraic loops between system-level components. Internal algebraic loops of components are not affected.', type=int)
  _addOption(parser, '--minimumStepSize', 1e-12, 'Specify the minimum step size', type=float)
  _addOption(parser, '--mode', 'co-simulation', "Force 'cs' (co-simulation) or 'me' (model exchange) for FMUs that export both kinds (.fmu only)", choices=['cs', 'me'])
  _addOption(parser, '--numProcs', 1, 'Specify the maximum number of processors to use (0=auto, 1=default)', type=int)
  _addOption(parser, '--progressBar', 'false', 'Show a progress bar for the simulation progress in the terminal', action=argparse.BooleanOptionalAction)
  _addOption(parser, '--realTime', 'false', 'Enable experimental feature for (soft) real-time co-simulation', action=argparse.BooleanOptionalAction)
  _addOption(parser, '--resultFile', "the model name plus '_res.mat'", 'Specify the name of the output result file')
  _addOption(parser, '--skipCSVHeader', 'true', 'Skip the CSV delimiter row in the header of .csv result files', action=argparse.BooleanOptionalAction)
  _addOption(parser, '--solver', 'cvode', 'Set the ODE solver for model-exchange FMUs (.fmu, mode=me only)', choices=['euler', 'cvode'])
  _addOption(parser, '--solverStats', 'false', 'Add solver stats to the result file, e.g., step size; not supported for all solvers', action=argparse.BooleanOptionalAction)
  _addOption(parser, '--startTime', 'from the model', 'Specify the start time', type=float)
  _addOption(parser, '--stepSize', 'from the model', 'Specify the (maximum) step size (.fmu only)', type=float)
  _addOption(parser, '--stopTime', 'from the model', 'Specify the stop time', type=float)
  _addOption(parser, '--stripRoot', 'false', 'Remove the root system prefix from all exported signals', action=argparse.BooleanOptionalAction)
  _addOption(parser, '--suppressPath', 'true', 'Suppress path information in info messages; especially useful for testing', action=argparse.BooleanOptionalAction)
  _addOption(parser, '--tempDir', 'the working directory', 'Specify the temporary directory')
  _addOption(parser, '--timeout', '0 (disabled)', 'Specify the maximum allowed time in seconds for running a simulation', type=int)
  _addOption(parser, '--tolerance', 'from the model', 'Specify the relative tolerance', type=float)
  _addOption(parser, '--wallTime', 'false', 'Add wall time information to the result file', action=argparse.BooleanOptionalAction)
  _addOption(parser, '--workingDir', 'the current directory', 'Specify the working directory')
  _addOption(parser, '--zeroNominal', 'false', 'Accept FMUs with invalid nominal values and replace the invalid nominal values with 1.0', action=argparse.BooleanOptionalAction)
  args = parser.parse_args(argv)

  handler = _HANDLERS.get(args.model.suffix.lower())
  if handler is None:
    parser.error(f"Unsupported file type '{args.model.suffix}'. Expected a .ssp or .fmu file, "f"or run a .py driver script directly, e.g. 'OMSimulator script.py'.")

  if not args.model.is_file():
    parser.error(f"File not found: {args.model}")

  # Claim the flag before the FMU/SSP layers, which suppress paths themselves.
  Capi.setSuppressPath(True if args.suppressPath is None else args.suppressPath)

  # Global native flags; set them once here rather than threading them through
  # FMU/SSP/SSD/InstantiatedModel. Must happen before instantiate().
  for attr in _NATIVE_OPTIONS:
    value = getattr(args, attr)
    if value is None:
      continue
    if attr in _NATIVE_VOID_OPTIONS:
      arg = _nativeArg(f'--{attr}')
    else:
      arg = _nativeArg(f'--{attr}', str(value).lower() if isinstance(value, bool) else str(value))
    status = Capi.setCommandLineOption(arg)
    if status != Status.ok:
      raise RuntimeError(f"Failed to set {arg}: {status}")

  handler(args.model, args)
  return 0

if __name__ == '__main__':
  sys.exit(main())
