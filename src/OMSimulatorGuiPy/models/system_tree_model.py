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

'''QAbstractItemModel exposing an OMSimulator System tree to a QTreeView.

Reads directly off the live OMSimulatorPython objects (System/Component/
ComponentTable/Connector/Connection) -- no mirrored dataclass tree. The
model snapshots the current shape into lightweight TreeNode wrappers on
setSystem()/refresh() so Qt's index()/parent() can be O(1); it does not
copy or duplicate the underlying model data itself (TreeNode.obj is the
actual System/Component/Connector/Connection instance).
'''

from pathlib import Path

from PySide6.QtCore import QAbstractItemModel, QModelIndex, Qt

from OMSimulator import Component, ComponentTable, Connection, Connector, System

# TreeNode.kind values
KIND_MODEL = 'model'
KIND_SYSTEM = 'system'
KIND_COMPONENT = 'component'
KIND_COMPONENT_TABLE = 'componenttable'
KIND_CONNECTORS_GROUP = 'connectors_group'
KIND_CONNECTOR = 'connector'
KIND_CONNECTIONS_GROUP = 'connections_group'
KIND_CONNECTION = 'connection'
KIND_PARAMETER_FILES_GROUP = 'parameter_files_group'
KIND_PARAMETER_FILE = 'parameter_file'
KIND_GROUP_KINDS = (KIND_CONNECTORS_GROUP, KIND_CONNECTIONS_GROUP, KIND_PARAMETER_FILES_GROUP)
KIND_INVISIBLE_ROOT = 'invisible_root'


class TreeNode:
  '''Lightweight display node. Not part of the OMSimulatorPython object model --
  purely bookkeeping so QAbstractItemModel can answer index()/parent() in O(1).
  '''

  __slots__ = ('kind', 'label', 'obj', 'parent', 'children')

  def __init__(self, kind: str, label: str, obj, parent: 'TreeNode | None' = None):
    self.kind = kind
    self.label = label
    self.obj = obj
    self.parent = parent
    self.children: list[TreeNode] = []

  def addChild(self, child: 'TreeNode') -> 'TreeNode':
    child.parent = self
    self.children.append(child)
    return child

  def row(self) -> int:
    if self.parent is None:
      return 0
    return self.parent.children.index(self)


def _connectorLabel(connector: Connector) -> str:
  signalType = connector.getSignalType()
  typeName = signalType.name if signalType is not None else '?'
  return f'{connector.name} : {connector.getCausality().name} {typeName}'


def _connectionLabel(connection: Connection) -> str:
  return f'{connection.startElement}.{connection.startConnector} -> {connection.endElement}.{connection.endConnector}'


def _elementLabel(name: str, element) -> str:
  if isinstance(element, System):
    return f'{name}  [system]'
  if isinstance(element, Component):
    return f'{name}  [{element.fmuPath}]'
  if isinstance(element, ComponentTable):
    return f'{name}  [table]'
  return name


def _parameterFileLabel(ssv: str, ssm: str | None) -> str:
  return f'{Path(ssv).name} (+ {Path(ssm).name})' if ssm else Path(ssv).name


def _addParameterFileNodes(node: TreeNode, element) -> None:
  '''Appends a "Parameter Files" group listing whatever SSV (optionally +SSM)
  references are attached to `element` (a System or Component) -- both hold
  the same self.parameterResources: list[{ssvResource: ssmResourceOrNone}]
  shape, see System/Component.addSSVReference.

  removeSSVReference only deletes the dict *key*, leaving an empty {} behind
  in the list (System/Component._remove) -- so parameterResources itself can
  stay non-empty (e.g. [{}]) after the last file was removed. Flatten first
  and check that instead of the raw list, or a removed entry would keep
  showing an empty "Parameter Files" group.'''
  entries = [(ssv, ssm) for entry in (getattr(element, 'parameterResources', None) or []) for ssv, ssm in entry.items()]
  if not entries:
    return
  group = node.addChild(TreeNode(KIND_PARAMETER_FILES_GROUP, 'Parameter Files', None))
  for ssv, ssm in entries:
    group.addChild(TreeNode(KIND_PARAMETER_FILE, _parameterFileLabel(ssv, ssm), (ssv, ssm)))


