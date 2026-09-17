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

'''SimulationProgressPanel: a progress bar + log + Pause/Resume/Stop panel
for a running simulation subprocess (see simulation/sim_client.py), embedded
in MainWindow below the diagram canvas rather than shown as a separate
modal dialog -- so simulating doesn't block interacting with the rest of
the window, and the same panel instance is reused across runs (see
setClient) instead of spawning a new top-level window every time.

Hidden by default; MainWindow shows it (via setClient) when a simulation
starts, and the user can dismiss it early with Hide without affecting the
run itself -- Stop is the only thing that actually stops the subprocess.'''

from PySide6.QtWidgets import (
    QHBoxLayout,
    QPlainTextEdit,
    QProgressBar,
    QPushButton,
    QVBoxLayout,
    QWidget,
)

from OMSimulatorGui.simulation.sim_client import SimulationClient


class SimulationProgressPanel(QWidget):
  def __init__(self, parent=None):
    super().__init__(parent)
    self._client: SimulationClient | None = None
    self._paused = False

    self._progressBar = QProgressBar(self)
    self._progressBar.setRange(0, 100)

    self._log = QPlainTextEdit(self)
    self._log.setReadOnly(True)
    self._log.setMaximumHeight(120)

    self._pauseButton = QPushButton('Pause', self)
    self._pauseButton.clicked.connect(self._onPauseClicked)
    self._stopButton = QPushButton('Stop', self)
    self._stopButton.clicked.connect(self._onStopClicked)
    self._hideButton = QPushButton('Hide', self)
    self._hideButton.clicked.connect(self.hide)

    buttonRow = QHBoxLayout()
    buttonRow.addWidget(self._pauseButton)
    buttonRow.addWidget(self._stopButton)
    buttonRow.addStretch()
    buttonRow.addWidget(self._hideButton)

    layout = QVBoxLayout(self)
    layout.addWidget(self._progressBar)
    layout.addWidget(self._log)
    layout.addLayout(buttonRow)

    self.hide()  # nothing running until MainWindow.setClient() is called

  def setClient(self, client: SimulationClient) -> None:
    '''Points the panel at a freshly-started SimulationClient, resetting
    the UI and swapping signal connections over to it -- called for every
    simulation run rather than creating a new panel each time.'''
    if self._client is not None:
      self._client.progress.disconnect(self._progressBar.setValue)
      self._client.logLine.disconnect(self._log.appendPlainText)
      self._client.finished.disconnect(self._onFinished)

    self._client = client
    self._paused = False
    self._progressBar.setValue(0)
    self._log.clear()
    self._pauseButton.setText('Pause')
    self._pauseButton.setEnabled(True)
    self._stopButton.setEnabled(True)

    client.progress.connect(self._progressBar.setValue)
    client.logLine.connect(self._log.appendPlainText)
    client.finished.connect(self._onFinished)

    self.show()

  def _onPauseClicked(self) -> None:
    if self._client is None:
      return
    if self._paused:
      if self._client.resume():
        self._paused = False
        self._pauseButton.setText('Pause')
    else:
      if self._client.pause():
        self._paused = True
        self._pauseButton.setText('Resume')

  def _onStopClicked(self) -> None:
    if self._client is None:
      return
    self._client.stop()
    self._stopButton.setEnabled(False)
    self._pauseButton.setEnabled(False)

  def _onFinished(self, success: bool, message: str) -> None:
    self._pauseButton.setEnabled(False)
    self._stopButton.setEnabled(False)
    if success:
      self._log.appendPlainText('Simulation finished.')
    else:
      self._log.appendPlainText(f'Simulation failed: {message}' if message else 'Simulation failed.')
