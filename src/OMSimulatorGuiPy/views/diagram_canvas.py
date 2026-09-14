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

'''DiagramView/DiagramScene: renders one System level at a time (M2), with
structured editing (M3: add/delete/rename via the tree) and, as of M4,
connection drawing (plain drag from a port to another) and moving elements
(dragging an icon commits its new position into elementgeometry) or ports
(Shift+drag a port commits its new position into connectorGeometry).

Elements/connectors without authored geometry get a real one assigned on
first render (a simple grid layout for elements; connectors of the same
causality spread evenly along their edge instead of all defaulting to the
same spot) so moving/connecting them works the same as authored ones --
this is the one place the view mutates the model outside of an explicit
user edit, and it's purely a position, not a structural change.
'''

import math
from collections import defaultdict

from PySide6.QtCore import QPointF, QRectF, Qt, QTimer, Signal
from PySide6.QtGui import QColor, QPainter, QPainterPath, QPen
from PySide6.QtWidgets import QGraphicsPathItem, QGraphicsScene, QGraphicsView, QMenu

from OMSimulator import System
from OMSimulator.component import Component
from OMSimulator.connector import ConnectorGeometry
from OMSimulator.elementgeometry import ElementGeometry
from OMSimulator.variable import Causality

from OMSimulatorGui.views.diagram_items import (
    ConnectionItem,
    ElementIconItem,
    ParameterFileBadgeItem,
    PortItem,
    SystemBoundaryItem,
    geometryToSceneRect,
)

_FALLBACK_COLS = 4
_FALLBACK_CELL_W = 80.0
_FALLBACK_CELL_H = 60.0
_FALLBACK_ELEMENT_W = 50.0
_FALLBACK_ELEMENT_H = 35.0
_BOUNDARY_MARGIN = 40.0
# A port that already has a connection sits exactly where that connection's
# own (wider, 6-unit) hit-tolerance shape also matches -- this needs to be
# at least that generous so starting a *second* connection from an
# already-wired port doesn't lose to reshaping the existing one. See
# DiagramView._portAt.
_PORT_HIT_RADIUS = 8.0


def _assignFallbackConnectorGeometry(connectors) -> None:
  '''Spreads input/output connectors lacking a connectorGeometry evenly
  along their edge (x=0 for input, x=1 for output) so multiple GUI-added
  connectors of the same causality don't all collide at one default
  position -- without this every such connector defaults to the same
  (edge, 0.5) spot and they render as a single overlapping dot.

  Since connectors get fallback-assigned incrementally (one diagram render
  per add, not all at once), the index used to space a newly-added
  connector out must come from its position within the *full* same-causality
  group -- not just the (usually one-element) subset still lacking geometry
  -- otherwise every new addition independently computes the same "only
  one in the group" midpoint and they still collide.'''
  groups = defaultdict(list)
  for connector in connectors:
    causality = connector.getCausality()
    if causality in (Causality.input, Causality.output):
      groups[0.0 if causality == Causality.input else 1.0].append(connector)

  for x, group in groups.items():
    count = len(group)
    for index, connector in enumerate(group):
      if connector.connectorGeometry is None:
        connector.connectorGeometry = ConnectorGeometry(x=x, y=(index + 1) / (count + 1))


def elementGeometryAt(scenePos: QPointF, width: float = _FALLBACK_ELEMENT_W,
                       height: float = _FALLBACK_ELEMENT_H) -> ElementGeometry:
  '''Builds an ElementGeometry (SSD Y-up) for a new element centered at
  scenePos (Qt Y-down scene coordinates) -- lets a canvas-added system or
  component land under the cursor instead of wherever the fallback grid
  layout would next place it (which always fills from the top-left corner,
  regardless of where on the canvas the user actually right-clicked).'''
  halfW, halfH = width / 2.0, height / 2.0
  return ElementGeometry(
      x1=scenePos.x() - halfW, x2=scenePos.x() + halfW,
      y1=-scenePos.y() - halfH, y2=-scenePos.y() + halfH)


def defaultCanvasCenter() -> QPointF:
  '''Scene-space center of the default (un-expanded) canvas -- used to
  center the root system's own box at the model level (MainWindow's
  _makeModelWrapper), since it otherwise has no elementgeometry of its own
  and would fall into the fallback grid layout's top-left starting slot.'''
  return QPointF(_DEFAULT_CANVAS_WIDTH / 2.0, _DEFAULT_CANVAS_HEIGHT / 2.0)


