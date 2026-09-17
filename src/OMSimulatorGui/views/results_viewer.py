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

'''ResultsViewer: a plain top-level window over one simulation's result
file -- a checkable tree of every signal (minus 'time', which is always the
plot's x-axis), grouped by dotted name into the same nesting the model
itself has (e.g. "test.Root.Gain.k" becomes test > Root > Gain > k, not one
flat "test.Root.Gain.k" row) -- and a pyqtgraph plot showing whichever leaf
signals are checked. Deliberately minimal for v1: no multi-axis/unit
grouping, no "check all children" propagation, no curve styling beyond
pyqtgraph's own defaults.'''

import pyqtgraph as pg
from PySide6.QtCore import Qt
from PySide6.QtWidgets import QHBoxLayout, QSplitter, QTreeWidget, QTreeWidgetItem, QWidget

from OMSimulatorGui.results.result_reader import readResultFile

# pyqtgraph defaults to a dark theme (black background/white foreground) --
# switch to a white background with dark axes/grid/text to match the rest
# of this app's plain light UI, before any PlotWidget is constructed.
pg.setConfigOption('background', 'w')
pg.setConfigOption('foreground', 'k')

_SIGNAL_ROLE = Qt.ItemDataRole.UserRole  # the full dotted signal name, stored on leaf items only


def _splitSignalName(name: str) -> list[str]:
  '''Splits on '.' at parenthesis depth 0 only -- a plain str.split('.')
  would wrongly treat the operator syntax "der(Foo.bar)" as two hierarchy
  levels ("der(Foo" then "bar)"), since the der() operator's own dot is
  part of the reference it wraps, not a structural separator.'''
  segments = []
  current = []
  depth = 0
  for ch in name:
    if ch == '(':
      depth += 1
      current.append(ch)
    elif ch == ')':
      depth -= 1
      current.append(ch)
    elif ch == '.' and depth == 0:
      segments.append(''.join(current))
      current = []
    else:
      current.append(ch)
  segments.append(''.join(current))
  return segments


class ResultsViewer(QWidget):
  def __init__(self, resultPath: str, parent=None):
    super().__init__(parent)
    self.setWindowFlag(Qt.WindowType.Window)
    self.setWindowTitle(f'Results - {resultPath}')
    self.resize(1000, 600)

    self._signals = readResultFile(resultPath)
    self._curves: dict[str, object] = {}
    # Cycles forward only (never reused on uncheck) so a signal's color
    # stays stable across toggling other signals on/off -- pg.mkPen()'s own
    # default pen now resolves to the 'foreground' config color (black,
    # see above), which made every curve render identically and
    # indistinguishable from the axes/text.
    self._nextColorIndex = 0

    self._tree = QTreeWidget(self)
    self._tree.setHeaderHidden(True)
    self._buildTree()
    self._tree.itemChanged.connect(self._onItemChanged)

    self._plot = pg.PlotWidget(self)
    self._plot.addLegend()
    self._plot.showGrid(x=True, y=True)

    splitter = QSplitter(self)
    splitter.addWidget(self._tree)
    splitter.addWidget(self._plot)
    splitter.setStretchFactor(0, 0)
    splitter.setStretchFactor(1, 1)
    splitter.setSizes([250, 750])

    layout = QHBoxLayout(self)
    layout.addWidget(splitter)

  def _buildTree(self) -> None:
    '''Groups every signal (minus 'time') by its dotted name into nested
    QTreeWidgetItems -- "test.Root.Gain.k" becomes test > Root > Gain > k --
    matching the tree hierarchy the model itself is edited in, rather than
    one flat row per fully-qualified signal name. Only leaf items (the
    signals themselves) are checkable; intermediate group nodes exist
    purely for navigation.'''
    groupNodes: dict[tuple[str, ...], QTreeWidgetItem] = {}

    def groupNode(path: tuple[str, ...]) -> QTreeWidgetItem:
      if path in groupNodes:
        return groupNodes[path]
      parent = self._tree if len(path) == 1 else groupNode(path[:-1])
      node = QTreeWidgetItem(parent, [path[-1]])
      # QTreeWidgetItem is checkable by default (unlike QListWidgetItem) --
      # clear it explicitly rather than relying on "no checkState was ever
      # set so no checkbox happens to show".
      node.setFlags(node.flags() & ~Qt.ItemFlag.ItemIsUserCheckable)
      groupNodes[path] = node
      return node

    for name in sorted(self._signals):
      if name == 'time':
        continue
      segments = tuple(_splitSignalName(name))
      parent = self._tree if len(segments) == 1 else groupNode(segments[:-1])
      leaf = QTreeWidgetItem(parent, [segments[-1]])
      leaf.setFlags(leaf.flags() | Qt.ItemFlag.ItemIsUserCheckable)
      leaf.setCheckState(0, Qt.CheckState.Unchecked)
      leaf.setData(0, _SIGNAL_ROLE, name)

  def _onItemChanged(self, item: QTreeWidgetItem) -> None:
    name = item.data(0, _SIGNAL_ROLE)
    if name is None:
      return  # an intermediate group node, not a signal -- nothing to plot
    if item.checkState(0) == Qt.CheckState.Checked:
      times, values = self._signals[name]
      color = pg.intColor(self._nextColorIndex, hues=12)
      self._nextColorIndex += 1
      self._curves[name] = self._plot.plot(times, values, name=name, pen=pg.mkPen(color=color, width=2))
    else:
      curve = self._curves.pop(name, None)
      if curve is not None:
        self._plot.removeItem(curve)
