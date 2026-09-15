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

'''SimulationClient: runs a simulation of an already-exported .ssp in a
separate subprocess (simulation/sim_worker.py, launched as `python -m`) for
crash isolation from misbehaving FMUs, reporting progress back over the
worker's own plain-text stdout/stdin protocol (see sim_worker.py's own
docstring for why this replaced the earlier ZMQ-based approach).

Everything here is driven by QProcess's own signals (readyReadStandardOutput,
finished, errorOccurred) -- no polling timer needed this time, since a
subprocess's stdout is something Qt can already notify us about directly,
unlike the ZMQ socket the previous version had to drain on a QTimer tick.
Pause/Resume/Stop are fire-and-forget stdin writes: there's no
request/reply round-trip to block on, which was itself a small latent
GUI-thread stall risk in the previous ZMQ implementation (a REQ/REP call
with a timeout, called directly from a button click).'''

import os
import sys
from pathlib import Path

from PySide6.QtCore import QObject, QProcess, QProcessEnvironment, Signal


def _installRoot() -> Path:
  '''sim_client.py installs to <install>/lib/OMSimulatorGui/simulation/ --
  three parents up from this file's own install location gets to <install>.'''
  return Path(__file__).resolve().parents[3]


def _workerProcessEnvironment() -> QProcessEnvironment:
  '''The subprocess needs its own copy of `OMSimulator` (and `OMSimulatorGui`
  itself, since the worker is launched as `python -m OMSimulatorGui...`)
  importable, plus the native library loadable -- same as this GUI process
  itself gets via OMSimulatorGui.bat/.in, but a subprocess started via
  QProcess only inherits the parent *environment*, not the parent
  interpreter's own sys.path, so PYTHONPATH/PATH must be set explicitly.'''
  env = QProcessEnvironment.systemEnvironment()
  installRoot = _installRoot()
  libDir = str(installRoot / 'lib')
  omcDir = str(installRoot / 'lib' / 'omc')
  binDir = str(installRoot / 'bin')

  pythonPath = env.value('PYTHONPATH')
  env.insert('PYTHONPATH', os.pathsep.join([libDir, omcDir, *([pythonPath] if pythonPath else [])]))

  path = env.value('PATH')
  env.insert('PATH', os.pathsep.join([binDir, *([path] if path else [])]))
  return env


class SimulationClient(QObject):
  progress = Signal(int)
  logLine = Signal(str)
  finished = Signal(bool, str)  # success, message (message empty on success)

  def __init__(self, parent=None):
    super().__init__(parent)
    self._process: QProcess | None = None
    self._finishedEmitted = False
    self._stdoutBuffer = ''

  def isRunning(self) -> bool:
    return self._process is not None and self._process.state() != QProcess.ProcessState.NotRunning

  def start(self, modelPath: str, workingDirectory: str, commandLineOptions: str = '') -> None:
    '''Launches sim_worker.py against the already-exported `modelPath`,
    with its working directory (and therefore any relatively-named result
    file) set to `workingDirectory`. `commandLineOptions` is an optional raw
    string of extra native flags (Tools > Options in MainWindow), forwarded
    verbatim -- sim_worker.py does the shlex splitting and applies them via
    Capi.setCommandLineOption() before the model is instantiated.'''
    if self.isRunning():
      raise RuntimeError('A simulation is already running.')
    self._finishedEmitted = False
    self._stdoutBuffer = ''

    args = ['-m', 'OMSimulatorGui.simulation.sim_worker',
            '--model', modelPath,
            '--working-directory', workingDirectory]
    if commandLineOptions:
      # Joined with '=' into one argv token, not passed as a separate
      # following token: commandLineOptions is virtually always itself a
      # string starting with '--' (e.g. "--suppressPath=true"), and argparse
      # (on the receiving end, sim_worker.py) refuses to consume a following
      # token that looks like another option as this option's value --
      # "expected one argument" -- unless it's joined into the same token.
      args.append(f'--command-line-options={commandLineOptions}')

    self._process = QProcess(self)
    self._process.setWorkingDirectory(workingDirectory)
    self._process.setProcessEnvironment(_workerProcessEnvironment())
    self._process.readyReadStandardOutput.connect(self._onStandardOutput)
    self._process.readyReadStandardError.connect(self._onStandardError)
    self._process.finished.connect(self._onProcessFinished)
    self._process.errorOccurred.connect(self._onProcessError)
    self._process.start(sys.executable, args)

  def _onStandardOutput(self) -> None:
    process = self._process
    if process is None:
      return
    # Buffered across calls: QProcess can hand us a partial line if the
    # worker's write lands mid-flush, and splitting on our own would corrupt
    # the next PROGRESS/DONE/ERROR token.
    self._stdoutBuffer += bytes(process.readAllStandardOutput()).decode(errors='replace')
    *lines, self._stdoutBuffer = self._stdoutBuffer.split('\n')
    for line in lines:
      self._handleLine(line.strip())

  def _handleLine(self, line: str) -> None:
    if not line:
      return
    if line.startswith('PROGRESS '):
      try:
        self.progress.emit(int(line[len('PROGRESS '):]))
      except ValueError:
        self.logLine.emit(line)
    elif line in ('DONE', 'STOPPED'):
      pass  # the actual outcome is reported via QProcess's own exit code
    else:
      self.logLine.emit(line)

  def _onStandardError(self) -> None:
    process = self._process
    if process is None:
      return
    chunk = bytes(process.readAllStandardError())
    if not chunk:
      return
    for line in chunk.decode(errors='replace').splitlines():
      if line:
        self.logLine.emit(line)

  def pause(self) -> bool:
    return self._sendCommand('PAUSE')

  def resume(self) -> bool:
    return self._sendCommand('RESUME')

  def stop(self) -> None:
    self._sendCommand('STOP')

  def _sendCommand(self, command: str) -> bool:
    if not self.isRunning():
      return False
    self._process.write(f'{command}\n'.encode())
    return True

  def _onProcessFinished(self, exitCode: int, exitStatus: QProcess.ExitStatus) -> None:
    success = exitCode == 0 and exitStatus == QProcess.ExitStatus.NormalExit
    message = '' if success else f'Simulation process exited with code {exitCode}.'
    self._finish(success, message)

  def _onProcessError(self, error: QProcess.ProcessError) -> None:
    # On QProcess.FailedToStart, `finished` never fires -- only this does.
    # On a crash after starting, both this and `finished` can fire; the
    # _finishedEmitted guard in _finish() keeps that from double-reporting.
    if error == QProcess.ProcessError.FailedToStart:
      self._finish(False, 'Failed to start the simulation process.')

  def _finish(self, success: bool, message: str) -> None:
    if self._finishedEmitted:
      return
    self._finishedEmitted = True
    self.finished.emit(success, message)
