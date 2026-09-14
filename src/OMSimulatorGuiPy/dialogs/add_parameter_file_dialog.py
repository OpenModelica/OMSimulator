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

'''AddParameterFileDialog: attach an SSV (parameter values) file, optionally
paired with an SSM (parameter mapping) file, to whichever System/Component
the user picked in the tree or on the canvas -- the actual SSP.addResource +
addSSVReference calls are made by MainWindow, keyed off ssvPath()/ssmPath().

Two modes, switched by radio buttons: browse for files already exported
elsewhere (the same shape SimpleSimulation10.py/SimpleSimulation11.py build
by hand), or author a new SSV -- and, optionally, a companion SSM mapping --
from scratch via a plain value table, exported to disk on accept so the rest
of the attach flow never needs to know which mode produced the files.'''

from pathlib import Path

from PySide6.QtWidgets import (
    QAbstractItemView,
    QCheckBox,
    QComboBox,
    QDialog,
    QDialogButtonBox,
    QFileDialog,
    QFormLayout,
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QMessageBox,
    QPushButton,
    QRadioButton,
    QStackedWidget,
    QTableWidget,
    QVBoxLayout,
    QWidget,
)

from OMSimulator import SSM, SSV

VALUE_TYPES = ('Real', 'Integer', 'Boolean', 'String')


def convertValue(text: str, typeName: str):
  if typeName == 'Real':
    return float(text)
  if typeName == 'Integer':
    return int(text)
  if typeName == 'Boolean':
    return text.strip().lower() in ('1', 'true', 'yes')
  return text


def _fileRow(edit: QLineEdit, browseButton: QPushButton) -> QWidget:
  row = QWidget()
  layout = QHBoxLayout(row)
  layout.setContentsMargins(0, 0, 0, 0)
  layout.addWidget(edit)
  layout.addWidget(browseButton)
  return row


