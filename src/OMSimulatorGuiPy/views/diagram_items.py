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

'''QGraphicsItem subclasses for the diagram canvas.

Coordinate convention: SSD/ElementGeometry is Y-up (Modelica convention);
Qt's QGraphicsView is Y-down. `geometryToSceneRect` is the one place that
flips Y -- everything downstream works in already-flipped Qt scene
coordinates, so item-local painting (including text) never needs its own
flip.

ElementGeometry.x1/y1/x2/y2 units are whatever the authoring tool chose
(dcmotor.ssp uses low hundreds) -- QGraphicsView handles arbitrary float
ranges fine via fitInView, so no rescaling is needed here.
'''

import math
from pathlib import Path

from PySide6.QtCore import QPointF, QRectF, Qt
from PySide6.QtGui import QBrush, QColor, QFont, QPainterPath, QPainterPathStroker, QPen, QPolygonF
from PySide6.QtWidgets import QGraphicsEllipseItem, QGraphicsItem, QGraphicsPathItem, QGraphicsPolygonItem, QGraphicsRectItem, QGraphicsSimpleTextItem

from OMSimulator import Component
from OMSimulator.connection import ConnectionGeometry
from OMSimulator.variable import Causality

PORT_SIZE = 3.0
# Top-level system connectors (SystemBoundaryItem's own ports) are drawn
# bigger than an FMU component's own ports -- there are usually only a
# handful of them and they're the ones most often dragged from repeatedly
# (fan-out to several internal inputs), so a larger, easier-to-grab target
# is worth the extra visual weight; component ports stay at PORT_SIZE.
BOUNDARY_PORT_SIZE = 6.0


def _arrowPolygon(size: float) -> QPolygonF:
  '''A small triangle pointing right (the Modelica.Blocks convention: input
  ports on the left edge point into the icon, output ports on the right edge
  point further out -- both rightward, distinguished by color).'''
  return QPolygonF([QPointF(-size, -size), QPointF(size, 0.0), QPointF(-size, size)])

CAUSALITY_COLORS = {
  Causality.input: QColor(60, 120, 220),
  Causality.output: QColor(40, 160, 90),
}


def geometryToSceneRect(geometry) -> QRectF:
  '''Maps an ElementGeometry/SystemGeometry-shaped object (x1,y1,x2,y2, Y-up)
  to a Qt scene rect (Y-down).'''
  top = -max(geometry.y1, geometry.y2)
  bottom = -min(geometry.y1, geometry.y2)
  left = min(geometry.x1, geometry.x2)
  right = max(geometry.x1, geometry.x2)
  return QRectF(QPointF(left, top), QPointF(right, bottom))


