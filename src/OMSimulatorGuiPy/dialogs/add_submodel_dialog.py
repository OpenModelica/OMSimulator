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

'''AddSubModelDialog: browse for an FMU or a result-file lookup table
(.csv/.mat) and name the resulting component -- matches OMEdit's own Add
SubModel, which offers both file kinds through the same dialog/action rather
than a separate "Add Component Table" action. SSP.addComponent already
dispatches on the resource's own type (an FMU becomes a Component, a
.csv/.mat becomes a ComponentTable via SSP._addResource constructing a
ResultReader for it, whose signals are always Causality.output) -- this
dialog only needed its file filter widened to actually let the user pick
one.'''

from pathlib import Path

from PySide6.QtWidgets import QDialog, QDialogButtonBox, QFileDialog, QFormLayout, QHBoxLayout, QLineEdit, QPushButton, QWidget

_FILE_FILTER = 'Component files (*.fmu *.csv *.mat);;FMU files (*.fmu);;Result files (*.csv *.mat)'


class AddSubModelDialog(QDialog):
  def __init__(self, parent=None):
    super().__init__(parent)
    self.setWindowTitle('Add Component')

    self._pathEdit = QLineEdit(self)
    self._pathEdit.textChanged.connect(self._onPathChanged)
    browseButton = QPushButton('Browse...', self)
    browseButton.clicked.connect(self._onBrowse)

    pathRow = QWidget(self)
    pathLayout = QHBoxLayout(pathRow)
    pathLayout.setContentsMargins(0, 0, 0, 0)
    pathLayout.addWidget(self._pathEdit)
    pathLayout.addWidget(browseButton)

    self._nameEdit = QLineEdit(self)

    buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel, self)
    buttons.accepted.connect(self._onAccept)
    buttons.rejected.connect(self.reject)

    layout = QFormLayout(self)
    layout.addRow('File:', pathRow)
    layout.addRow('Name:', self._nameEdit)
    layout.addRow(buttons)

  def _onBrowse(self) -> None:
    path, _ = QFileDialog.getOpenFileName(self, 'Select Component', '', _FILE_FILTER)
    if path:
      self._pathEdit.setText(path)

  def _onPathChanged(self, text: str) -> None:
    if not self._nameEdit.text().strip() and text.strip():
      self._nameEdit.setText(Path(text).stem)

  def _onAccept(self) -> None:
    if self._pathEdit.text().strip() and self._nameEdit.text().strip():
      self.accept()

  def filePath(self) -> str:
    return self._pathEdit.text().strip()

  def name(self) -> str:
    return self._nameEdit.text().strip()
