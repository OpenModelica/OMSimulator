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

'''EditParameterFileDialog: edit the values already attached via an SSV
resource (and, if one is attached alongside it, the companion SSM mapping
too) -- opened by double-clicking a component/system's "P" badge on the
canvas, or "Edit Values..."/double-click on its tree entry.

Reuses AddParameterFileDialog's table shapes (Name/Value/Type for values,
SSV Variable/Target Parameter for mapping), just pre-populated from the live
SSV/SSM objects (SSP.resources[resourceName], already parsed from disk by
SSP.addResource -- see ssp.py's _addResource) instead of starting empty.
Writes back into those SAME instances and re-exports each to its own file
(ssv.filename/ssm.filename) rather than creating new ones, so no
SSP.addResource/addSSVReference bookkeeping is needed -- the references
already point at these files, only their content changes.

If there's no companion SSM yet, a checkbox offers to add one, either by
authoring a new one inline or by picking one already sitting in the SSP's
resource pool (e.g. one added via "Add Resource..." or left over from a
previous attachment elsewhere -- see the screenshot-driven follow-up request
this was added for: the user had a "mapping.ssm" already registered and
wanted to reuse it instead of retyping the same rows into a new file). A
*brand-new* mapping resource needs an SSP.addResource + addSSVReference call
to actually get attached (unlike editing an existing one, which is pure file
content), and this dialog has no cref to call those with -- so in that case
it only exports the new .ssm to disk and exposes the path via newSsmPath().
Picking an *existing* resource needs no export at all, just addSSVReference
with the resource name already in hand -- exposed via existingSsmResource().
Either way, MainWindow._editParameterFileResource does the actual attach
afterwards, the same division of labor AddParameterFileDialog already has
with its own ssvPath()/ssmPath() accessors.'''

from pathlib import Path

from PySide6.QtWidgets import (
    QAbstractItemView,
    QCheckBox,
    QComboBox,
    QDialog,
    QDialogButtonBox,
    QFileDialog,
    QHBoxLayout,
    QLabel,
    QMessageBox,
    QPushButton,
    QTableWidget,
    QTableWidgetItem,
    QVBoxLayout,
)

from OMSimulator import SSM

from OMSimulatorGui.dialogs.add_parameter_file_dialog import VALUE_TYPES, convertValue


def _typeName(value, valueType) -> str:
  '''valueType is None whenever a value was set via Values.setValue without
  an explicit SignalType -- which is exactly what this dialog's own
  _onAccept does (it lets export()'s own type inference, based on the native
  Python value, pick the SSV type tag). Editing a file a second time before
  it's been saved-and-reloaded would otherwise see that None and default
  every such row to 'Real', silently turning an Integer/Boolean value into
  one -- so infer from the live Python value here the same way
  Values._getVariableType does at export time (bool before int, since bool
  is a subclass of int), instead of guessing.'''
  if valueType is not None:
    return valueType.name
  if isinstance(value, bool):
    return 'Boolean'
  if isinstance(value, int):
    return 'Integer'
  if isinstance(value, str):
    return 'String'
  return 'Real'


_CREATE_NEW_SSM = None  # QComboBox item data sentinel for "author a new mapping" vs. an existing resource name