class PortItem(QGraphicsPolygonItem):
  '''A connector port, drawn as a small right-pointing arrow (matching the
  Modelica.Blocks connector icon convention) and positioned at
  connector.connectorGeometry.(x,y) -- relative [0,1] within `localRect`
  (the parent icon/boundary's own (0,0)..(w,h) frame). y=1 is icon-top
  (Y-up convention), consistent with geometryToSceneRect's flip.
  connector.connectorGeometry is always real by construction time
  (DiagramScene spreads out fallback positions for connectors that don't
  have one, so same-causality ports never collide).

  Draggable with Shift held (plain drag from a port means "start a
  connection" -- see DiagramView.mousePressEvent); dropping commits the new
  position back into connectorGeometry, clamped to [0,1].

  `onMoved`, if given, is called (no args) once such a drag completes, so
  the owning scene can rebuild -- connections aren't live-tracked mid-drag.
  '''

  def __init__(self, connector, localRect: QRectF, parent: QGraphicsItem, onMoved=None, size: float = PORT_SIZE):
    super().__init__(_arrowPolygon(size), parent)
    self.connector = connector
    self._localRect = localRect
    self._onMoved = onMoved

    geometry = connector.connectorGeometry
    self.setPos(localRect.left() + geometry.x * localRect.width(),
                localRect.top() + (1.0 - geometry.y) * localRect.height())
    self._dragStartScenePos = self.pos()

    color = CAUSALITY_COLORS.get(connector.getCausality(), QColor(130, 130, 130))
    self.setBrush(QBrush(color))
    self.setPen(QPen(color.darker(150)))
    self.setFlag(QGraphicsItem.GraphicsItemFlag.ItemIsMovable, True)
    self.setZValue(2)
    self.setToolTip(f'{connector.name} ({connector.getCausality().name}, {connector.getSignalType().name})')

  def mouseReleaseEvent(self, event) -> None:
    super().mouseReleaseEvent(event)
    self.commitPositionIfMoved()

  def commitPositionIfMoved(self) -> None:
    '''Writes self.pos() back into connectorGeometry.(x,y) (clamped to
    [0,1] of the parent's local rect) and notifies onMoved. Split out from
    mouseReleaseEvent so it can be exercised without a real Qt mouse-event
    sequence.'''
    newPos = self.pos()
    if newPos == self._dragStartScenePos:
      return
    self._dragStartScenePos = newPos

    width = self._localRect.width() or 1.0
    height = self._localRect.height() or 1.0
    fx = (newPos.x() - self._localRect.left()) / width
    fy = 1.0 - (newPos.y() - self._localRect.top()) / height

    geometry = self.connector.connectorGeometry
    geometry.x = max(0.0, min(1.0, fx))
    geometry.y = max(0.0, min(1.0, fy))

    if self._onMoved is not None:
      self._onMoved()


_WIREABLE_CAUSALITIES = (Causality.input, Causality.output)


def _createPorts(hostItem: QGraphicsItem, connectors, localRect: QRectF, onMoved=None, size: float = PORT_SIZE) -> dict:
  '''Only input/output connectors are drawn as ports -- parameters (and
  calculatedParameter/local/independent) aren't valid connection endpoints
  (Connection.is_validConnection never accepts them), they're set via the
  properties dialog (M5) instead.'''
  wireable = [c for c in connectors if c.getCausality() in _WIREABLE_CAUSALITIES]
  return {str(connector.name): PortItem(connector, localRect, hostItem, onMoved, size=size) for connector in wireable}


def _portScenePos(hostItem: QGraphicsItem, ports: dict, connectorName: str) -> QPointF | None:
  port = ports.get(connectorName)
  return None if port is None else hostItem.mapToScene(port.pos())


def _parameterFileNames(resources) -> list[str]:
  '''Flattens a parameterResources list ([{ssvResource: ssmResourceOrNone}, ...],
  see System/Component.addSSVReference) into display names, e.g.
  "parameters2.ssv" or "parameters2.ssv (+ mapping1.ssm)".

  removeSSVReference only deletes the dict *key*, leaving an empty {} behind
  in the list (System/Component._remove) -- so an entry with nothing left in
  it is simply skipped here rather than showing up as an empty name.'''
  names = []
  for entry in resources or []:
    for ssv, ssm in entry.items():
      names.append(f'{Path(ssv).name} (+ {Path(ssm).name})' if ssm else Path(ssv).name)
  return names


class ParameterFileBadgeItem(QGraphicsSimpleTextItem):
  '''The "P" badge itself, as its own class purely so DiagramView's
  mouseDoubleClickEvent can recognize "the user double-clicked the badge,
  not the box behind it" via isinstance() rather than a fragile text=='P'
  check.'''