def connectorGeometryAt(scenePos: QPointF, boundaryRect: QRectF) -> ConnectorGeometry:
  '''Builds a ConnectorGeometry -- fractional [0,1] within the system's own
  boundary, Y-up per PortItem's own convention (y=1 is the boundary's top
  edge) -- from a click's scene position, clamped to the boundary. Same
  "land under the cursor" reasoning as elementGeometryAt.'''
  if boundaryRect.width() <= 0 or boundaryRect.height() <= 0:
    return ConnectorGeometry(x=0.5, y=0.5)
  x = (scenePos.x() - boundaryRect.left()) / boundaryRect.width()
  y = 1.0 - (scenePos.y() - boundaryRect.top()) / boundaryRect.height()
  return ConnectorGeometry(x=min(1.0, max(0.0, x)), y=min(1.0, max(0.0, y)))


_GRID_SPACING = 10.0
_GRID_COLOR = QColor(225, 225, 225)
_CANVAS_BORDER_COLOR = QColor(160, 160, 160)
_CANVAS_OUTSIDE_COLOR = QColor(235, 235, 235)
_DEFAULT_CANVAS_WIDTH = 400.0
_DEFAULT_CANVAS_HEIGHT = 250.0
_CANVAS_MARGIN = 20.0  # keeps content from touching the canvas edge as it expands


