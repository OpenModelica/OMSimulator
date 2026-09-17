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

'''AddConnectorDialog: name + causality + signal type for a new top-level
system connector (a port on the current system's own boundary).'''

from PySide6.QtWidgets import QComboBox, QDialog, QDialogButtonBox, QFormLayout, QLineEdit

from OMSimulator import Causality, SignalType

_SIGNAL_TYPES = (SignalType.Real, SignalType.Integer, SignalType.Boolean, SignalType.String)


class AddConnectorDialog(QDialog):
  def __init__(self, parent=None):
    super().__init__(parent)
    self.setWindowTitle('Add Connector')

    self._nameEdit = QLineEdit(self)

    self._causalityCombo = QComboBox(self)
    for causality in Causality:
      self._causalityCombo.addItem(causality.name, causality)

    self._typeCombo = QComboBox(self)
    for signalType in _SIGNAL_TYPES:
      self._typeCombo.addItem(signalType.name, signalType)

    buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel, self)
    buttons.accepted.connect(self._onAccept)
    buttons.rejected.connect(self.reject)

    layout = QFormLayout(self)
    layout.addRow('Name:', self._nameEdit)
    layout.addRow('Causality:', self._causalityCombo)
    layout.addRow('Type:', self._typeCombo)
    layout.addRow(buttons)

  def _onAccept(self) -> None:
    if self._nameEdit.text().strip():
      self.accept()

  def name(self) -> str:
    return self._nameEdit.text().strip()

  def causality(self) -> Causality:
    return self._causalityCombo.currentData()

  def signalType(self) -> SignalType:
    return self._typeCombo.currentData()
