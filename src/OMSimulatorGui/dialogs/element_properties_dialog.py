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

'''ElementPropertiesDialog: Info/Parameters/Inputs tabs for a single FMU-backed
Component, on double-click (canvas) or "Properties..." (tree). Mirrors OMEdit's
own scope limitation: only Real/Integer/Boolean values are editable, String
parameters are shown read-only, and calculatedParameter connectors are always
read-only since the FMU computes their value itself.

Values are read via Component.getValue() (an override if one was set, else the
FMU's modelDescription.xml start value) and committed back via
Component.setValue() on OK -- both keyed by the connector's own CRef object
(kept consistent, since Values' internal dict does an exact key match: a CRef
and its own str() are NOT interchangeable dict keys despite hashing the same
string, per CRef.__eq__ requiring isinstance(other, CRef)).'''

from PySide6.QtCore import Qt
from PySide6.QtWidgets import (
    QCheckBox,
    QDialog,
    QDialogButtonBox,
    QFormLayout,
    QLabel,
    QLineEdit,
    QMessageBox,
    QTableWidget,
    QTableWidgetItem,
    QTabWidget,
    QVBoxLayout,
    QWidget,
)

from OMSimulator.variable import Causality, SignalType

_EDITABLE_TYPES = (SignalType.Real, SignalType.Integer, SignalType.Boolean)
_CONNECTOR_ROLE = Qt.ItemDataRole.UserRole
_INITIAL_VALUE_ROLE = Qt.ItemDataRole.UserRole + 1


def _asBool(value) -> bool:
  '''Component.getValue() returns either a real Python bool (an override
  previously set via setValue) or the FMU's raw modelDescription.xml start
  value string ("true"/"false", unconverted) -- plain bool(value) would
  misread the latter, since a non-empty string is always truthy.'''
  if isinstance(value, str):
    return value.strip().lower() == 'true'
  return bool(value)


class ElementPropertiesDialog(QDialog):
  def __init__(self, component, parent=None):
    super().__init__(parent)
    self._component = component
    self.setWindowTitle(f'Properties - {component.name}')
    self.resize(520, 420)

    tabs = QTabWidget(self)
    tabs.addTab(self._buildInfoTab(), 'Info')

    parameterConnectors = [c for c in component.connectors
                           if c.causality in (Causality.parameter, Causality.calculatedParameter)]
    self._parametersTable = self._buildValuesTable(parameterConnectors)
    tabs.addTab(self._parametersTable, 'Parameters')

    inputConnectors = [c for c in component.connectors if c.causality == Causality.input]
    self._inputsTable = self._buildValuesTable(inputConnectors)
    tabs.addTab(self._inputsTable, 'Inputs')

    buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel, self)
    buttons.accepted.connect(self._onAccept)
    buttons.rejected.connect(self.reject)

    layout = QVBoxLayout(self)
    layout.addWidget(tabs)
    layout.addWidget(buttons)

  def _buildInfoTab(self) -> QWidget:
    widget = QWidget(self)
    form = QFormLayout(widget)
    form.addRow('Name:', QLabel(str(self._component.name)))
    form.addRow('FMU:', QLabel(self._component.fmuPath))
    self._descriptionEdit = QLineEdit(widget)
    self._descriptionEdit.setText(self._component.description or '')
    form.addRow('Description:', self._descriptionEdit)
    return widget

  def _buildValuesTable(self, connectors) -> QTableWidget:
    table = QTableWidget(len(connectors), 4, self)
    table.setHorizontalHeaderLabels(['Name', 'Type', 'Value', 'Unit'])
    table.horizontalHeader().setStretchLastSection(True)
    table.verticalHeader().setVisible(False)

    for row, connector in enumerate(connectors):
      nameItem = QTableWidgetItem(str(connector.name))
      nameItem.setFlags(nameItem.flags() & ~Qt.ItemFlag.ItemIsEditable)
      table.setItem(row, 0, nameItem)

      typeItem = QTableWidgetItem(connector.signal_type.name)
      typeItem.setFlags(typeItem.flags() & ~Qt.ItemFlag.ItemIsEditable)
      table.setItem(row, 1, typeItem)

      editable = connector.causality != Causality.calculatedParameter and connector.signal_type in _EDITABLE_TYPES
      currentValue = self._component.getValue(connector.name)
      if connector.signal_type == SignalType.Boolean:
        initialChecked = _asBool(currentValue)
        checkBox = QCheckBox(table)
        checkBox.setChecked(initialChecked)
        checkBox.setEnabled(editable)
        checkBox.setProperty('connector', connector)
        checkBox.setProperty('initialChecked', initialChecked)
        table.setCellWidget(row, 2, checkBox)
      else:
        initialText = '' if currentValue is None else str(currentValue)
        valueItem = QTableWidgetItem(initialText)
        if not editable:
          valueItem.setFlags(valueItem.flags() & ~Qt.ItemFlag.ItemIsEditable)
        valueItem.setData(_CONNECTOR_ROLE, connector)
        valueItem.setData(_INITIAL_VALUE_ROLE, initialText)
        table.setItem(row, 2, valueItem)

      unitItem = QTableWidgetItem(connector.unit or '')
      unitItem.setFlags(unitItem.flags() & ~Qt.ItemFlag.ItemIsEditable)
      table.setItem(row, 3, unitItem)

    return table

  def _onAccept(self) -> None:
    try:
      self._applyTable(self._parametersTable)
      self._applyTable(self._inputsTable)
    except ValueError as e:
      QMessageBox.critical(self, 'Invalid value', str(e))
      return
    self._component.description = self._descriptionEdit.text().strip() or None
    self.accept()

  def _applyTable(self, table: QTableWidget) -> None:
    for row in range(table.rowCount()):
      checkBox = table.cellWidget(row, 2)
      if isinstance(checkBox, QCheckBox):
        if not checkBox.isEnabled() or checkBox.isChecked() == checkBox.property('initialChecked'):
          continue
        connector = checkBox.property('connector')
        self._component.setValue(connector.name, checkBox.isChecked())
        continue

      item = table.item(row, 2)
      connector = item.data(_CONNECTOR_ROLE)
      if connector is None:
        continue
      if connector.causality == Causality.calculatedParameter or connector.signal_type not in _EDITABLE_TYPES:
        # Read-only rows (calculated parameters, String/Enumeration/unmapped
        # types): the displayed text is whatever was there originally, never
        # user-edited, and for a non-numeric type it wouldn't parse as one --
        # e.g. int("Set me!") for an untouched String parameter.
        continue
      text = item.text().strip()
      if not text or text == item.data(_INITIAL_VALUE_ROLE):
        # Leave untouched fields alone -- writing the FMU's own default back
        # as an explicit override on every OK would add unnecessary
        # <ssd:ParameterBindings> to the exported model for values the user
        # never actually changed.
        continue
      try:
        value = float(text) if connector.signal_type == SignalType.Real else int(text)
      except ValueError:
        raise ValueError(f"'{text}' is not a valid {connector.signal_type.name} value for '{connector.name}'")
      unit = connector.unit if connector.signal_type == SignalType.Real else None
      self._component.setValue(connector.name, value, unit=unit)
