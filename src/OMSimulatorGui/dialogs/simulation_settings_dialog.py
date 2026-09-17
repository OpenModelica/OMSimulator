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

'''SimulationSettingsDialog: edits the active SSD's own experiment settings
(General tab: start/stop time, tolerance, step size, result file, result
file buffer size, logging interval -- the same fields shown on OMEdit's own
"Simulation Setup > General" tab, persisted on the model itself) plus named
solver configurations and per-component solver assignments (Solver Settings
tab), matching OMEdit's "Simulation Setup" dialog.

Solver configurations are `system.solvers` -- a list of dicts (see
SSD.newSolver) each shaped by SolverConfigDialog. A component left
unassigned (`element.solver is None`) falls back to the automatic defaults
System.processElements picks at export time (oms_me_solver for ME FMUs,
oms_ma_default for CS FMUs/tables) -- this dialog only ever writes an
explicit assignment when the user actually picks one, same "don't write
data nobody asked for" reasoning as ElementPropertiesDialog's parameter
tab.

Unlike the simpler Add* dialogs (which only collect and let MainWindow
apply), this one applies directly to the `ssd` object on OK -- the working
copies (`_solverConfigs`, `_pendingAssignments`) exist so Cancel truly
discards everything, including solver renames/removals cascaded into
in-progress (not yet committed) component assignments.'''

from PySide6.QtCore import Qt
from PySide6.QtWidgets import (
    QComboBox,
    QDialog,
    QDialogButtonBox,
    QFormLayout,
    QGroupBox,
    QHBoxLayout,
    QLineEdit,
    QMessageBox,
    QPushButton,
    QTableWidget,
    QTableWidgetItem,
    QTabWidget,
    QVBoxLayout,
    QWidget,
)

from OMSimulator import System

from OMSimulatorGui.dialogs.solver_config_dialog import SolverConfigDialog

_NONE_SOLVER = '(none)'


def _collectComponents(system: System, prefix: str = '') -> list[tuple[str, object]]:
  '''Every Component/ComponentTable in the model, recursively, labeled with
  a dotted path relative to the root (bare name for a top-level component,
  "subsystem.name" for a nested one) -- matches OMEdit's own "Component
  Assignments" table, which lists every leaf component in the whole model,
  not just the current system's own direct children.'''
  result = []
  for name, element in system.elements.items():
    path = f'{prefix}{name}' if not prefix else f'{prefix}.{name}'
    if isinstance(element, System):
      result.extend(_collectComponents(element, path))
    else:
      result.append((path, element))
  return result