class DiagramScene(QGraphicsScene):
  def __init__(self, parent=None):
    super().__init__(parent)
    self._system: System | None = None
    self._elementItems: dict[str, ElementIconItem] = {}
    self._boundaryItem: SystemBoundaryItem | None = None
    self._canvasRect = QRectF(0, 0, _DEFAULT_CANVAS_WIDTH, _DEFAULT_CANVAS_HEIGHT)

  def drawBackground(self, painter, rect) -> None:
    # A bounded, fixed-size "page" like OMEdit's, not an endlessly-tiling
    # grid texture: starts at a sensible default size and only grows once
    # actual content (elements/the system's own boundary) needs more room --
    # see setSystem's canvasRect computation. Purely presentational either
    # way, since SSP itself has no diagram-extent/coordinate-system concept
    # to size a canvas from.
    painter.fillRect(rect, _CANVAS_OUTSIDE_COLOR)
    painter.fillRect(self._canvasRect, QColor(255, 255, 255))

    gridRect = self._canvasRect.intersected(rect)
    if not gridRect.isEmpty():
      pen = QPen(_GRID_COLOR)
      pen.setCosmetic(True)
      painter.setPen(pen)
      left = math.floor(gridRect.left() / _GRID_SPACING) * _GRID_SPACING
      top = math.floor(gridRect.top() / _GRID_SPACING) * _GRID_SPACING
      x = left
      while x < gridRect.right():
        painter.drawLine(QPointF(x, gridRect.top()), QPointF(x, gridRect.bottom()))
        x += _GRID_SPACING
      y = top
      while y < gridRect.bottom():
        painter.drawLine(QPointF(gridRect.left(), y), QPointF(gridRect.right(), y))
        y += _GRID_SPACING

    borderPen = QPen(_CANVAS_BORDER_COLOR)
    borderPen.setCosmetic(True)
    painter.setPen(borderPen)
    painter.setBrush(Qt.BrushStyle.NoBrush)
    painter.drawRect(self._canvasRect)

  def setSystem(self, system: System | None) -> None:
    self.clear()
    self._elementItems.clear()
    self._boundaryItem = None
    self._system = system
    self._canvasRect = QRectF(0, 0, _DEFAULT_CANVAS_WIDTH, _DEFAULT_CANVAS_HEIGHT)

    if system is None:
      self.setSceneRect(self._canvasRect.adjusted(-40, -40, 40, 40))
      return

    rects: list[QRectF] = []
    fallbackIndex = 0
    for name, element in system.elements.items():
      if element.elementgeometry is None:
        row, col = divmod(fallbackIndex, _FALLBACK_COLS)
        fallbackIndex += 1
        x1 = col * _FALLBACK_CELL_W
        y2 = -(row * _FALLBACK_CELL_H)  # Y-up: later rows sit lower, i.e. more negative
        element.elementgeometry = ElementGeometry(
            x1=x1, y1=y2 - _FALLBACK_ELEMENT_H, x2=x1 + _FALLBACK_ELEMENT_W, y2=y2)

      _assignFallbackConnectorGeometry(element.connectors)
      rect = geometryToSceneRect(element.elementgeometry)
      rects.append(rect)
      item = ElementIconItem(str(name), element, rect, onMoved=self._onElementMoved)
      self.addItem(item)
      self._elementItems[str(name)] = item

    if rects:
      union = rects[0]
      for rect in rects[1:]:
        union = union.united(rect)
    else:
      # No elements yet, only (maybe) top-level connectors: anchor the
      # fallback union to the canvas center -- the old plain QRectF(0, 0,
      # 200, 200) here predates the bounded-canvas work and is unrelated to
      # it, so the resulting boundaryRect below could land partly outside
      # the visible canvas (e.g. its left/top edge going negative once
      # -_BOUNDARY_MARGIN is applied), putting a fallback-positioned
      # connector outside the canvas on the very first add.
      center = defaultCanvasCenter()
      union = QRectF(center.x() - _FALLBACK_ELEMENT_W / 2.0, center.y() - _FALLBACK_ELEMENT_H / 2.0,
                      _FALLBACK_ELEMENT_W, _FALLBACK_ELEMENT_H)

    if system.connectors:
      _assignFallbackConnectorGeometry(system.connectors)
      boundaryRect = union.adjusted(-_BOUNDARY_MARGIN, -_BOUNDARY_MARGIN, _BOUNDARY_MARGIN, _BOUNDARY_MARGIN)
      self._boundaryItem = SystemBoundaryItem(system, boundaryRect, onMoved=self._onElementMoved)
      self.addItem(self._boundaryItem)
      union = union.united(boundaryRect)

    for connection in system.connections:
      startPos = self._resolvePortPos(connection.startElement, connection.startConnector)
      endPos = self._resolvePortPos(connection.endElement, connection.endConnector)
      if startPos is not None and endPos is not None:
        self.addItem(ConnectionItem(connection, startPos, endPos, onMoved=self._onElementMoved))

    if rects or self._boundaryItem is not None:
      # Union with the raw (unpadded) content first -- padding it before
      # comparing against the origin-anchored default would push even a
      # single small, comfortably-contained element's rect corner just
      # negative, making the canvas falsely "expand" for content that never
      # actually needed more room. Only pad afterwards, and only along
      # whichever edges the union actually grew past. Includes the boundary
      # (already folded into `union` above) even when there are no elements,
      # so a system's own connectors always stay inside the canvas too.
      grown = self._canvasRect.united(union)
      if grown != self._canvasRect:
        grown.adjust(-_CANVAS_MARGIN, -_CANVAS_MARGIN, _CANVAS_MARGIN, _CANVAS_MARGIN)
      self._canvasRect = grown

    self.setSceneRect(self.itemsBoundingRect().united(self._canvasRect).adjusted(-40, -40, 40, 40))

  def boundaryRectInScene(self) -> QRectF | None:
    '''The current system's own boundary rect in scene coordinates, or None
    if it doesn't have one yet (no top-level connectors) -- used to place a
    canvas-added connector's fractional [0,1] position under the cursor.'''
    if self._boundaryItem is None:
      return None
    rect = self._boundaryItem.rect()
    return QRectF(self._boundaryItem.pos(), rect.size())

  def _resolvePortPos(self, elementName, connectorName):
    elementName = str(elementName)
    connectorName = str(connectorName)
    if elementName == '':
      return None if self._boundaryItem is None else self._boundaryItem.portScenePos(connectorName)
    item = self._elementItems.get(elementName)
    return None if item is None else item.portScenePos(connectorName)

  def _onElementMoved(self, elementName: str | None = None, delta: QPointF | None = None) -> None:
    '''An icon settled after a drag: rebuild so connections (baked in as
    static polylines, not live-tracked mid-drag) snap to the new position.

    Shared as the onMoved callback for ports and connection-reshape commits
    too (both call it with no arguments -- a plain rebuild is all they need,
    since a port's own scene position and a connection's own waypoints are
    already exactly what moved in those cases). Only ElementIconItem passes
    (name, delta): when a whole ELEMENT moves, any connection attached to it
    that has explicit waypoints needs those waypoints shifted by the same
    delta first -- otherwise the connection's start/end anchor would snap to
    the element's new port position via _resolvePortPos while its interior
    waypoints stayed exactly where they were, stretching the path into a
    shape unrelated to where the element actually is now.'''
    if elementName is not None and delta is not None and not delta.isNull():
      deltaModelX = delta.x()
      deltaModelY = -delta.y()  # Y-flip: scene Y-down -> SSD Y-up
      for connection in self._system.connections:
        if str(connection.startElement) == elementName or str(connection.endElement) == elementName:
          geometry = connection.connectionGeometry
          if geometry is not None:
            geometry.pointsX = [x + deltaModelX for x in geometry.pointsX]
            geometry.pointsY = [y + deltaModelY for y in geometry.pointsY]
    self.setSystem(self._system)


