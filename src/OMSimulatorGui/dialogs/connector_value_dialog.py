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

'''ConnectorValueDialog: sets a fixed value for a single top-level system
connector, on double-click of its port on the canvas -- the same
"override this signal's value" mechanism ElementPropertiesDialog's Inputs
tab uses for a component's own input connectors (System.setValue/getValue,
via SSP.setValue/getValue), just for one connector at a time. Most useful
for an unconnected input: giving it a constant value to drive whatever
it's wired to internally without needing an actual source component.

Leaving the Value field empty is a deliberate no-op, not an error: the
simulation backend already treats an unset value as 0, so there's no need
to force the user to type "0" explicitly (and no unit field either --
setValue's own unit argument is optional, and this dialog never needs it).

Mirrors ElementPropertiesDialog's own scope limits: only Real/Integer/
Boolean are editable; String stays read-only (nothing to type that would
parse safely). self._value always starts equal to currentValue, only ever
reassigned in _onAccept for an editable type, so value() is safe to call
unconditionally and a Cancel-free "did nothing change" check upstream
(newValue == currentValue) works the same whether the user left an
editable field untouched or the type simply isn't editable at all.'''

from PySide6.QtWidgets import (
    QCheckBox,
    QDialog,
    QDialogButtonBox,
    QFormLayout,
    QLabel,
    QLineEdit,
    QMessageBox,
)

from OMSimulator.variable import SignalType

_EDITABLE_TYPES = (SignalType.Real, SignalType.Integer, SignalType.Boolean)


class ConnectorValueDialog(QDialog):
  def __init__(self, connector, currentValue, parent=None):
    super().__init__(parent)
    self._connector = connector
    self._value = currentValue
    self.setWindowTitle(f'Set Value - {connector.name}')

    editable = connector.signal_type in _EDITABLE_TYPES

    layout = QFormLayout(self)
    layout.addRow('Name:', QLabel(str(connector.name)))
    layout.addRow('Causality:', QLabel(connector.getCausality().name))
    layout.addRow('Type:', QLabel(connector.signal_type.name))

    if connector.signal_type == SignalType.Boolean:
      self._checkBox = QCheckBox(self)
      self._checkBox.setChecked(bool(currentValue) if currentValue is not None else False)
      self._checkBox.setEnabled(editable)
      self._valueEdit = None
      layout.addRow('Value:', self._checkBox)
    else:
      self._checkBox = None
      self._valueEdit = QLineEdit('' if currentValue is None else str(currentValue), self)
      self._valueEdit.setEnabled(editable)
      layout.addRow('Value:', self._valueEdit)

    self._editable = editable
    buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel, self)
    buttons.accepted.connect(self._onAccept)
    buttons.rejected.connect(self.reject)
    layout.addRow(buttons)

  def _onAccept(self) -> None:
    if not self._editable:
      self.accept()
      return
    if self._checkBox is not None:
      self._value = self._checkBox.isChecked()
      self.accept()
      return
    text = self._valueEdit.text().strip()
    if not text:
      # Left blank on purpose -- the backend already defaults an unset
      # value to 0, so there's nothing to force the user to type.
      self._value = None
      self.accept()
      return
    try:
      self._value = float(text) if self._connector.signal_type == SignalType.Real else int(text)
    except ValueError:
      QMessageBox.critical(self, 'Invalid value', f"'{text}' is not a valid {self._connector.signal_type.name} value.")
      return
    self.accept()

  def value(self):
    return self._value