def _addParameterFileBadge(hostItem: QGraphicsItem, resources, rectWidth: float) -> ParameterFileBadgeItem | None:
  '''A small "P" badge in the box's top-right corner when one or more SSV/SSM
  files are attached (System.parameterResources / Component.parameterResources)
  -- mirrors how the FMU-type label communicates FMU kind, just for parameter
  files instead. Double-clicking it opens the values editor (see
  DiagramView.mouseDoubleClickEvent); the tree is still where files are
  actually added/removed.'''
  names = _parameterFileNames(resources)
  if not names:
    return None
  badge = ParameterFileBadgeItem('P', hostItem)
  badgeFont = QFont()
  badgeFont.setPointSizeF(6.0)
  badgeFont.setBold(True)
  badge.setFont(badgeFont)
  badge.setBrush(QBrush(QColor(140, 40, 140)))
  badge.setPos(max(0.0, rectWidth - 8.0), 1)
  badge.setToolTip('Parameter files (double-click to edit values):\n' + '\n'.join(names))
  return badge


_RESIZE_MARGIN = 4.0
_MIN_ICON_SIZE = 15.0

_EDGE_CURSORS = {
  ('left',): Qt.CursorShape.SizeHorCursor,
  ('right',): Qt.CursorShape.SizeHorCursor,
  ('top',): Qt.CursorShape.SizeVerCursor,
  ('bottom',): Qt.CursorShape.SizeVerCursor,
  ('left', 'top'): Qt.CursorShape.SizeFDiagCursor,
  ('right', 'bottom'): Qt.CursorShape.SizeFDiagCursor,
  ('right', 'top'): Qt.CursorShape.SizeBDiagCursor,
  ('left', 'bottom'): Qt.CursorShape.SizeBDiagCursor,
}