class SimulationSettingsDialog(QDialog):
  def __init__(self, ssd, parent=None):
    super().__init__(parent)
    self._ssd = ssd
    self.setWindowTitle(f'Simulation Setup - {ssd.name}')

    tabs = QTabWidget(self)
    tabs.addTab(self._buildGeneralTab(ssd), 'General')
    tabs.addTab(self._buildSolverTab(ssd), 'Solver Settings')

    buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel, self)
    buttons.accepted.connect(self._onAccept)
    buttons.rejected.connect(self.reject)

    layout = QVBoxLayout(self)
    layout.addWidget(tabs)
    layout.addWidget(buttons)

  # --- General tab ---------------------------------------------------------

  def _buildGeneralTab(self, ssd) -> QWidget:
    widget = QWidget(self)
    self._startTimeEdit = QLineEdit(str(ssd.startTime), widget)
    self._stopTimeEdit = QLineEdit(str(ssd.stopTime), widget)
    self._toleranceEdit = QLineEdit(str(ssd.tolerance), widget)
    self._stepSizeEdit = QLineEdit(str(ssd.maximumStepSize), widget)
    self._resultFileEdit = QLineEdit(ssd.resultFile, widget)
    self._bufferSizeEdit = QLineEdit(str(ssd.bufferSize), widget)
    self._loggingIntervalEdit = QLineEdit(str(ssd.loggingInterval), widget)

    layout = QFormLayout(widget)
    layout.addRow('Start Time:', self._startTimeEdit)
    layout.addRow('Stop Time:', self._stopTimeEdit)
    layout.addRow('Tolerance:', self._toleranceEdit)
    layout.addRow('Step Size:', self._stepSizeEdit)
    layout.addRow('Result File:', self._resultFileEdit)
    layout.addRow('Result File Buffer Size:', self._bufferSizeEdit)
    layout.addRow('Logging Interval:', self._loggingIntervalEdit)
    return widget

  def startTime(self) -> float:
    return float(self._startTimeEdit.text())

  def stopTime(self) -> float:
    return float(self._stopTimeEdit.text())

  def tolerance(self) -> float:
    return float(self._toleranceEdit.text())

  def stepSize(self) -> float:
    return float(self._stepSizeEdit.text())

  def resultFile(self) -> str:
    return self._resultFileEdit.text().strip()

  def bufferSize(self) -> int:
    return int(self._bufferSizeEdit.text())

  def loggingInterval(self) -> float:
    return float(self._loggingIntervalEdit.text())

  # --- Solver Settings tab --------------------------------------------------

  def _buildSolverTab(self, ssd) -> QWidget:
    widget = QWidget(self)
    layout = QVBoxLayout(widget)

    self._solverConfigs = [dict(s) for s in ssd.system.solvers]
    self._components = _collectComponents(ssd.system)
    self._pendingAssignments = {id(element): element.solver for _, element in self._components}

    configGroup = QGroupBox('Solver Configurations', widget)
    configLayout = QVBoxLayout(configGroup)
    self._solverTable = QTableWidget(0, 2, configGroup)
    self._solverTable.setHorizontalHeaderLabels(['Name', 'Method'])
    self._solverTable.horizontalHeader().setStretchLastSection(True)
    self._solverTable.verticalHeader().setVisible(False)
    self._solverTable.setSelectionBehavior(QTableWidget.SelectionBehavior.SelectRows)
    self._solverTable.setEditTriggers(QTableWidget.EditTrigger.NoEditTriggers)
    configLayout.addWidget(self._solverTable)

    buttonRow = QHBoxLayout()
    addButton = QPushButton('Add', configGroup)
    removeButton = QPushButton('Remove', configGroup)
    editButton = QPushButton('Edit', configGroup)
    addButton.clicked.connect(self._onAddSolver)
    removeButton.clicked.connect(self._onRemoveSolver)
    editButton.clicked.connect(self._onEditSolver)
    buttonRow.addWidget(addButton)
    buttonRow.addWidget(removeButton)
    buttonRow.addWidget(editButton)
    buttonRow.addStretch()
    configLayout.addLayout(buttonRow)
    layout.addWidget(configGroup)

    assignGroup = QGroupBox('Component Assignments', widget)
    assignLayout = QVBoxLayout(assignGroup)
    self._assignmentTable = QTableWidget(0, 2, assignGroup)
    self._assignmentTable.setHorizontalHeaderLabels(['Component', 'Solver'])
    self._assignmentTable.horizontalHeader().setStretchLastSection(True)
    self._assignmentTable.verticalHeader().setVisible(False)
    assignLayout.addWidget(self._assignmentTable)
    layout.addWidget(assignGroup)

    self._refreshSolverTable()
    self._refreshAssignmentTable()
    return widget

  def _solverNames(self) -> list[str]:
    return [config.get('name', '') for config in self._solverConfigs]

  def _refreshSolverTable(self) -> None:
    self._solverTable.setRowCount(len(self._solverConfigs))
    for row, config in enumerate(self._solverConfigs):
      nameItem = QTableWidgetItem(config.get('name', ''))
      nameItem.setFlags(nameItem.flags() & ~Qt.ItemFlag.ItemIsEditable)
      self._solverTable.setItem(row, 0, nameItem)
      methodItem = QTableWidgetItem(config.get('method', ''))
      methodItem.setFlags(methodItem.flags() & ~Qt.ItemFlag.ItemIsEditable)
      self._solverTable.setItem(row, 1, methodItem)

  def _refreshAssignmentTable(self) -> None:
    names = self._solverNames()
    self._assignmentTable.setRowCount(len(self._components))
    for row, (path, element) in enumerate(self._components):
      nameItem = QTableWidgetItem(path)
      nameItem.setFlags(nameItem.flags() & ~Qt.ItemFlag.ItemIsEditable)
      self._assignmentTable.setItem(row, 0, nameItem)

      combo = QComboBox(self._assignmentTable)
      combo.addItem(_NONE_SOLVER)
      combo.addItems(names)
      pending = self._pendingAssignments.get(id(element))
      combo.setCurrentText(pending if pending in names else _NONE_SOLVER)
      combo.currentTextChanged.connect(self._makeAssignmentHandler(id(element)))
      self._assignmentTable.setCellWidget(row, 1, combo)

  def _makeAssignmentHandler(self, elementId: int):
    def handler(text: str) -> None:
      self._pendingAssignments[elementId] = None if text == _NONE_SOLVER else text
    return handler

  def _defaultSolverName(self) -> str:
    '''"solver1", "solver2", ... -- skipping any name already taken (not
    just counting existing solvers), matching OMEdit's own default naming
    for a newly-added solver configuration.'''
    existing = set(self._solverNames())
    n = 1
    while f'solver{n}' in existing:
      n += 1
    return f'solver{n}'

  def _onAddSolver(self) -> None:
    # oms_ma as the starting method: it's the same one System.processElements
    # falls back to automatically for an unassigned CS FMU/table, so a fresh
    # manually-added solver defaults to matching what "no solver" already means.
    dialog = SolverConfigDialog(config={'name': self._defaultSolverName(), 'method': 'oms_ma'}, parent=self)
    if dialog.exec() != QDialog.DialogCode.Accepted:
      return
    if dialog.name() in self._solverNames():
      QMessageBox.critical(self, 'Duplicate name', f"A solver named '{dialog.name()}' already exists.")
      return
    self._solverConfigs.append(dialog.toDict())
    self._refreshSolverTable()
    self._refreshAssignmentTable()

  def _onEditSolver(self) -> None:
    row = self._solverTable.currentRow()
    if row < 0:
      return
    oldName = self._solverConfigs[row].get('name')
    dialog = SolverConfigDialog(config=self._solverConfigs[row], parent=self)
    if dialog.exec() != QDialog.DialogCode.Accepted:
      return
    newName = dialog.name()
    if newName != oldName and newName in self._solverNames():
      QMessageBox.critical(self, 'Duplicate name', f"A solver named '{newName}' already exists.")
      return
    self._solverConfigs[row] = dialog.toDict()
    if newName != oldName:
      # Carry any in-progress (uncommitted) assignment pointing at the old
      # name forward to the new one, rather than silently orphaning it.
      for elementId, assigned in self._pendingAssignments.items():
        if assigned == oldName:
          self._pendingAssignments[elementId] = newName
    self._refreshSolverTable()
    self._refreshAssignmentTable()

  def _onRemoveSolver(self) -> None:
    row = self._solverTable.currentRow()
    if row < 0:
      return
    removedName = self._solverConfigs[row].get('name')
    del self._solverConfigs[row]
    for elementId, assigned in self._pendingAssignments.items():
      if assigned == removedName:
        self._pendingAssignments[elementId] = None
    self._refreshSolverTable()
    self._refreshAssignmentTable()

  # --- Commit ----------------------------------------------------------------

  def _onAccept(self) -> None:
    try:
      self.startTime()
      self.stopTime()
      self.tolerance()
      self.stepSize()
      self.bufferSize()
      self.loggingInterval()
    except ValueError:
      return
    if not self._resultFileEdit.text().strip():
      return

    self._ssd.startTime = self.startTime()
    self._ssd.stopTime = self.stopTime()
    self._ssd.tolerance = self.tolerance()
    self._ssd.maximumStepSize = self.stepSize()
    self._ssd.resultFile = self.resultFile()
    self._ssd.bufferSize = self.bufferSize()
    self._ssd.loggingInterval = self.loggingInterval()

    self._ssd.system.solvers = list(self._solverConfigs)
    for _, element in self._components:
      element.solver = self._pendingAssignments.get(id(element))

    self.accept()
