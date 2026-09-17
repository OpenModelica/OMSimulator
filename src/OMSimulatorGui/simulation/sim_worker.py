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

'''Standalone simulation worker for OMSimulatorGui: instantiates and steps
an already-exported .ssp directly via the OMSimulatorPython API, reporting
progress as plain text lines on stdout and accepting PAUSE/RESUME/STOP
commands as plain text lines on stdin.

This replaces OMSimulatorSimulationServer.py (the ZMQ pub/sub + req/rep
script OMEdit's own C++ simulation output widget also uses) for this GUI
only, not for OMEdit -- pyzmq turned out to be the single most fragile dependency touched
while building M6 (a broken system-wide install cost real debugging time),
and the ZMQ script's own pause/stop loop had three real, independently
confirmed bugs this session. A subprocess is still used for the same crash
isolation as before -- only the IPC protocol changes, from ZMQ sockets to
QProcess's own stdin/stdout, which the GUI already has to handle
asynchronously either way.

Deliberately has no PySide6 import -- this runs as a plain `python -m`
invocation, not as part of the Qt application.

Protocol (one line each, stdout unless noted):
  PROGRESS <int 0-100>   -- emitted after every step while running
  DONE                   -- simulation reached stopTime normally
  STOPPED                -- ended early due to a STOP command
  ERROR <message>        -- something went wrong (also a nonzero exit code)
  (stdin) PAUSE / RESUME / STOP -- one command per line, read continuously
  by a background thread so the main loop is never blocked waiting on it
'''

import argparse
import math
import shlex
import sys
import threading
import time

from OMSimulator import SSP, Capi
from OMSimulator.capi import Status

_state = {'paused': False, 'alive': True}
_lock = threading.Lock()


def _readCommands() -> None:
  '''Runs in its own thread for the process's whole lifetime so PAUSE/
  RESUME/STOP take effect immediately regardless of what the main loop is
  doing -- reading stdin.readline() only blocks this thread, never the
  simulation loop.'''
  for line in sys.stdin:
    command = line.strip().upper()
    with _lock:
      if command == 'PAUSE':
        _state['paused'] = True
      elif command == 'RESUME':
        _state['paused'] = False
      elif command == 'STOP':
        _state['alive'] = False


def _isPaused() -> bool:
  with _lock:
    return _state['paused']


def _isAlive() -> bool:
  with _lock:
    return _state['alive']


def _main() -> int:
  parser = argparse.ArgumentParser(description='OMSimulatorGui simulation worker')
  parser.add_argument('--model', required=True, help='the .ssp to simulate')
  parser.add_argument('--working-directory', default=None,
                       help='sets the native working directory (relative result files land here)')
  parser.add_argument('--command-line-options', default=None,
                       help='extra native flags forwarded verbatim to Capi.setCommandLineOption(), '
                            'e.g. "--suppressPath=true --logLevel=1"')
  args = parser.parse_args()

  if args.working_directory:
    Capi.setWorkingDirectory(args.working_directory)

  # Same mechanism (and the same native flags) OMSimulator's own CLI forwards
  # its options through -- see cli.py's _NATIVE_OPTIONS loop. Must happen
  # before the model is loaded/instantiated below.
  if args.command_line_options:
    for token in shlex.split(args.command_line_options):
      status = Capi.setCommandLineOption(token)
      if status != Status.ok:
        print(f'ERROR Failed to set command line option {token!r}: {status}', flush=True)
        return 1

  threading.Thread(target=_readCommands, daemon=True).start()

  try:
    model = SSP(args.model)
    instModel = model.instantiate()
    instModel.setResultFile(model.activeVariant.resultFile)

    startTime = float(model.activeVariant.startTime)
    stopTime = float(model.activeVariant.stopTime)
    instModel.setStartTime(startTime)
    instModel.setStopTime(stopTime)
    instModel.initialize()
  except Exception as e:
    print(f'ERROR {e}', flush=True)
    return 1

  print('PROGRESS 0', flush=True)
  time_ = float(instModel.getTime())
  stoppedEarly = False

  try:
    while _isAlive():
      if _isPaused():
        time.sleep(0.1)
        continue

      instModel.doStep()
      time_ = float(instModel.getTime())
      try:
        progress = math.floor((time_ - startTime) / (stopTime - startTime) * 100)
      except ZeroDivisionError:
        progress = 100
      print(f'PROGRESS {progress}', flush=True)

      if time_ >= stopTime:
        break
    else:
      stoppedEarly = True
  except Exception as e:
    print(f'ERROR {e}', flush=True)
    return 1
  finally:
    instModel.terminate()
    instModel.delete()

  if stoppedEarly:
    print('STOPPED', flush=True)
  else:
    print('PROGRESS 100', flush=True)
    print('DONE', flush=True)
  return 0


if __name__ == '__main__':
  sys.exit(_main())