class AddParameterFileDialog(QDialog):
  def __init__(self, parent=None):
    super().__init__(parent)
    self.setWindowTitle('Add Parameter File')
    self.resize(480, 420)

    self._ssvPath: str | None = None
    self._ssmPath: str | None = None

    self._browseRadio = QRadioButton('Use existing file(s)', self)
    self._createRadio = QRadioButton('Create new file', self)
    self._browseRadio.setChecked(True)
    modeRow = QHBoxLayout()
    modeRow.addWidget(self._browseRadio)
    modeRow.addWidget(self._createRadio)
    modeRow.addStretch(1)

    self._stack = QStackedWidget(self)
    self._stack.addWidget(self._buildBrowsePage())
    self._stack.addWidget(self._buildCreatePage())
    self._browseRadio.toggled.connect(lambda checked: self._stack.setCurrentIndex(0 if checked else 1))

    buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel, self)
    buttons.accepted.connect(self._onAccept)
    buttons.rejected.connect(self.reject)

    layout = QVBoxLayout(self)
    layout.addLayout(modeRow)
    layout.addWidget(self._stack)
    layout.addWidget(buttons)

  # --- Browse page -------------------------------------------------------------

  def _buildBrowsePage(self) -> QWidget:
    page = QWidget(self)

    self._ssvEdit = QLineEdit(page)
    ssvBrowse = QPushButton('Browse...', page)
    ssvBrowse.clicked.connect(lambda: self._browseInto(self._ssvEdit, 'Select SSV File', 'SSV files (*.ssv)'))

    self._ssmEdit = QLineEdit(page)
    ssmBrowse = QPushButton('Browse...', page)
    ssmBrowse.clicked.connect(lambda: self._browseInto(self._ssmEdit, 'Select SSM File', 'SSM files (*.ssm)'))

    layout = QFormLayout(page)
    layout.addRow('SSV file:', _fileRow(self._ssvEdit, ssvBrowse))
    layout.addRow('SSM file (optional):', _fileRow(self._ssmEdit, ssmBrowse))
    return page

  def _browseInto(self, edit: QLineEdit, title: str, filterStr: str) -> None:
    path, _ = QFileDialog.getOpenFileName(self, title, '', filterStr)
    if path:
      edit.setText(path)

  # --- Create page ---------------------------------------------------------------

  def _buildCreatePage(self) -> QWidget:
    page = QWidget(self)

    self._valuesTable = QTableWidget(0, 3, page)
    self._valuesTable.setHorizontalHeaderLabels(['Name', 'Value', 'Type'])
    self._valuesTable.setSelectionBehavior(QAbstractItemView.SelectionBehavior.SelectRows)

    addValueButton = QPushButton('Add Row', page)
    addValueButton.clicked.connect(lambda: self._addTableRow(self._valuesTable, withType=True))
    removeValueButton = QPushButton('Remove Row', page)
    removeValueButton.clicked.connect(lambda: self._removeSelectedRow(self._valuesTable))
    valueButtonsRow = QHBoxLayout()
    valueButtonsRow.addWidget(addValueButton)
    valueButtonsRow.addWidget(removeValueButton)
    valueButtonsRow.addStretch(1)

    self._mappingCheck = QCheckBox('Also create a parameter mapping (.ssm)', page)
    self._mappingTable = QTableWidget(0, 2, page)
    self._mappingTable.setHorizontalHeaderLabels(['SSV Variable', 'Target Parameter'])
    self._mappingTable.setSelectionBehavior(QAbstractItemView.SelectionBehavior.SelectRows)
    self._mappingTable.setEnabled(False)
    self._mappingCheck.toggled.connect(self._mappingTable.setEnabled)

    addMappingButton = QPushButton('Add Row', page)
    addMappingButton.clicked.connect(lambda: self._addTableRow(self._mappingTable, withType=False))
    removeMappingButton = QPushButton('Remove Row', page)
    removeMappingButton.clicked.connect(lambda: self._removeSelectedRow(self._mappingTable))
    mappingButtonsRow = QHBoxLayout()
    mappingButtonsRow.addWidget(addMappingButton)
    mappingButtonsRow.addWidget(removeMappingButton)
    mappingButtonsRow.addStretch(1)

    layout = QVBoxLayout(page)
    layout.addWidget(QLabel('Parameter values:', page))
    layout.addWidget(self._valuesTable)
    layout.addLayout(valueButtonsRow)
    layout.addWidget(self._mappingCheck)
    layout.addWidget(self._mappingTable)
    layout.addLayout(mappingButtonsRow)
    return page

  def _addTableRow(self, table: QTableWidget, withType: bool) -> None:
    row = table.rowCount()
    table.insertRow(row)
    if withType:
      typeCombo = QComboBox(table)
      typeCombo.addItems(VALUE_TYPES)
      table.setCellWidget(row, 2, typeCombo)

  def _removeSelectedRow(self, table: QTableWidget) -> None:
    row = table.currentRow()
    if row >= 0:
      table.removeRow(row)

  # --- Accept ----------------------------------------------------------------------

  def _onAccept(self) -> None:
    if self._browseRadio.isChecked():
      self._acceptBrowsed()
    else:
      self._acceptCreated()

  def _acceptBrowsed(self) -> None:
    ssvPath = self._ssvEdit.text().strip()
    if not ssvPath or not Path(ssvPath).is_file():
      QMessageBox.critical(self, 'Add Parameter File', 'Select a valid SSV file.')
      return
    ssmPath = self._ssmEdit.text().strip() or None
    if ssmPath and not Path(ssmPath).is_file():
      QMessageBox.critical(self, 'Add Parameter File', 'Select a valid SSM file, or leave it empty.')
      return
    self._ssvPath = ssvPath
    self._ssmPath = ssmPath
    self.accept()

  def _acceptCreated(self) -> None:
    if self._valuesTable.rowCount() == 0:
      QMessageBox.critical(self, 'Add Parameter File', 'Add at least one parameter value.')
      return

    savePath, _ = QFileDialog.getSaveFileName(self, 'Save SSV File', 'parameters.ssv', 'SSV files (*.ssv)')
    if not savePath:
      return

    ssv = SSV()
    for row in range(self._valuesTable.rowCount()):
      nameItem = self._valuesTable.item(row, 0)
      valueItem = self._valuesTable.item(row, 1)
      typeCombo = self._valuesTable.cellWidget(row, 2)
      name = nameItem.text().strip() if nameItem else ''
      if not name:
        continue
      valueText = valueItem.text().strip() if valueItem else ''
      try:
        value = convertValue(valueText, typeCombo.currentText())
      except ValueError:
        QMessageBox.critical(self, 'Add Parameter File',
                              f"'{valueText}' is not a valid {typeCombo.currentText()} value for '{name}'.")
        return
      ssv.setValue(name, value)
    ssv.export(savePath)
    self._ssvPath = savePath
    self._ssmPath = None

    if self._mappingCheck.isChecked() and self._mappingTable.rowCount() > 0:
      ssmSavePath, _ = QFileDialog.getSaveFileName(self, 'Save SSM File', 'mapping.ssm', 'SSM files (*.ssm)')
      if not ssmSavePath:
        return
      ssm = SSM()
      for row in range(self._mappingTable.rowCount()):
        sourceItem = self._mappingTable.item(row, 0)
        targetItem = self._mappingTable.item(row, 1)
        source = sourceItem.text().strip() if sourceItem else ''
        target = targetItem.text().strip() if targetItem else ''
        if source and target:
          ssm.mapParameter(source, target)
      ssm.export(ssmSavePath)
      self._ssmPath = ssmSavePath

    self.accept()

  def ssvPath(self) -> str | None:
    return self._ssvPath

  def ssmPath(self) -> str | None:
    return self._ssmPath