def _buildSystemNode(system: System, label: str, parent: TreeNode | None = None) -> TreeNode:
  node = TreeNode(KIND_SYSTEM, label, system, parent)

  for name, element in system.elements.items():
    if isinstance(element, System):
      node.addChild(_buildSystemNode(element, _elementLabel(name, element)))
    elif isinstance(element, Component):
      componentNode = node.addChild(TreeNode(KIND_COMPONENT, _elementLabel(name, element), element))
      _addParameterFileNodes(componentNode, element)
    elif isinstance(element, ComponentTable):
      tableNode = node.addChild(TreeNode(KIND_COMPONENT_TABLE, _elementLabel(name, element), element))
      _addParameterFileNodes(tableNode, element)

  if system.connectors:
    group = node.addChild(TreeNode(KIND_CONNECTORS_GROUP, 'Connectors', None))
    for connector in system.connectors:
      group.addChild(TreeNode(KIND_CONNECTOR, _connectorLabel(connector), connector))

  if system.connections:
    group = node.addChild(TreeNode(KIND_CONNECTIONS_GROUP, 'Connections', None))
    for connection in system.connections:
      group.addChild(TreeNode(KIND_CONNECTION, _connectionLabel(connection), connection))

  _addParameterFileNodes(node, system)

  return node


class SystemTreeModel(QAbstractItemModel):
  '''Tree model over any number of open SSD variants. Each open model gets
  its own top-level wrapper row (labeled with that model's own name), with
  its root System nested one level below it as its own visible row --
  matching OMEdit's "model name > root system" shape, just repeated once per
  open model instead of assuming there is only ever one. This is purely a
  display convention: a model wrapper row carries no operations of its own
  (right-clicking it shows no menu) and every cref built for the API is
  anchored at the *System*'s name, never the model name -- see
  MainWindow._crefPath's KIND_SYSTEM-only ancestor walk.

  A root System itself is always a real row (not hidden) -- otherwise a
  brand-new, still-empty model would show nothing to right-click to start
  adding systems/components/connectors to it.'''

  def __init__(self, parent=None):
    super().__init__(parent)
    self._invisibleRoot = TreeNode(KIND_INVISIBLE_ROOT, '', None)
    # Ordered (rootSystem, modelName) pairs -- the source of truth setModels()
    # rebuilds from; refresh() just replays the same list.
    self._models: list[tuple[System, str]] = []

  def setModels(self, models: list[tuple[System, str]]) -> None:
    '''Rebuild the whole tree from an ordered list of (rootSystem, modelName)
    pairs -- one top-level row per open model.'''
    self.beginResetModel()
    self._invisibleRoot = TreeNode(KIND_INVISIBLE_ROOT, '', None)
    self._models = list(models)
    for system, modelName in self._models:
      modelNode = TreeNode(KIND_MODEL, modelName, system)
      self._invisibleRoot.addChild(modelNode)
      modelNode.addChild(_buildSystemNode(system, str(system.name)))
    self.endResetModel()

  def refresh(self) -> None:
    '''Rebuild the whole tree from the same (rootSystem, modelName) pairs
    last given to setModels() -- call after any structural edit anywhere,
    in any open model.'''
    self.setModels(self._models)

  def isTopLevelSystem(self, node: TreeNode) -> bool:
    '''True if `node` is some open model's own root System (as opposed to a
    nested one) -- used by the tree view to disable "Delete" on it.'''
    return node.parent is not None and node.parent.kind == KIND_MODEL

  def nodeFromIndex(self, index: QModelIndex) -> TreeNode | None:
    if not index.isValid():
      return self._invisibleRoot
    return index.internalPointer()

  # --- QAbstractItemModel overrides -----------------------------------------

  def index(self, row: int, column: int, parent: QModelIndex = QModelIndex()) -> QModelIndex:
    if not self.hasIndex(row, column, parent):
      return QModelIndex()

    parentNode = self.nodeFromIndex(parent)
    if parentNode is None or row >= len(parentNode.children):
      return QModelIndex()

    return self.createIndex(row, column, parentNode.children[row])

  def parent(self, index: QModelIndex) -> QModelIndex:
    if not index.isValid():
      return QModelIndex()

    node: TreeNode = index.internalPointer()
    parentNode = node.parent
    if parentNode is None or parentNode is self._invisibleRoot:
      return QModelIndex()

    return self.createIndex(parentNode.row(), 0, parentNode)

  def rowCount(self, parent: QModelIndex = QModelIndex()) -> int:
    if parent.column() > 0:
      return 0
    parentNode = self.nodeFromIndex(parent)
    if parentNode is None:
      return 0
    return len(parentNode.children)

  def columnCount(self, parent: QModelIndex = QModelIndex()) -> int:
    return 1

  def data(self, index: QModelIndex, role: int = Qt.ItemDataRole.DisplayRole):
    if not index.isValid():
      return None
    node: TreeNode = index.internalPointer()
    if role == Qt.ItemDataRole.DisplayRole:
      return node.label
    return None

  def headerData(self, section: int, orientation: Qt.Orientation, role: int = Qt.ItemDataRole.DisplayRole):
    if orientation == Qt.Orientation.Horizontal and role == Qt.ItemDataRole.DisplayRole and section == 0:
      return 'Model'
    return None