class EditParameterFileDialog(QDialog):
  def __init__(self, ssv, ssm=None, availableSsmResources=None, parent=None):
    super().__init__(parent)
    self._ssv = ssv
    self._ssm = ssm
    self._newSsmPath: str | None = None
    self._existingSsmResource: str | None = None
    self._mappingCheck = None
    self._mappingSourceCombo = None
    title = Path(ssv.filename).name if ssv.filename else 'parameters.ssv'
    self.setWindowTitle(f'Edit Parameter Values - {title}')
    self.resize(420, 480)

    self._table = QTableWidget(0, 3, self)
    self._table.setHorizontalHeaderLabels(['Name', 'Value', 'Type'])
    self._table.setSelectionBehavior(QAbstractItemView.SelectionBehavior.SelectRows)
    for name, (value, valueType, _unit, _description) in ssv.value.getAllParameters().items():
      self._addRow(str(name), value, _typeName(value, valueType))

    addButton = QPushButton('Add Row', self)
    addButton.clicked.connect(lambda: self._addRow('', '', 'Real'))
    removeButton = QPushButton('Remove Row', self)
    removeButton.clicked.connect(self._removeSelectedRow)
    buttonsRow = QHBoxLayout()
    buttonsRow.addWidget(addButton)
    buttonsRow.addWidget(removeButton)
    buttonsRow.addStretch(1)

    layout = QVBoxLayout(self)
    layout.addWidget(QLabel('Parameter values:', self))
    layout.addWidget(self._table)
    layout.addLayout(buttonsRow)

    self._mappingTable = QTableWidget(0, 2, self)
    self._mappingTable.setHorizontalHeaderLabels(['SSV Variable', 'Target Parameter'])
    self._mappingTable.setSelectionBehavior(QAbstractItemView.SelectionBehavior.SelectRows)
    if ssm is not None:
      for source, targets in ssm.mappingEntry.items():
        for entry in targets:
          self._addMappingRow(str(source), str(entry['target']))
    else:
      # No mapping attached yet -- offer to create one, same idea as
      # AddParameterFileDialog's own optional-mapping checkbox. Starts
      # disabled/empty so an untouched dialog can't accidentally attach a
      # blank mapping.
      self._mappingTable.setEnabled(False)

    self._addMappingButton = QPushButton('Add Row', self)
    self._addMappingButton.clicked.connect(lambda: self._addMappingRow('', ''))
    self._removeMappingButton = QPushButton('Remove Row', self)
    self._removeMappingButton.clicked.connect(self._removeSelectedMappingRow)
    if ssm is None:
      self._addMappingButton.setEnabled(False)
      self._removeMappingButton.setEnabled(False)
    mappingButtonsRow = QHBoxLayout()
    mappingButtonsRow.addWidget(self._addMappingButton)
    mappingButtonsRow.addWidget(self._removeMappingButton)
    mappingButtonsRow.addStretch(1)

    if ssm is not None:
      layout.addWidget(QLabel('Parameter mapping:', self))
    else:
      self._mappingCheck = QCheckBox('Add a parameter mapping (.ssm)', self)
      layout.addWidget(self._mappingCheck)

      self._mappingSourceCombo = QComboBox(self)
      self._mappingSourceCombo.addItem('Create new...', _CREATE_NEW_SSM)
      for resourceName in (availableSsmResources or []):
        self._mappingSourceCombo.addItem(Path(resourceName).name, resourceName)
      self._mappingSourceCombo.setEnabled(False)
      self._mappingSourceCombo.currentIndexChanged.connect(self._onMappingSourceChanged)
      sourceRow = QHBoxLayout()
      sourceRow.addWidget(QLabel('Source:', self))
      sourceRow.addWidget(self._mappingSourceCombo)
      layout.addLayout(sourceRow)

      self._mappingCheck.toggled.connect(self._onMappingCheckToggled)
    layout.addWidget(self._mappingTable)
    layout.addLayout(mappingButtonsRow)

    buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel, self)
    buttons.accepted.connect(self._onAccept)
    buttons.rejected.connect(self.reject)
    layout.addWidget(buttons)

  def _addRow(self, name: str, value, typeName: str) -> None:
    row = self._table.rowCount()
    self._table.insertRow(row)
    self._table.setItem(row, 0, QTableWidgetItem(name))
    self._table.setItem(row, 1, QTableWidgetItem('' if value in (None, '') else str(value)))
    typeCombo = QComboBox(self._table)
    typeCombo.addItems(VALUE_TYPES)
    typeCombo.setCurrentText(typeName)
    self._table.setCellWidget(row, 2, typeCombo)

  def _removeSelectedRow(self) -> None:
    row = self._table.currentRow()
    if row >= 0:
      self._table.removeRow(row)

  def _onMappingCheckToggled(self, checked: bool) -> None:
    self._mappingSourceCombo.setEnabled(checked)
    self._updateMappingTableEnabled()

  def _onMappingSourceChanged(self, _index: int) -> None:
    self._updateMappingTableEnabled()

  def _updateMappingTableEnabled(self) -> None:
    '''The table (and its Add/Remove Row buttons) are only for *authoring* a
    new mapping -- picking an already-registered .ssm resource instead has
    nothing to edit inline here, its content is whatever's already on disk.'''
    isCreatingNew = self._mappingCheck.isChecked() and self._mappingSourceCombo.currentData() is _CREATE_NEW_SSM
    self._mappingTable.setEnabled(isCreatingNew)
    self._addMappingButton.setEnabled(isCreatingNew)
    self._removeMappingButton.setEnabled(isCreatingNew)

  def _addMappingRow(self, source: str, target: str) -> None:
    row = self._mappingTable.rowCount()
    self._mappingTable.insertRow(row)
    self._mappingTable.setItem(row, 0, QTableWidgetItem(source))
    self._mappingTable.setItem(row, 1, QTableWidgetItem(target))

  def _removeSelectedMappingRow(self) -> None:
    row = self._mappingTable.currentRow()
    if row >= 0:
      self._mappingTable.removeRow(row)

  def _onAccept(self) -> None:
    # Validate and stage everything *before* mutating/exporting anything --
    # a cancelled "where should the new .ssm go?" picker (see below) must
    # leave the dialog exactly as it was, not half-applied with the value
    # edits already written to disk but the mapping silently dropped.
    newValues = {}
    for row in range(self._table.rowCount()):
      nameItem = self._table.item(row, 0)
      valueItem = self._table.item(row, 1)
      typeCombo = self._table.cellWidget(row, 2)
      name = nameItem.text().strip() if nameItem else ''
      if not name:
        continue
      valueText = valueItem.text().strip() if valueItem else ''
      try:
        newValues[name] = convertValue(valueText, typeCombo.currentText())
      except ValueError:
        QMessageBox.critical(self, 'Edit Parameter Values',
                              f"'{valueText}' is not a valid {typeCombo.currentText()} value for '{name}'.")
        return

    newSsm = None
    savePath = None
    pickedExistingResource = None
    if self._ssm is None and self._mappingCheck is not None and self._mappingCheck.isChecked():
      selectedResource = self._mappingSourceCombo.currentData()
      if selectedResource is not _CREATE_NEW_SSM:
        pickedExistingResource = selectedResource
      else:
        newSsm = SSM()
        for row in range(self._mappingTable.rowCount()):
          sourceItem = self._mappingTable.item(row, 0)
          targetItem = self._mappingTable.item(row, 1)
          source = sourceItem.text().strip() if sourceItem else ''
          target = targetItem.text().strip() if targetItem else ''
          if source and target:
            newSsm.mapParameter(source, target)
        # Checking the box means "I want a mapping file", full stop -- prompt
        # for where to save it even with zero rows, so an intentionally empty
        # mapping can still be created (to fill in later).
        savePath, _ = QFileDialog.getSaveFileName(self, 'Save SSM File', 'mapping.ssm', 'SSM files (*.ssm)')
        if not savePath:
          return  # cancelled -- nothing has been touched yet, leave the dialog open

    # Rebuilt wholesale rather than patched in place -- lets rows be renamed
    # or removed too, not just have their value edited, the same way the
    # table itself allows.
    self._ssv.value.start_values = {}
    for name, value in newValues.items():
      self._ssv.setValue(name, value)
    if self._ssv.filename:
      self._ssv.export(str(self._ssv.filename))

    if self._ssm is not None:
      self._ssm.mappingEntry.clear()
      for row in range(self._mappingTable.rowCount()):
        sourceItem = self._mappingTable.item(row, 0)
        targetItem = self._mappingTable.item(row, 1)
        source = sourceItem.text().strip() if sourceItem else ''
        target = targetItem.text().strip() if targetItem else ''
        if source and target:
          self._ssm.mapParameter(source, target)
      # Written even if every row was removed -- SSM.export() now always
      # writes (an emptied mapping table means "clear the mapping", not
      # "leave the old file's content in place").
      if self._ssm.filename:
        self._ssm.export(str(self._ssm.filename))
    elif newSsm is not None and savePath is not None:
      newSsm.export(savePath)
      self._newSsmPath = savePath
    elif pickedExistingResource is not None:
      self._existingSsmResource = pickedExistingResource

    self.accept()

  def newSsmPath(self) -> str | None:
    '''Set only when the user checked "Add a parameter mapping", left the
    source on "Create new...", and accepted -- MainWindow is responsible for
    actually attaching it (SSP.addResource + addSSVReference), since that
    needs a cref this dialog doesn't have.'''
    return self._newSsmPath

  def existingSsmResource(self) -> str | None:
    '''Set when the user checked "Add a parameter mapping" and picked an
    already-registered .ssm resource instead of authoring a new one --
    already in the SSP's resource pool, so MainWindow only needs to
    addSSVReference with it directly, no addResource call first.'''
    return self._existingSsmResource