class ElementIconItem(QGraphicsRectItem):
  '''One child element (System/Component/ComponentTable) of the currently
  displayed system, drawn as a labelled box with its connectors as ports on
  its border. `element.elementgeometry` is always a real ElementGeometry by
  the time this is constructed (DiagramScene assigns one to fallback-
  positioned elements before building items).

  Draggable from its middle (moves it) or from within _RESIZE_MARGIN of an
  edge/corner (resizes it, opposite edge(s) staying fixed) -- both commit
  straight back into elementgeometry, translated back to SSD's Y-up
  convention, so the model stays the single source of truth. Resizing also
  repositions the ports, whose fractional [0,1] placement is relative to
  this icon's current size.

  `onMoved`, if given, is called (no args) once a move or resize completes,
  so the owning scene can rebuild -- connections aren't live-tracked during
  the drag itself, only once it settles.
  '''

  def __init__(self, name: str, element, sceneRect: QRectF, onMoved=None, parent=None):
    super().__init__(0, 0, sceneRect.width(), sceneRect.height(), parent)
    self.setPos(sceneRect.topLeft())
    self.name = name
    self.element = element
    self._onMoved = onMoved
    self._dragStartScenePos = sceneRect.topLeft()
    self._resizeEdges: tuple[str, ...] | None = None
    self._resizeStartRect: QRectF | None = None
    self._resizeStartScenePos: QPointF | None = None
    self._resizeStartMouseScenePos: QPointF | None = None

    self.setBrush(QBrush(QColor(235, 238, 245)))
    self.setPen(QPen(QColor(90, 90, 90)))
    # Deliberately not ItemIsSelectable: selection isn't used for anything in
    # this app, and Qt's default behavior for movable+selectable items is to
    # drag every currently-selected item together -- a stray rubber-band drag
    # selecting two icons would then move both whenever either was dragged.
    self.setFlag(QGraphicsItem.GraphicsItemFlag.ItemIsMovable, True)
    self.setAcceptHoverEvents(True)
    self.setZValue(1)
    self.setToolTip(name)

    # FMU-backed components show their FMU kind ('me'/'cs'/'me_cs', from the
    # modelDescription.xml's <ModelExchange>/<CoSimulation> elements) next
    # to the name -- element.fmu is None if the resource failed to load, in
    # which case there's nothing to report.
    labelText = name
    if isinstance(element, Component) and element.fmu is not None:
      labelText = f'{name}\n({element.fmu.fmuType})'

    self._label = QGraphicsSimpleTextItem(labelText, self)
    labelFont = QFont()
    labelFont.setPointSizeF(6.5)
    self._label.setFont(labelFont)
    self._label.setPos(2, 1)

    _addParameterFileBadge(self, getattr(element, 'parameterResources', None), sceneRect.width())

    self.ports = _createPorts(self, getattr(element, 'connectors', []), self.rect(), onMoved)

  def portScenePos(self, connectorName: str) -> QPointF | None:
    return _portScenePos(self, self.ports, connectorName)

  # --- move ------------------------------------------------------------------

  def mouseReleaseEvent(self, event) -> None:
    if self._resizeEdges is not None:
      self._commitResize()
      self._resizeEdges = None
      event.accept()
      return
    super().mouseReleaseEvent(event)
    self.commitPositionIfMoved()

  def commitPositionIfMoved(self) -> None:
    '''Writes self.pos()'s delta from the last commit into elementgeometry
    (translated back to SSD's Y-up convention) and notifies onMoved with
    (this element's name, the scene-space delta) so any attached connection
    with explicit waypoints can be shifted by the same amount (see
    DiagramScene._onElementMoved) -- without that, a connection's anchor
    would snap to the element's new port position while its interior
    waypoints stayed exactly where they were, stretching the path into a
    shape unrelated to where the element actually moved. Split out from
    mouseReleaseEvent so it can be exercised without a real Qt mouse-event
    sequence.'''
    newScenePos = self.pos()
    delta = newScenePos - self._dragStartScenePos
    self._dragStartScenePos = newScenePos
    if delta.isNull():
      return

    geometry = self.element.elementgeometry
    if geometry is None:
      return

    # Scene X maps directly to SSD x; scene Y is negated SSD y (see module
    # docstring), so a downward screen move (positive dy) is a decrease in y.
    geometry.x1 += delta.x()
    geometry.x2 += delta.x()
    geometry.y1 -= delta.y()
    geometry.y2 -= delta.y()

    if self._onMoved is not None:
      self._onMoved(self.name, delta)

  # --- resize ------------------------------------------------------------------

  def _edgesNear(self, pos: QPointF) -> tuple[str, ...]:
    rect = self.rect()
    edges = []
    if abs(pos.x() - rect.left()) <= _RESIZE_MARGIN:
      edges.append('left')
    elif abs(pos.x() - rect.right()) <= _RESIZE_MARGIN:
      edges.append('right')
    if abs(pos.y() - rect.top()) <= _RESIZE_MARGIN:
      edges.append('top')
    elif abs(pos.y() - rect.bottom()) <= _RESIZE_MARGIN:
      edges.append('bottom')
    return tuple(edges)

  def hoverMoveEvent(self, event) -> None:
    edges = self._edgesNear(event.pos())
    self.setCursor(_EDGE_CURSORS.get(edges, Qt.CursorShape.SizeAllCursor))
    super().hoverMoveEvent(event)

  def hoverLeaveEvent(self, event) -> None:
    self.unsetCursor()
    super().hoverLeaveEvent(event)

  def mousePressEvent(self, event) -> None:
    edges = self._edgesNear(event.pos())
    if edges:
      self._resizeEdges = edges
      self._resizeStartRect = QRectF(self.rect())
      self._resizeStartScenePos = self.pos()
      self._resizeStartMouseScenePos = event.scenePos()
      event.accept()
      return
    self._resizeEdges = None
    super().mousePressEvent(event)

  def mouseMoveEvent(self, event) -> None:
    if self._resizeEdges is None:
      super().mouseMoveEvent(event)
      return

    delta = event.scenePos() - self._resizeStartMouseScenePos
    rect = QRectF(self._resizeStartRect)
    pos = QPointF(self._resizeStartScenePos)

    if 'right' in self._resizeEdges:
      rect.setWidth(max(_MIN_ICON_SIZE, self._resizeStartRect.width() + delta.x()))
    elif 'left' in self._resizeEdges:
      newWidth = max(_MIN_ICON_SIZE, self._resizeStartRect.width() - delta.x())
      pos.setX(self._resizeStartScenePos.x() + (self._resizeStartRect.width() - newWidth))
      rect.setWidth(newWidth)

    if 'bottom' in self._resizeEdges:
      rect.setHeight(max(_MIN_ICON_SIZE, self._resizeStartRect.height() + delta.y()))
    elif 'top' in self._resizeEdges:
      newHeight = max(_MIN_ICON_SIZE, self._resizeStartRect.height() - delta.y())
      pos.setY(self._resizeStartScenePos.y() + (self._resizeStartRect.height() - newHeight))
      rect.setHeight(newHeight)

    self.prepareGeometryChange()
    self.setRect(0, 0, rect.width(), rect.height())
    self.setPos(pos)
    self._repositionPorts()
    event.accept()

  def _repositionPorts(self) -> None:
    '''Re-derives each port's local position from its connector's fractional
    (x,y) against this icon's *current* rect -- called live during a resize
    so ports stay on the border instead of drifting into the icon.'''
    localRect = self.rect()
    for port in self.ports.values():
      geometry = port.connector.connectorGeometry
      port._localRect = localRect
      port.setPos(localRect.left() + geometry.x * localRect.width(),
                  localRect.top() + (1.0 - geometry.y) * localRect.height())

  def _commitResize(self) -> None:
    '''Writes the settled rect+position into elementgeometry (translated
    back to SSD's Y-up convention) and notifies onMoved.'''
    geometry = self.element.elementgeometry
    if geometry is not None:
      scenePos = self.pos()
      rect = self.rect()
      geometry.x1 = scenePos.x()
      geometry.x2 = scenePos.x() + rect.width()
      geometry.y1 = -(scenePos.y() + rect.height())
      geometry.y2 = -scenePos.y()

    self._dragStartScenePos = self.pos()
    for port in self.ports.values():
      port._dragStartScenePos = port.pos()

    if self._onMoved is not None:
      self._onMoved()


