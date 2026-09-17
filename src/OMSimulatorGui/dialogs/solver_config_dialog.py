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

'''SolverConfigDialog: name + method + method-specific parameters for one
named entry in a system's `solvers` list (see SSD.newSolver/System.solvers).
Mirrors OMEdit's "SolverSettings - <name> (<method>)" popup: cvode/oms_mav/
oms_mav2 are variable-step (initial/minimum/maximum step size + tolerance),
euler/oms_ma are fixed-step (fixed step size) -- the irrelevant fields for
whichever method is selected are disabled, not hidden, same as OMEdit.

The exact field set per method mirrors InstantiatedModel.instantiate()'s own
method dispatch (the code that actually applies these at simulation time) --
see that method's comments for why relativeTolerance applies unconditionally
while the step-size fields are gated by method.'''

from PySide6.QtWidgets import QComboBox, QDialog, QDialogButtonBox, QFormLayout, QLineEdit, QMessageBox

_VARIABLE_STEP_METHODS = ('cvode', 'oms_mav', 'oms_mav2')
_FIXED_STEP_METHODS = ('euler', 'oms_ma')
_ALL_METHODS = ('cvode', 'euler', 'oms_ma', 'oms_mav', 'oms_mav2')


class SolverConfigDialog(QDialog):
  def __init__(self, config: dict | None = None, parent=None):
    super().__init__(parent)
    config = config or {}
    self.setWindowTitle('Solver Configuration')

    self._nameEdit = QLineEdit(config.get('name', ''), self)

    self._methodCombo = QComboBox(self)
    self._methodCombo.addItems(_ALL_METHODS)
    if config.get('method') in _ALL_METHODS:
      self._methodCombo.setCurrentText(config['method'])
    self._methodCombo.currentTextChanged.connect(self._updateFieldStates)

    self._fixedStepEdit = QLineEdit(str(config.get('fixedStepSize', 0.001)), self)
    self._initialStepEdit = QLineEdit(str(config.get('initialStepSize', 1e-6)), self)
    self._minimumStepEdit = QLineEdit(str(config.get('minimumStepSize', 1e-12)), self)
    self._maximumStepEdit = QLineEdit(str(config.get('maximumStepSize', 0.001)), self)
    self._toleranceEdit = QLineEdit(str(config.get('relativeTolerance', 0.0001)), self)

    buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok | QDialogButtonBox.StandardButton.Cancel, self)
    buttons.accepted.connect(self._onAccept)
    buttons.rejected.connect(self.reject)

    layout = QFormLayout(self)
    layout.addRow('Name:', self._nameEdit)
    layout.addRow('Method:', self._methodCombo)
    layout.addRow('Fixed Step Size:', self._fixedStepEdit)
    layout.addRow('Initial Step Size:', self._initialStepEdit)
    layout.addRow('Minimum Step Size:', self._minimumStepEdit)
    layout.addRow('Maximum Step Size:', self._maximumStepEdit)
    layout.addRow('Relative Tolerance:', self._toleranceEdit)
    layout.addRow(buttons)

    self._updateFieldStates(self._methodCombo.currentText())

  def _updateFieldStates(self, method: str) -> None:
    self._fixedStepEdit.setEnabled(method in _FIXED_STEP_METHODS)
    isVariable = method in _VARIABLE_STEP_METHODS
    self._initialStepEdit.setEnabled(isVariable)
    self._minimumStepEdit.setEnabled(isVariable)
    self._maximumStepEdit.setEnabled(isVariable)

  def _onAccept(self) -> None:
    if not self._nameEdit.text().strip():
      return
    try:
      float(self._fixedStepEdit.text())
      float(self._initialStepEdit.text())
      float(self._minimumStepEdit.text())
      float(self._maximumStepEdit.text())
      float(self._toleranceEdit.text())
    except ValueError:
      QMessageBox.critical(self, 'Invalid value', 'All step size and tolerance fields must be numbers.')
      return
    self.accept()

  def name(self) -> str:
    return self._nameEdit.text().strip()

  def toDict(self) -> dict:
    '''A solvers-list entry (see SSD.newSolver) -- only carries the keys
    relevant to the selected method (plus relativeTolerance, which
    InstantiatedModel applies unconditionally regardless of method).'''
    method = self._methodCombo.currentText()
    result = {'name': self.name(), 'method': method, 'relativeTolerance': float(self._toleranceEdit.text())}
    if method in _FIXED_STEP_METHODS:
      result['fixedStepSize'] = float(self._fixedStepEdit.text())
    if method in _VARIABLE_STEP_METHODS:
      result['initialStepSize'] = float(self._initialStepEdit.text())
      result['minimumStepSize'] = float(self._minimumStepEdit.text())
      result['maximumStepSize'] = float(self._maximumStepEdit.text())
    return result
