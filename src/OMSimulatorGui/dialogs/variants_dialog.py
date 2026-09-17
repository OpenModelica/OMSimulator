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

'''VariantsDialog: lists every variant (SSD) held by an SSP -- an .ssp file
can carry more than one SystemStructure/*.ssd, each a self-contained
alternative model configuration (see ParkerSimPack.ssp in the testsuite for a
real example: D16_auto, D16_aero, D16_aero_stimuli, D16_auto_stimuli). Exactly
one is "active" at a time (SSP.activeVariantName) -- the one that gets
exported as SystemStructure.ssd and the one every SSP-level editing method
(addComponent, addConnection, ...) operates on. SSP._extract_ssp always loads
SystemStructure.ssd first, and SSP._addSSD only defaults activeVariantName
when it's still unset, so the active variant is SystemStructure.ssd's own
variant unless the user has since switched it here.

Activate/Duplicate both mutate `ssp` directly and immediately (there is no
Cancel to undo through) -- unlike ElementPropertiesDialog/
SimulationSettingsDialog's working-copy pattern, there is nothing to stage: a
variant switch or a duplicate is a single atomic, harmless-to-repeat SSP-level
call, not a multi-field form a user might want to back out of half-filled.
MainWindow is responsible for rebuilding its own model-level diagram wrapper
after this dialog closes, since activating a different variant swaps out the
whole system tree the wrapper was built from -- see its own
_onVariantsTriggered.'''

from PySide6.QtCore import Qt
from PySide6.QtWidgets import (
    QDialog,
    QHBoxLayout,
    QInputDialog,
    QListWidget,
    QListWidgetItem,
    QMessageBox,
    QPushButton,
    QVBoxLayout,
)

from OMSimulator import SSP

_NAME_ROLE = Qt.ItemDataRole.UserRole


class VariantsDialog(QDialog):
  def __init__(self, ssp: SSP, parent=None):
    super().__init__(parent)
    self._ssp = ssp
    self.setWindowTitle('SSP Variants')
    self.resize(360, 320)

    self._list = QListWidget(self)
    self._list.itemDoubleClicked.connect(self._onActivate)
    self._refreshList()

    activateButton = QPushButton('Activate', self)
    activateButton.clicked.connect(self._onActivate)
    duplicateButton = QPushButton('Duplicate...', self)
    duplicateButton.clicked.connect(self._onDuplicate)
    closeButton = QPushButton('Close', self)
    closeButton.clicked.connect(self.accept)

    buttonLayout = QVBoxLayout()
    buttonLayout.addWidget(activateButton)
    buttonLayout.addWidget(duplicateButton)
    buttonLayout.addStretch()
    buttonLayout.addWidget(closeButton)

    layout = QHBoxLayout(self)
    layout.addWidget(self._list, 1)
    layout.addLayout(buttonLayout)

  def _refreshList(self) -> None:
    selected = self._selectedVariantName()
    self._list.clear()
    active = self._ssp.activeVariantName
    for name in self._ssp.getAllVariantNames():
      item = QListWidgetItem(f'{name}  (active)' if name == active else name)
      item.setData(_NAME_ROLE, name)
      if name == active:
        font = item.font()
        font.setBold(True)
        item.setFont(font)
      self._list.addItem(item)
      if name == (selected if selected is not None else active):
        self._list.setCurrentItem(item)

  def _selectedVariantName(self) -> str | None:
    item = self._list.currentItem()
    return item.data(_NAME_ROLE) if item is not None else None

  def _onActivate(self) -> None:
    name = self._selectedVariantName()
    if name is None or name == self._ssp.activeVariantName:
      return
    self._ssp.activeVariantName = name
    self._refreshList()

  def _onDuplicate(self) -> None:
    name = self._selectedVariantName()
    if name is None:
      return

    existing = set(self._ssp.getAllVariantNames())
    base = f'{name} copy'
    suggestion = base
    n = 2
    while suggestion in existing:
      suggestion = f'{base} ({n})'
      n += 1

    newName, ok = QInputDialog.getText(self, 'Duplicate Variant', 'New variant name:', text=suggestion)
    newName = newName.strip()
    if not ok or not newName:
      return
    if newName in existing:
      QMessageBox.warning(self, 'Duplicate Variant', f'A variant named "{newName}" already exists.')
      return

    newSsd = self._ssp.getVariant(name).duplicate(newName)
    self._ssp.add(newSsd)
    self._refreshList()