class SystemBoundaryItem(QGraphicsRectItem):
  '''The currently displayed system's own boundary -- a dashed frame hosting
  ports for the system's own (top-level) connectors, i.e. the ports that
  connect this system to its parent.

  Doesn't show a "P" parameter-file badge itself: a system's own
  parameterResources are shown as dedicated ParameterFileIconItem boxes
  instead (see that class) -- this item only exists at all when the system
  has top-level connectors, so a badge hosted here would stay invisible
  whenever a system has parameter files but no connectors (a real, silent
  gap this replaced).'''

  def __init__(self, system, sceneRect: QRectF, onMoved=None, parent=None):
    super().__init__(0, 0, sceneRect.width(), sceneRect.height(), parent)
    self.setPos(sceneRect.topLeft())

    pen = QPen(QColor(160, 160, 160))
    pen.setStyle(Qt.PenStyle.DashLine)
    self.setPen(pen)
    self.setBrush(QBrush(Qt.BrushStyle.NoBrush))
    self.setZValue(-1)

    self.system = system

    localRect = QRectF(0, 0, sceneRect.width(), sceneRect.height())
    self.ports = _createPorts(self, system.connectors, localRect, onMoved, size=BOUNDARY_PORT_SIZE)

  def portScenePos(self, connectorName: str) -> QPointF | None:
    return _portScenePos(self, self.ports, connectorName)


