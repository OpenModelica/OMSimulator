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

'''OptionsDialog: global, app-wide simulation preferences -- Tools > Options.

Mirrors OMEdit's own "OMSimulator/SSP" options page (Command Line Options +
Logging Level), minus the logging level dropdown, which this GUI has no
equivalent concept for yet. Persisted via QSettings by MainWindow, not by
this dialog itself -- this class only collects the two values.

Working Directory: when set, every simulation run exports the model into
this fixed directory and runs there (so result files land somewhere
predictable and get overwritten run to run) instead of a fresh
tempfile.mkdtemp() directory each time (the default, left blank).

Command Line Options: forwarded token-by-token straight to
Capi.setCommandLineOption() by sim_worker.py before the model is
instantiated -- the same native flags (and the same underlying call)
OMSimulator's own CLI (src/OMSimulatorPython/cli.py's _NATIVE_OPTIONS)
forwards for e.g. "--suppressPath=true" or "--logLevel=1". Free text, split
with shlex so a quoted value containing spaces survives intact.'''

from PySide6.QtWidgets import (
    QDialog,
    QDialogButtonBox,
    QFileDialog,
    QFormLayout,
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QPushButton,
    QVBoxLayout,
    QWidget,
)


class OptionsDialog(QDialog):
  def __init__(self, workingDirectory: str, commandLineOptions: str, parent=None):
    super().__init__(parent)
    self.setWindowTitle('Options')

    self._workingDirEdit = QLineEdit(workingDirectory, self)
    self._workingDirEdit.setPlaceholderText('(use a fresh temporary directory for every run)')
    browseButton = QPushButton('Browse...', self)
    browseButton.clicked.connect(self._onBrowse)

    workingDirRow = QWidget(self)
    workingDirLayout = QHBoxLayout(workingDirRow)
    workingDirLayout.setContentsMargins(0, 0, 0, 0)
    workingDirLayout.addWidget(self._workingDirEdit)
    workingDirLayout.addWidget(browseButton)

    self._commandLineOptionsEdit = QLineEdit(commandLineOptions, self)
    self._commandLineOptionsEdit.setPlaceholderText('e.g. --suppressPath=true --logLevel=1')

    formLayout = QFormLayout()
    formLayout.addRow('Working Directory:', workingDirRow)
    formLayout.addRow('Command Line Options:', self._commandLineOptionsEdit)

    hint = QLabel(
        'Applies to every simulation run. Command line options are the same native '
        'flags OMSimulator\'s own CLI accepts (see --help).', self)
    hint.setWordWrap(True)

    buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel, self)
    buttons.accepted.connect(self.accept)
    buttons.rejected.connect(self.reject)

    layout = QVBoxLayout(self)
    layout.addLayout(formLayout)
    layout.addWidget(hint)
    layout.addWidget(buttons)

  def _onBrowse(self) -> None:
    path = QFileDialog.getExistingDirectory(self, 'Select Working Directory', self._workingDirEdit.text())
    if path:
      self._workingDirEdit.setText(path)

  def workingDirectory(self) -> str:
    return self._workingDirEdit.text().strip()

  def commandLineOptions(self) -> str:
    return self._commandLineOptionsEdit.text().strip()
