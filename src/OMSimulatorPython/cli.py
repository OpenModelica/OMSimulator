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
and co-simulation, use --mode to pick which one to run.
'''

import argparse
import sys
from pathlib import Path

from OMSimulator import FMU, SSP
from OMSimulator.fmu import _FMU_KIND_STR


def _runSSP(path: Path, args: argparse.Namespace) -> None:
  ssp = SSP(str(path))
  model = ssp.instantiate()

  if args.stop_time is not None:
    model.setStopTime(args.stop_time)
  if args.result_file is not None:
    model.setResultFile(args.result_file)

  model.initialize()
  model.simulate()
  model.terminate()
  model.delete()


def _logInfo(msg: str) -> None:
  '''Print a message using the "info:    " style of the OMSimulator log output.'''
  lines = msg.split('\n')
  print(f"info:    {lines[0]}")
  for line in lines[1:]:
    print(f"         {line}")


def _runFMU(path: Path, args: argparse.Namespace) -> None:
  fmu = FMU(str(path))
  fmu.mode = args.mode
  fmu.instantiate()  # sets defaults from the FMU's DefaultExperiment

  exp = dict(fmu.defaultExperiment)
  if args.start_time is not None:
    fmu.setStartTime(args.start_time)
    exp['startTime'] = args.start_time
  if args.stop_time is not None:
    fmu.setStopTime(args.stop_time)
    exp['stopTime'] = args.stop_time
  if args.tolerance is not None:
    fmu.setTolerance(args.tolerance)
    exp['tolerance'] = args.tolerance
  if args.step_size is not None:
    fmu.setStepSize(args.step_size)
    exp['stepSize'] = args.step_size
  fmu.setResultFile(args.result_file or f"{path.stem}_res.mat")

  kind_str = _FMU_KIND_STR.get(fmu.mode, fmu.mode)
  _logInfo(
    '*** FMU Simulation Info ***\n'
    f'- model:     {fmu.modelName} ({kind_str})\n'
    f'- startTime: {exp["startTime"]:.6f}\n'
    f'- stopTime:  {exp["stopTime"]:.6f}\n'
    f'- tolerance: {exp["tolerance"]:.6f}\n'
    f'- stepSize:  {exp["stepSize"]:.6f}'
  )

  fmu.initialize()
  fmu.simulate()
  fmu.terminate()
  fmu.delete()


_HANDLERS = {
  '.ssp': _runSSP,
  '.fmu': _runFMU,
}


def main(argv=None) -> int:
  parser = argparse.ArgumentParser(prog='OMSimulatorPython3', description=__doc__)
  parser.add_argument('model', type=Path, help='Path to a .ssp or .fmu file to simulate')
  parser.add_argument('--result-file', help='Override the result file name')
  parser.add_argument('--start-time', type=float, help='Override the simulation start time')
  parser.add_argument('--stop-time', type=float, help='Override the simulation stop time')
  parser.add_argument('--tolerance', type=float, help='Override the solver tolerance (.fmu only)')
  parser.add_argument('--step-size', type=float, help='Override the (maximum) simulation step size (.fmu only)')
  parser.add_argument('--mode', choices=['cs', 'me'],
                       help="Force 'cs' (co-simulation) or 'me' (model exchange) for FMUs that export both "
                            "kinds (.fmu only)")
  args = parser.parse_args(argv)

  handler = _HANDLERS.get(args.model.suffix.lower())
  if handler is None:
    parser.error(f"Unsupported file type '{args.model.suffix}'. Expected a .ssp or .fmu file, "
                 f"or run a .py driver script directly, e.g. 'OMSimulatorPython3 script.py'.")

  if not args.model.is_file():
    parser.error(f"File not found: {args.model}")

  handler(args.model, args)
  return 0


if __name__ == '__main__':
  sys.exit(main())