class ParameterFileIconItem(QGraphicsRectItem):
  '''A small, movable box representing one SSV (+ optional SSM) reference
  attached directly to the *currently displayed* system itself -- as
  opposed to a child component/subsystem's own attached file, which is
  shown via a "P" badge on that child's existing ElementIconItem instead
  (see _addParameterFileBadge). A system-level file has no such natural
  host to badge, and (per SystemBoundaryItem's own docstring) the boundary
  itself doesn't always even exist, so it gets its own box here.

  There's no backing geometry field for this in the SSP/SSD model
  (System.parameterResources is just [{ssvResource: ssmResourceOrNone}, ...],
  nothing about position) -- so, like DiagramScene's own fallback element
  grid, its position is computed once and then remembered only for the rest
  of this session (DiagramScene._parameterFileGeometry, keyed by ssvResource
  path), never written to the .ssp file. Dragging it calls `onCommit` with
  its new top-left scene position so the *next* unrelated rebuild
  (DiagramScene.setSystem is called after every edit) doesn't snap it back
  to a freshly-computed fallback slot.'''

  def __init__(self, ssvResource: str, ssmResource: str | None, sceneRect: QRectF, onCommit=None, parent=None):
    super().__init__(0, 0, sceneRect.width(), sceneRect.height(), parent)
    self.setPos(sceneRect.topLeft())
    self.ssvResource = ssvResource
    self.ssmResource = ssmResource
    self._onCommit = onCommit
    self._dragStartScenePos = sceneRect.topLeft()

    self.setBrush(QBrush(QColor(245, 230, 250)))
    self.setPen(QPen(QColor(140, 40, 140)))
    self.setFlag(QGraphicsItem.GraphicsItemFlag.ItemIsMovable, True)
    self.setZValue(1)

    # The box itself is already the "which file" indicator (its position is
    # the whole point of it existing at all) -- the filename lives in the
    # tooltip, same as the "P" badge on a component's own box; showing it as
    # the on-canvas label too just repeats information already visible in
    # the tree and made the box unreadably cramped for anything but a short
    # name.
    label = _parameterFileNames([{ssvResource: ssmResource}])[0]
    self.setToolTip(f'Parameter file: {label}\nDouble-click to edit values.')

    self._label = QGraphicsSimpleTextItem('P', self)
    labelFont = QFont()
    labelFont.setPointSizeF(10.0)
    labelFont.setBold(True)
    self._label.setFont(labelFont)
    self._label.setBrush(QBrush(QColor(40, 40, 40)))
    labelRect = self._label.boundingRect()
    self._label.setPos((sceneRect.width() - labelRect.width()) / 2.0,
                        (sceneRect.height() - labelRect.height()) / 2.0)

  def mouseReleaseEvent(self, event) -> None:
    super().mouseReleaseEvent(event)
    self.commitPositionIfMoved()

  def commitPositionIfMoved(self) -> None:
    newScenePos = self.pos()
    if newScenePos == self._dragStartScenePos:
      return
    self._dragStartScenePos = newScenePos
    if self._onCommit is not None:
      self._onCommit(self.ssvResource, newScenePos)


_CONNECTION_HIT_MARGIN = 6.0  # widened click/hover tolerance around the (thin) drawn line
_WAYPOINT_RADIUS = 0.5  # drawn circle around each interior waypoint, also the grab target
_MOVE_THRESHOLD = 0.5  # below this, a press+release is a no-op click, not a drag


def _pointSegmentDistance(p: QPointF, a: QPointF, b: QPointF) -> float:
  '''Shortest distance from p to the segment a-b (not the infinite line).'''
  ab = b - a
  lengthSquared = ab.x() ** 2 + ab.y() ** 2
  if lengthSquared == 0:
    return math.hypot(p.x() - a.x(), p.y() - a.y())
  t = max(0.0, min(1.0, QPointF.dotProduct(p - a, ab) / lengthSquared))
  projection = a + ab * t
  return math.hypot(p.x() - projection.x(), p.y() - projection.y())


_ROUTE_STUB = 15.0          # short leg leaving/entering a port, in its own arrow direction
_ROUTE_DETOUR_MARGIN = 20.0  # clearance above both boxes for the "backward" detour