def _elementNameForPort(port: PortItem) -> str:
  '''Empty string means the port belongs to the current system's own
  boundary, matching Connection.startElement/endElement's convention.'''
  parent = port.parentItem()
  return parent.name if isinstance(parent, ElementIconItem) else ''


_DRAG_STEER_THRESHOLD = 8.0  # perpendicular deviation (scene units) before a new corner locks in


def _axisPoint(anchor: QPointF, target: QPointF, axis: str) -> QPointF:
  '''target projected onto the line through anchor along the given axis --
  'h' keeps anchor's y and follows target's x, 'v' the other way around.'''
  return QPointF(target.x(), anchor.y()) if axis == 'h' else QPointF(anchor.x(), target.y())


_UNSET = object()  # distinct from any real System *and* from None -- see DiagramView.__init__


class DiagramView(QGraphicsView):
  '''Emits systemDrillDownRequested(System, name) on double-clicking a
  system-type element; MainWindow owns the navigation stack and calls
  setSystem() for both drill-down and "up".

  Dragging from one port to another emits connectionRequested with both
  ports' (elementName, connectorName) plus whatever waypoints the drag
  itself steered through -- MainWindow builds the crefs and calls
  SSP.addConnection (which already validates causality, including the
  flipped-direction case, internally) then applies those waypoints to the
  new connection's connectionGeometry. Right-clicking a connection emits
  connectionDeleteRequested the same way, for SSP.deleteConnection.
  '''

  systemDrillDownRequested = Signal(object, str)
  # elem1, conn1, elem2, conn2, waypoints (list[QPointF] the user actually
  # steered through while dragging -- see _updateConnectDragPreview; empty
  # if they dragged straight across without steering).
  connectionRequested = Signal(str, str, str, str, object)
  connectionDeleteRequested = Signal(str, str, str, str)  # elem1, conn1, elem2, conn2
  # Right-click on empty canvas -- adds to the level shown here, at the
  # click's own scene position (see MainWindow's _addSystemAtPath and co.)
  # rather than wherever the fallback grid layout would otherwise put it.
  addSystemRequested = Signal(QPointF)
  addComponentRequested = Signal(QPointF)
  addConnectorRequested = Signal(QPointF)
  elementPropertiesRequested = Signal(object)  # Component: double-clicked on the canvas
  connectorValueRequested = Signal(str)  # current system's own connector: double-clicked on the canvas
  # Delete key pressed while the cursor is over an element/the current
  # system's own connector -- see keyPressEvent/_deleteUnderCursor. Named
  # by the current level, same as addComponentRequested/addConnectorRequested;
  # MainWindow resolves the full cref from self._diagramLevelPath().
  elementDeleteRequested = Signal(str)
  connectorDeleteRequested = Signal(str)
  # Right-click "Add Parameter File..." -- '' means the current system level
  # itself (its own boundary/empty canvas), a non-empty name means that
  # element (component or nested system) at the current level. Same
  # empty-string-means-current-level convention as connectorValueRequested.
  addParameterFileRequested = Signal(str)
  # Double-click on a component/system's own "P" badge -- '' means the
  # current system level itself, same convention as addParameterFileRequested.
  editParameterFileRequested = Signal(str)

  def __init__(self, parent=None):
    super().__init__(parent)
    self._scene = DiagramScene(self)
    self.setScene(self._scene)
    self.setRenderHint(QPainter.RenderHint.Antialiasing)
    self.setDragMode(QGraphicsView.DragMode.RubberBandDrag)
    self.setTransformationAnchor(QGraphicsView.ViewportAnchor.AnchorUnderMouse)
    # Needed for the Delete-key shortcut below to ever reach this widget --
    # QGraphicsView doesn't accept keyboard focus by default.
    self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)
    # ...and for _lastHoverPos below to track the mouse even when no button
    # is held (mouseMoveEvent otherwise only fires during an active drag).
    # QAbstractScrollArea delivers real mouse events via the viewport
    # widget, not the QGraphicsView frame itself -- both need tracking on.
    self.setMouseTracking(True)
    self.viewport().setMouseTracking(True)
    self._lastHoverPos: QPointF | None = None

    # _UNSET, not None: setSystem(None) is exactly what shows the empty
    # default canvas (no model loaded/created yet), and it still needs its
    # one-time fit/center -- comparing against a real None here would make
    # that first setSystem(None) look like a no-op ("already showing None")
    # and skip fitting entirely.
    self._currentSystem = _UNSET
    self._connectDragPort: PortItem | None = None
    self._connectDragLine: QGraphicsPathItem | None = None
    self._connectDragPoints: list[QPointF] = []  # corners locked in so far, see _updateConnectDragPreview
    self._connectDragAxis: str | None = None     # 'h' or 'v': the axis the trailing (uncommitted) segment tracks
    self._reshapingConnection: ConnectionItem | None = None
    # True once fitInView has actually run for the level currently shown.
    self._hasFitCurrentLevel = False

  def setSystem(self, system: System | None) -> None:
    '''Rebuilds the scene. Only re-fits the view when navigating to a
    different System (a fresh setSystem call after an edit to the SAME level
    -- e.g. from _onElementMoved or MainWindow's shared refresh -- must not
    reset the user's current pan/zoom).

    The retry-fit for "widget has no real size yet" is deferred via
    QTimer.singleShot rather than hooked into resizeEvent. resizeEvent fires
    for reasons that have nothing to do with the widget's own on-screen size
    changing -- e.g. a scene-rect change (from a drag moving an item far
    enough to toggle scrollbar visibility) can trigger a genuine viewport
    resizeEvent -- and re-fitting there would silently re-center the whole
    view, undoing the drag's visual effect (the committed geometry is
    unaffected, only the camera snaps back). Deferring via the event loop
    instead ties the retry purely to "give layout a chance to finish",
    with no dependency on resize events at all.'''
    isNewLevel = system is not self._currentSystem
    self._currentSystem = system
    self._scene.setSystem(system)
    if isNewLevel:
      self._hasFitCurrentLevel = False
      if not self._fitIfNeeded():
        QTimer.singleShot(0, self._fitIfNeeded)

  def boundaryRectInScene(self) -> QRectF | None:
    return self._scene.boundaryRectInScene()

  def _fitIfNeeded(self) -> bool:
    if self._hasFitCurrentLevel:
      return True
    if self._scene.sceneRect().isEmpty() or self.viewport().rect().isEmpty():
      return False
    self.fitInView(self._scene.sceneRect(), Qt.AspectRatioMode.KeepAspectRatio)
    self._hasFitCurrentLevel = True
    return True

  def wheelEvent(self, event) -> None:
    factor = 1.15 if event.angleDelta().y() > 0 else 1 / 1.15
    self.scale(factor, factor)

  def mouseDoubleClickEvent(self, event) -> None:
    clickedItem = self.itemAt(event.pos())

    if isinstance(clickedItem, ParameterFileBadgeItem):
      host = clickedItem.parentItem()
      elementName = host.name if isinstance(host, ElementIconItem) else ''
      self.editParameterFileRequested.emit(elementName)
      return

    # Radius-based fallback, not just an exact hit -- same reasoning as
    # mousePressEvent's own use of _portAt: a port's own (tiny) drawn shape
    # can miss a slightly-imprecise click that's still unambiguously "at"
    # it, and an unhandled double-click landing on the *connection*
    # underneath instead falls through to super().mouseDoubleClickEvent(),
    # whose default QGraphicsView behavior redispatches as a mousePressEvent
    # -- starting a connection reshape with no matching release ever coming
    # to close it, corrupting that connection (confirmed via a real repro:
    # this silently dropped a connection from the canvas, though the model
    # itself stayed intact).
    port = clickedItem if isinstance(clickedItem, PortItem) else self._portAt(self.mapToScene(event.pos()))
    if port is not None and _elementNameForPort(port) == '':
      # A port on the current system's own boundary (not a child element's
      # port -- see _elementNameForPort) -- double-click sets a fixed value
      # for it rather than drilling in/showing element properties, since a
      # boundary connector has neither.
      self.connectorValueRequested.emit(str(port.connector.name))
      return

    item = clickedItem
    while item is not None and not isinstance(item, ElementIconItem):
      item = item.parentItem()

    if item is not None:
      # duck-typed: a plain System element resolves to itself; a
      # _RootBoxProxy (the model-level "root system as a box" stand-in,
      # see MainWindow) resolves to the real system it wraps.
      target = getattr(item.element, 'system', item.element)
      if isinstance(target, System):
        self.systemDrillDownRequested.emit(target, item.name)
        return
      if isinstance(target, Component):
        self.elementPropertiesRequested.emit(target)
        return

    super().mouseDoubleClickEvent(event)

  def _connectionAt(self, scenePos) -> ConnectionItem | None:
    '''Finds a ConnectionItem near scenePos regardless of z-order/what's
    drawn on top of it. Connections are deliberately drawn *behind* element
    icons, so a stretch of a connection's route that happens to pass
    underneath one is never the topmost item there -- itemAt() would only
    ever find the icon, making that stretch permanently ungrabbable. Scanning
    every connection's own (already hit-tolerance-widened) shape() directly
    sidesteps that, so a segment hidden under an icon can still be dragged
    out into free space.'''
    for item in self._scene.items():
      if isinstance(item, ConnectionItem) and item.shape().contains(item.mapFromScene(scenePos)):
        return item
    return None

  def _portAt(self, scenePos) -> PortItem | None:
    '''Finds the nearest PortItem within _PORT_HIT_RADIUS of scenePos, not
    just an exact hit on its (tiny, ~3-unit) drawn triangle. A port that
    already has a connection sits exactly where that connection's own much
    more forgiving hit-tolerance shape also matches (see _connectionAt) --
    without this, a slightly-off click meant to start a *second* connection
    from an already-wired port would land on the existing connection
    instead (grabbing it to reshape) well before it could ever miss the
    port widely enough to matter. Only ever used to decide whether to start
    a *new* connection drag -- exact-hit itemAt() is still what governs
    Shift+drag-to-move-a-port, which isn't the ambiguity this resolves.'''
    best = None
    bestDist = _PORT_HIT_RADIUS
    for item in self._scene.items():
      if isinstance(item, PortItem):
        pos = item.scenePos()
        dist = math.hypot(pos.x() - scenePos.x(), pos.y() - scenePos.y())
        if dist <= bestDist:
          best = item
          bestDist = dist
    return best

  def mousePressEvent(self, event) -> None:
    # Explicit rather than relying on QWidget's own click-to-focus: several
    # branches below return early without calling super(), which would
    # otherwise skip the base implementation's own focus handling -- and
    # the Delete-key shortcut needs this view to actually hold focus.
    self.setFocus(Qt.FocusReason.MouseFocusReason)
    item = self.itemAt(event.pos())
    if isinstance(item, PortItem) and event.modifiers() & Qt.KeyboardModifier.ShiftModifier:
      # Shift+drag repositions the port itself (PortItem.ItemIsMovable takes
      # over via normal Qt item dragging); plain drag starts a connection.
      super().mousePressEvent(event)
      return

    port = item if isinstance(item, PortItem) else self._portAt(self.mapToScene(event.pos()))
    if port is not None:
      self._connectDragPort = port
      startPos = port.scenePos()
      self._connectDragPoints = [startPos]
      self._connectDragAxis = None
      self._connectDragLine = QGraphicsPathItem()
      self._connectDragLine.setPath(QPainterPath(startPos))
      self._connectDragLine.setPen(QPen(QColor(200, 70, 70), 1.5, Qt.PenStyle.DashLine))
      self._connectDragLine.setZValue(3)
      self._scene.addItem(self._connectDragLine)
      event.accept()
      return

    if event.button() == Qt.MouseButton.LeftButton:
      connection = self._connectionAt(self.mapToScene(event.pos()))
      if connection is not None:
        self._reshapingConnection = connection
        connection.beginReshapeAt(connection.mapFromScene(self.mapToScene(event.pos())))
        event.accept()
        return

    super().mousePressEvent(event)

  def _updateConnectDragPreview(self, currentPos: QPointF) -> None:
    '''Builds the drag-to-connect preview from the path the mouse has
    actually traveled since the drag started, rather than a pre-computed
    shape: the trailing (uncommitted) segment tracks whichever axis
    (horizontal/vertical) the cursor is currently moving along from the
    last locked-in corner, and a new corner locks in the moment the cursor
    picks up enough perpendicular movement to be steering the other way.
    A quick, direct drag that never steers ends up with no interior corners
    at all -- mouseReleaseEvent then leaves connectionGeometry unset so the
    finished connection falls back to defaultRoute's automatic elbow,
    exactly like it did before this existed.'''
    lastCorner = self._connectDragPoints[-1]
    dx = currentPos.x() - lastCorner.x()
    dy = currentPos.y() - lastCorner.y()

    if self._connectDragAxis is None:
      if abs(dx) < _DRAG_STEER_THRESHOLD and abs(dy) < _DRAG_STEER_THRESHOLD:
        livePoint = currentPos
      else:
        self._connectDragAxis = 'h' if abs(dx) >= abs(dy) else 'v'
        livePoint = _axisPoint(lastCorner, currentPos, self._connectDragAxis)
    else:
      livePoint = _axisPoint(lastCorner, currentPos, self._connectDragAxis)
      deviation = abs(dy) if self._connectDragAxis == 'h' else abs(dx)
      if deviation > _DRAG_STEER_THRESHOLD:
        self._connectDragPoints.append(livePoint)
        self._connectDragAxis = 'v' if self._connectDragAxis == 'h' else 'h'
        livePoint = _axisPoint(livePoint, currentPos, self._connectDragAxis)

    path = QPainterPath(self._connectDragPoints[0])
    for point in self._connectDragPoints[1:]:
      path.lineTo(point)
    path.lineTo(livePoint)
    self._connectDragLine.setPath(path)

  def mouseMoveEvent(self, event) -> None:
    self._lastHoverPos = event.pos()
    if self._connectDragPort is not None:
      self._updateConnectDragPreview(self.mapToScene(event.pos()))
      event.accept()
      return
    if self._reshapingConnection is not None:
      connection = self._reshapingConnection
      connection.updateReshape(connection.mapFromScene(self.mapToScene(event.pos())))
      event.accept()
      return
    super().mouseMoveEvent(event)

  def mouseReleaseEvent(self, event) -> None:
    if self._connectDragPort is not None:
      startPort = self._connectDragPort
      self._connectDragPort = None
      self._scene.removeItem(self._connectDragLine)
      self._connectDragLine = None
      # Interior corners actually steered through, excluding the start
      # anchor (that's always the port's own live position, recomputed on
      # render like any other connection's endpoints).
      waypoints = self._connectDragPoints[1:]
      self._connectDragPoints = []
      self._connectDragAxis = None

      targetItem = self.itemAt(event.pos())
      if isinstance(targetItem, PortItem) and targetItem is not startPort:
        self.connectionRequested.emit(
            _elementNameForPort(startPort), str(startPort.connector.name),
            _elementNameForPort(targetItem), str(targetItem.connector.name), waypoints)
      event.accept()
      return
    if self._reshapingConnection is not None:
      connection = self._reshapingConnection
      self._reshapingConnection = None
      connection.endReshapeAt(connection.mapFromScene(self.mapToScene(event.pos())))
      event.accept()
      return
    super().mouseReleaseEvent(event)

  def keyPressEvent(self, event) -> None:
    if event.key() == Qt.Key.Key_Delete and self._deleteUnderCursor():
      event.accept()
      return
    super().keyPressEvent(event)

  def _deleteUnderCursor(self) -> bool:
    '''Resolves whatever's under the *cursor* (not a tracked "selection" --
    this app deliberately never made icons/ports selectable, see
    ElementIconItem's own docstring) the same way contextMenuEvent already
    does, and emits the matching delete-request signal. Uses the last
    hover position tracked by mouseMoveEvent rather than QCursor.pos() --
    both name "wherever the mouse currently is", but the tracked position
    stays reliable under the offscreen platform (used by this project's
    headless tests) and works the same regardless of platform quirks
    around querying the real OS cursor position.

    A port that belongs to a child element (its own FMU-derived connector,
    not independently deletable -- only the whole component can be) is
    deliberately ignored rather than falling through to deleting that
    child element instead.'''
    if self._lastHoverPos is None:
      return False
    viewportPos = self._lastHoverPos
    if not self.viewport().rect().contains(viewportPos):
      return False

    connectionItem = self._connectionAt(self.mapToScene(viewportPos))
    if connectionItem is not None:
      connection = connectionItem.connection
      self.connectionDeleteRequested.emit(
          str(connection.startElement), str(connection.startConnector),
          str(connection.endElement), str(connection.endConnector))
      return True

    item = self.itemAt(viewportPos)

    port = item
    while port is not None and not isinstance(port, PortItem):
      port = port.parentItem()
    if port is not None:
      if _elementNameForPort(port) == '':
        self.connectorDeleteRequested.emit(str(port.connector.name))
        return True
      return False

    element = item
    while element is not None and not isinstance(element, ElementIconItem):
      element = element.parentItem()
    if element is not None:
      self.elementDeleteRequested.emit(element.name)
      return True

    return False

  def contextMenuEvent(self, event) -> None:
    scenePos = self.mapToScene(event.pos())
    # Scanned via _connectionAt, not itemAt(): same reasoning as
    # mousePressEvent -- a connection stretch hidden under an icon should
    # still get "Delete Connection"/"Remove Waypoint", not be shadowed by
    # whatever icon happens to be drawn on top of it there.
    connectionItem = self._connectionAt(scenePos)
    if connectionItem is not None:
      waypointIndex = connectionItem.waypointIndexAt(scenePos)
      menu = QMenu(self)
      removeWaypointAction = menu.addAction('Remove Waypoint') if waypointIndex is not None else None
      deleteAction = menu.addAction('Delete Connection')
      chosen = menu.exec(event.globalPos())
      if removeWaypointAction is not None and chosen == removeWaypointAction:
        connectionItem.removeWaypoint(waypointIndex)
      elif chosen == deleteAction:
        connection = connectionItem.connection
        self.connectionDeleteRequested.emit(
            str(connection.startElement), str(connection.startConnector),
            str(connection.endElement), str(connection.endConnector))
      return

    item = self.itemAt(event.pos())

    if item is None or isinstance(item, SystemBoundaryItem):
      if self._currentSystem is None:
        # No model loaded/created yet (just the blank default canvas) --
        # there's nothing to add a system/component/connector to, so don't
        # even offer the menu rather than showing it and then rejecting
        # whatever gets picked.
        return
      # Empty canvas: add to whatever level is currently shown here. The
      # dashed SystemBoundaryItem covers the whole scene, so a right-click
      # anywhere inside it (not just where nothing is drawn at all) counts
      # as "empty canvas" too.
      clickScenePos = self.mapToScene(event.pos())
      menu = QMenu(self)
      addSystemAction = menu.addAction('Add System...')
      addComponentAction = menu.addAction('Add Component...')
      addConnectorAction = menu.addAction('Add Connector...')
      addParameterFileAction = menu.addAction('Add Parameter File...')
      chosen = menu.exec(event.globalPos())
      if chosen == addSystemAction:
        self.addSystemRequested.emit(clickScenePos)
      elif chosen == addComponentAction:
        self.addComponentRequested.emit(clickScenePos)
      elif chosen == addConnectorAction:
        self.addConnectorRequested.emit(clickScenePos)
      elif chosen == addParameterFileAction:
        self.addParameterFileRequested.emit('')
      return

    element = item
    while element is not None and not isinstance(element, ElementIconItem):
      element = element.parentItem()
    if element is not None:
      menu = QMenu(self)
      addParameterFileAction = menu.addAction('Add Parameter File...')
      chosen = menu.exec(event.globalPos())
      if chosen == addParameterFileAction:
        self.addParameterFileRequested.emit(element.name)
      return

    super().contextMenuEvent(event)
