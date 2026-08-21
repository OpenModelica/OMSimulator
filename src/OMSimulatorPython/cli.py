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
Command line entry point for OMSimulatorPython3.

Allows running a model file directly, e.g.:
  OMSimulatorPython3 test.ssp
  OMSimulatorPython3 model.fmu

without having to write a driver script. All simulation settings
(start/stop time, result file, tolerance, ...) are taken from the
model itself; use --result-file/--start-time/--stop-time/--tolerance/
--step-size to override them. For FMUs that export both model exchange
and co-simulation, use --mode to pick which one to run. Pass --stripRoot
to drop the "model.root" prefix from exported signal names.

Files are validated against their schema (FMI for .fmu, SSP for .ssp)
before simulation; pass --validate to only validate the file and skip
simulation e.g.:
  OMSimulatorPython --validate test.ssp
  OMSimulatorPython --validate model.fmu
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

  if args.stopTime is not None:
    model.setStopTime(args.stopTime)
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

  if args.solver is not None:
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

# Boolean CLI flags that map 1:1 to a global native command line option (applies
# process-wide, regardless of .fmu/.ssp). args attribute name -> native option string.
_GLOBAL_FLAGS = {
  'stripRoot': '--stripRoot=true',
  'skipCSVHeader': '--skipCSVHeader=true',
}

def main(argv=None) -> int:
  parser = argparse.ArgumentParser(prog='OMSimulatorPython3', description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  parser.add_argument('model', type=Path, help='Path to a .ssp or .fmu file to simulate')
  parser.add_argument('--resultFile', default='default_res.mat', help='Override the result file name')
  parser.add_argument('--startTime', type=float, default=0.0, help='Override the simulation start time')
  parser.add_argument('--stopTime', type=float, default=1.0, help='Override the simulation stop time')
  parser.add_argument('--tolerance', type=float, default=1e-4, help='Override the solver tolerance (.fmu only)')
  parser.add_argument('--stepSize', type=float, default=1e-3, help='Override the (maximum) simulation step size (.fmu only)')
  parser.add_argument('--mode', choices=['cs', 'me'], default='me', help="Force 'cs' (co-simulation) or 'me' (model exchange) for FMUs that export both " "kinds (.fmu only)")
  parser.add_argument('--solver', choices=['euler', 'cvode'], default='cvode', help='Set the ODE solver for model-exchange FMUs (.fmu, mode=me only)')
  parser.add_argument('--stripRoot', action='store_true', default=True, help='Remove the root system prefix from exported signal names')
  parser.add_argument('--skipCSVHeader', action='store_true', default=True, help='Skip the CSV delimiter row in the header of .csv result files (already the default)')
  parser.add_argument('--validate', action='store_true', help='Only validate the file against its schema; do not simulate')
  parser.add_argument('--addParametersToCSV', action='store_true', help='Export parameters to a .csv file')
  parser.add_argument('--algLoopSolver', choices=['fixedpoint', 'kinsol'], default='kinsol', help='Specifies the loop solver method (fixedpoint, kinsol) used for algebraic loops spanning multiple components')
  parser.add_argument('--clearAllOptions', action='store_true', help='Reset all flags to their default values')
  parser.add_argument('--CVODEMaxErrTestFails', type=int, default=100, help='Maximum number of error test failures for CVODE')
  parser.add_argument('--CVODEMaxNLSFailures', type=int, default=100, help='Maximum number of nonlinear convergence failures for CVODE')
  parser.add_argument('--CVODEMaxNLSIterations', type=int, default=5, help='Maximum number of nonlinear solver iterations for CVODE')
  parser.add_argument('--CVODEMaxSteps', type=int, default=1000, help='Maximum number of steps for CVODE')
  parser.add_argument('--deleteTempFiles', action=argparse.BooleanOptionalAction, default=True, help='Delete temporary files as soon as they are no longer needed')
  parser.add_argument('--directionalDerivatives', action=argparse.BooleanOptionalAction, default=True, help='Use directional derivatives to calculate the Jacobian for algebraic loops')
  parser.add_argument('--dumpAlgLoops', action='store_true', help='Dump information for algebraic loops')
  parser.add_argument('--emitEvents', action=argparse.BooleanOptionalAction, default=True, help='Emit events during simulation')
  parser.add_argument('--ignoreInitialUnknowns', action='store_true', help='Ignore initial unknowns from the modelDescription.xml')
  parser.add_argument('--initialStepSize', type=float, default=1e-6, help='Specify the initial step size')
  parser.add_argument('--inputExtrapolation', action='store_true', help='Enable input extrapolation using derivative information')
  parser.add_argument('--intervals', '-i', type=int, default=500, help='Specify the number of communication points (arg > 1)')
  parser.add_argument('--logFile', '-l', help='Specify the log file (stdout is used if no log file is specified)')
  parser.add_argument('--logLevel', type=int, default=0, help='Set the log level (0: default, 1: debug, 2: debug+trace)')
  parser.add_argument('--master', default='ma', help='Specify the master algorithm (ma)')
  parser.add_argument('--maxEventIteration', type=int, default=100, help='Specify the maximum number of iterations for handling a single event')
  parser.add_argument('--maxLoopIteration', type=int, default=10, help='Specify the maximum number of iterations for solving algebraic loops between system-level components. Internal algebraic loops of components are not affected.')
  parser.add_argument('--minimumStepSize', type=float, default=1e-12, help='Specify the minimum step size')
  parser.add_argument('--numProcs', '-n', type=int, default=1, help='Specify the maximum number of processors to use (0=auto, 1=default)')
  parser.add_argument('--progressBar', action='store_true', help='Show a progress bar for the simulation progress in the terminal')
  parser.add_argument('--realTime', action='store_true', help='Enable experimental feature for (soft) real-time co-simulation')
  parser.add_argument('--solverStats', action='store_true', help='Add solver stats to the result file, e.g., step size; not supported for all solvers')
  parser.add_argument('--suppressPath', action='store_true', help='Suppress path information in info messages; especially useful for testing')
  parser.add_argument('--tempDir', default='.', help='Specify the temporary directory')
  parser.add_argument('--timeout', type=int, default=0, help='Specify the maximum allowed time in seconds for running a simulation (0 disables)')
  parser.add_argument('--wallTime', action='store_true', help='Add wall time information to the result file')
  parser.add_argument('--workingDir', default='.', help='Specify the working directory')
  parser.add_argument('--zeroNominal', action='store_true', help='Accept FMUs with invalid nominal values and replace the invalid nominal values with 1.0')
  args = parser.parse_args(argv)

  handler = _HANDLERS.get(args.model.suffix.lower())
  if handler is None:
    parser.error(f"Unsupported file type '{args.model.suffix}'. Expected a .ssp or .fmu file, "f"or run a .py driver script directly, e.g. 'OMSimulatorPython3 script.py'.")

  if not args.model.is_file():
    parser.error(f"File not found: {args.model}")

  # Global native flags; set them once here rather than threading them through
  # FMU/SSP/SSD/InstantiatedModel. Must happen before instantiate().
  for flag_name, option in _GLOBAL_FLAGS.items():
    if getattr(args, flag_name):
      status = Capi.setCommandLineOption(option)
      if status != Status.ok:
        raise RuntimeError(f"Failed to set {option}: {status}")

  handler(args.model, args)
  return 0

if __name__ == '__main__':
  sys.exit(main())