def defaultRoute(start: QPointF, end: QPointF) -> list[QPointF]:
  '''The route a connection gets when it has no saved connectionGeometry yet
  -- an immediate orthogonal elbow, not a raw diagonal, matching OMEdit's own
  default routing. Also used for the live drag-preview while connecting two
  ports (DiagramView.mouseMoveEvent recomputes it on every move), so this is
  what makes the preview reroute itself as the cursor crosses back and forth
  relative to the source -- not a fixed shape, a function of wherever the
  target currently is.

  Ports are always drawn as small rightward-pointing arrows in this app
  regardless of which edge they actually sit on (see this module's
  docstring). When the target is far enough to the right, a route that
  leaves the start heading right and arrives at the end heading right too
  (elbowing at the horizontal midpoint) reads correctly for every port pair,
  and ports already level with each other collapse this to a plain straight
  line. But when the target sits behind the source (level with or to the
  left of it -- e.g. connecting a later element's output back to an earlier
  element's input), that same "exit right, enter right" shape would have to
  double back through whichever box's own edge it just left. In that case,
  route around instead: a short stub out of the source (matching its arrow),
  a detour above both boxes, then a short stub into the target from its own
  left (matching its arrow too, rather than backing into it from the right).'''
  if end.x() >= start.x() + 2 * _ROUTE_STUB:
    if abs(start.y() - end.y()) < 1e-6:
      return [start, end]
    midX = (start.x() + end.x()) / 2.0
    return [start, QPointF(midX, start.y()), QPointF(midX, end.y()), end]

  detourY = min(start.y(), end.y()) - _ROUTE_DETOUR_MARGIN
  exitX = start.x() + _ROUTE_STUB
  enterX = end.x() - _ROUTE_STUB
  return [start, QPointF(exitX, start.y()), QPointF(exitX, detourY),
          QPointF(enterX, detourY), QPointF(enterX, end.y()), end]


