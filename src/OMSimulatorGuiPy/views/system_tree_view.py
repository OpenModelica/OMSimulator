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

'''QTreeView subclass for the system tree.

Context menus emit request signals carrying the target TreeNode; MainWindow
owns the actual System/SSP API calls and the dialogs, keeping this view
free of editing-API specifics.
'''

from PySide6.QtCore import Qt, Signal
from PySide6.QtWidgets import QMenu, QTreeView

from OMSimulatorGui.models.system_tree_model import (
    KIND_COMPONENT,
    KIND_COMPONENT_TABLE,
    KIND_CONNECTOR,
    KIND_MODEL,
    KIND_PARAMETER_FILE,
    KIND_RESOURCE,
    KIND_SYSTEM,
)

_DELETABLE_KINDS = (KIND_MODEL, KIND_SYSTEM, KIND_COMPONENT, KIND_COMPONENT_TABLE, KIND_CONNECTOR)


class SystemTreeView(QTreeView):
  addSystemRequested = Signal(object)          # TreeNode: parent system to add into
  addComponentRequested = Signal(object)       # TreeNode: parent system to add into
  addConnectorRequested = Signal(object)       # TreeNode: parent system to add into
  addParameterFileRequested = Signal(object)   # TreeNode: system/component to attach to
  editParameterFileRequested = Signal(object)  # TreeNode: the parameter-file entry to edit
  removeParameterFileRequested = Signal(object)  # TreeNode: the parameter-file entry to remove
  swapParameterFileRequested = Signal(object)  # TreeNode: the parameter-file entry to swap
  addResourceRequested = Signal(object)        # TreeNode: the model row to add a resource to
  removeResourceRequested = Signal(object)     # TreeNode: the resource entry to remove
  deleteRequested = Signal(object)        # TreeNode: element/connector to delete
  renameRequested = Signal(object)        # TreeNode: element to rename
  propertiesRequested = Signal(object)    # TreeNode: FMU component to show properties for

  def __init__(self, parent=None):
    super().__init__(parent)
    self.setHeaderHidden(False)
    self.setUniformRowHeights(True)
    self.setExpandsOnDoubleClick(True)
    self.setContextMenuPolicy(self.contextMenuPolicy().CustomContextMenu)
    self.customContextMenuRequested.connect(self._onContextMenuRequested)
    self.doubleClicked.connect(self._onDoubleClicked)

  def _onDoubleClicked(self, index) -> None:
    node = self.model().nodeFromIndex(index)
    if node is None:
      return
    if node.kind == KIND_COMPONENT:
      self.propertiesRequested.emit(node)
    elif node.kind == KIND_PARAMETER_FILE:
      self.editParameterFileRequested.emit(node)

  def keyPressEvent(self, event) -> None:
    if event.key() == Qt.Key.Key_Delete:
      node = self.model().nodeFromIndex(self.currentIndex())
      if node is not None and node.kind == KIND_PARAMETER_FILE:
        self.removeParameterFileRequested.emit(node)
        event.accept()
        return
      if node is not None and node.kind == KIND_RESOURCE:
        self.removeResourceRequested.emit(node)
        event.accept()
        return
      if node is not None and node.kind in _DELETABLE_KINDS:
        self.deleteRequested.emit(node)
        event.accept()
        return
    super().keyPressEvent(event)

  def _onContextMenuRequested(self, pos) -> None:
    index = self.indexAt(pos)
    if not index.isValid():
      return
    node = self.model().nodeFromIndex(index)
    if node is None:
      return

    menu = QMenu(self)

    if node.kind == KIND_SYSTEM:
      menu.addAction('Add System...', lambda: self.addSystemRequested.emit(node))
      menu.addAction('Add Component...', lambda: self.addComponentRequested.emit(node))
      menu.addAction('Add Connector...', lambda: self.addConnectorRequested.emit(node))
      menu.addAction('Add Parameter File...', lambda: self.addParameterFileRequested.emit(node))
      menu.addSeparator()
      menu.addAction('Rename...', lambda: self.renameRequested.emit(node))
      menu.addAction('Delete', lambda: self.deleteRequested.emit(node))
    elif node.kind in (KIND_COMPONENT, KIND_COMPONENT_TABLE):
      if node.kind == KIND_COMPONENT:
        menu.addAction('Properties...', lambda: self.propertiesRequested.emit(node))
        menu.addSeparator()
      menu.addAction('Add Parameter File...', lambda: self.addParameterFileRequested.emit(node))
      menu.addSeparator()
      menu.addAction('Rename...', lambda: self.renameRequested.emit(node))
      menu.addAction('Delete', lambda: self.deleteRequested.emit(node))
    elif node.kind == KIND_CONNECTOR:
      menu.addAction('Delete', lambda: self.deleteRequested.emit(node))
    elif node.kind == KIND_PARAMETER_FILE:
      menu.addAction('Edit Values...', lambda: self.editParameterFileRequested.emit(node))
      menu.addAction('Swap Resource...', lambda: self.swapParameterFileRequested.emit(node))
      menu.addAction('Remove', lambda: self.removeParameterFileRequested.emit(node))
    elif node.kind == KIND_MODEL:
      menu.addAction('Add Resource...', lambda: self.addResourceRequested.emit(node))
      menu.addSeparator()
      menu.addAction('Rename...', lambda: self.renameRequested.emit(node))
      menu.addAction('Delete', lambda: self.deleteRequested.emit(node))
    elif node.kind == KIND_RESOURCE:
      menu.addAction('Remove', lambda: self.removeResourceRequested.emit(node))
    else:
      return

    menu.exec(self.viewport().mapToGlobal(pos))
