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

'''CreateModelDialog: prompts for a brand-new SSP's model name and its root
system name (File > New) -- these are two distinct fields in the underlying
model (SSD.name vs. the root System's own name), matching OMEdit's "SSP
Model" dialog.'''

from PySide6.QtWidgets import QDialog, QDialogButtonBox, QFormLayout, QGroupBox, QLineEdit, QVBoxLayout


class CreateModelDialog(QDialog):
  def __init__(self, parent=None):
    super().__init__(parent)
    self.setWindowTitle('SSP Model')

    self._modelNameEdit = QLineEdit(self)
    self._modelNameEdit.setText('')

    self._rootSystemNameEdit = QLineEdit(self)
    self._rootSystemNameEdit.setText('Root')

    rootSystemGroup = QGroupBox('Root System', self)
    rootSystemLayout = QFormLayout(rootSystemGroup)
    rootSystemLayout.addRow('Name:', self._rootSystemNameEdit)

    buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel, self)
    buttons.accepted.connect(self._onAccept)
    buttons.rejected.connect(self.reject)

    topLayout = QFormLayout()
    topLayout.addRow('Name:', self._modelNameEdit)

    layout = QVBoxLayout(self)
    layout.addLayout(topLayout)
    layout.addWidget(rootSystemGroup)
    layout.addWidget(buttons)

  def _onAccept(self) -> None:
    if self._modelNameEdit.text().strip() and self._rootSystemNameEdit.text().strip():
      self.accept()

  def modelName(self) -> str:
    return self._modelNameEdit.text().strip()

  def rootSystemName(self) -> str:
    return self._rootSystemNameEdit.text().strip()