class ConnectionItem(QGraphicsPathItem):
  '''A connection between two resolved port scene positions. Honors
  connectionGeometry.pointsX/pointsY waypoints when present (same Y-up
  convention as ElementGeometry), otherwise starts from defaultRoute's
  automatic orthogonal elbow rather than a raw diagonal.

  Also lets the user reshape it interactively, since the default route (or a
  saved one) can still cross over unrelated icons: dragging any point along
  the line inserts a new waypoint there (or moves an existing one, if the
  press lands close enough to it) and drags it live; right-clicking a
  waypoint removes it (DiagramView.contextMenuEvent handles that, via
  waypointIndexAt/removeWaypoint). Both commit straight back into
  connection.connectionGeometry.pointsX/pointsY. Purely-automatic routing is
  never written to connectionGeometry on its own -- only once the user
  actually drags something does the (then-current) shape freeze into real,
  persisted waypoints; until touched, a reload recomputes the same default
  elbow rather than the file carrying a redundant, never-asked-for geometry.

  `onMoved`, if given, is called (no args) once a reshape actually changes
  something, so the owning scene can rebuild -- matching ElementIconItem's
  own onMoved contract. A click that doesn't move the mouse (inserting, then
  immediately releasing without dragging) is treated as a no-op and doesn't
  call it, so an idle click can't spam stray waypoints.'''

  def __init__(self, connection, startPos: QPointF, endPos: QPointF, onMoved=None, parent=None):
    super().__init__(parent)
    self.connection = connection
    self._onMoved = onMoved
    self._dragIndex: int | None = None
    self._dragStartPos: QPointF | None = None
    self._dragWasInsert = False

    geometry = connection.connectionGeometry
    if geometry is not None and geometry.pointsX and len(geometry.pointsX) == len(geometry.pointsY):
      self._points: list[QPointF] = [startPos]
      self._points += [QPointF(px, -py) for px, py in zip(geometry.pointsX, geometry.pointsY)]
      self._points.append(endPos)
    else:
      self._points = defaultRoute(startPos, endPos)

    self.setPen(QPen(QColor(60, 60, 60), 0.5))
    self.setZValue(0)
    self.setAcceptHoverEvents(True)
    self.setToolTip(f'{connection.startElement}.{connection.startConnector} -> '
                     f'{connection.endElement}.{connection.endConnector}')
    self._rebuildPath()

  def _rebuildPath(self) -> None:
    path = QPainterPath(self._points[0])
    for point in self._points[1:]:
      path.lineTo(point)
    self.setPath(path)

  def shape(self) -> QPainterPath:
    # The default shape() for a thinly-stroked path is a razor-thin hit
    # target; widen it so both grabbing a point to drag and right-clicking
    # to delete/remove-waypoint are actually clickable.
    stroker = QPainterPathStroker()
    stroker.setWidth(_CONNECTION_HIT_MARGIN)
    return stroker.createStroke(self.path())

  def paint(self, painter, option, widget=None) -> None:
    super().paint(painter, option, widget)
    if len(self._points) <= 2:
      return
    painter.setBrush(QBrush(QColor(60, 60, 60)))
    painter.setPen(Qt.PenStyle.NoPen)
    for point in self._points[1:-1]:
      painter.drawEllipse(point, _WAYPOINT_RADIUS, _WAYPOINT_RADIUS)

  # --- reshaping ---------------------------------------------------------------

  def _nearestInteriorIndex(self, pos: QPointF) -> int | None:
    best = None
    bestDist = _CONNECTION_HIT_MARGIN
    for index in range(1, len(self._points) - 1):
      dist = math.hypot(self._points[index].x() - pos.x(), self._points[index].y() - pos.y())
      if dist <= bestDist:
        bestDist = dist
        best = index
    return best

  def waypointIndexAt(self, scenePos: QPointF) -> int | None:
    return self._nearestInteriorIndex(self.mapFromScene(scenePos))

  def removeWaypoint(self, index: int) -> None:
    del self._points[index]
    self._rebuildPath()
    self._commitGeometry()

  # Driven entirely by DiagramView (beginReshapeAt/updateReshape/endReshapeAt),
  # not by this item's own mousePressEvent/mouseMoveEvent/mouseReleaseEvent --
  # a connection is drawn *behind* element icons (see class docstring), so
  # Qt's normal topmost-item dispatch would never deliver a press here for any
  # stretch of the route that happens to pass underneath one. DiagramView
  # instead scans every ConnectionItem's shape() itself, regardless of
  # z-order/what's on top, so a segment hidden under an icon can still be
  # grabbed and dragged out into free space.

  def isReshaping(self) -> bool:
    return self._dragIndex is not None

  def beginReshapeAt(self, pos: QPointF) -> None:
    index = self._nearestInteriorIndex(pos)
    if index is not None:
      self._dragWasInsert = False
    else:
      index = self._insertPointOnNearestSegment(pos)
      self._dragWasInsert = True
    self._dragIndex = index
    self._dragStartPos = QPointF(self._points[index])

  def _insertPointOnNearestSegment(self, pos: QPointF) -> int:
    bestIndex = 1
    bestDist = None
    for i in range(len(self._points) - 1):
      dist = _pointSegmentDistance(pos, self._points[i], self._points[i + 1])
      if bestDist is None or dist < bestDist:
        bestDist = dist
        bestIndex = i + 1
    self._points.insert(bestIndex, QPointF(pos))
    return bestIndex

  def updateReshape(self, pos: QPointF) -> None:
    self._points[self._dragIndex] = pos
    self._rebuildPath()

  def endReshapeAt(self, pos: QPointF) -> None:
    index = self._dragIndex
    self._points[index] = pos
    moved = (self._points[index] - self._dragStartPos).manhattanLength() > _MOVE_THRESHOLD
    self._dragIndex = None
    self._dragStartPos = None
    if not moved:
      if self._dragWasInsert:
        del self._points[index]
        self._rebuildPath()
      return
    self._commitGeometry()

  def _commitGeometry(self) -> None:
    interior = self._points[1:-1]
    if interior:
      self.connection.connectionGeometry = ConnectionGeometry(
          pointsX=[p.x() for p in interior], pointsY=[-p.y() for p in interior])
    else:
      self.connection.connectionGeometry = None
    if self._onMoved is not None:
      self._onMoved()
