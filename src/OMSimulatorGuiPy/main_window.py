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

'''MainWindow: owns every open SSP instance and wires them to the views.

M1: File > Open loads an existing .ssp and shows its root System in a
read-only tree (the tree always shows the full nested hierarchy).
M2: adds a read-only diagram canvas showing one System level at a time,
with its own drill-down/"Up" navigation, kept in sync with the tree
selection.
M3: File > New, Save As, and structured editing (add/delete/rename systems,
components, connectors) via the tree's context menu. Every edit goes
through SSP-level methods (never System directly) so resource registration
(addResource) and connector auto-population from FMU modelDescription stay
correct; every edit is followed by one shared _onModelChanged() refresh so
the tree and diagram never drift apart.
Simulation and the XML viewer land in later milestones (see the plan this
was built from).
'''

import os
import tempfile
from pathlib import Path

from PySide6.QtCore import QPointF, QSettings, Qt, QTimer
from PySide6.QtGui import QColor, QIcon, QPainter, QPixmap, QPolygonF
from PySide6.QtWidgets import (
    QDialog,
    QFileDialog,
    QInputDialog,
    QLabel,
    QMainWindow,
    QMessageBox,
    QSplitter,
    QStatusBar,
    QToolBar,
    QVBoxLayout,
    QWidget,
)

from OMSimulator import SSM, SSP, SSV, Connector, CRef, System
from OMSimulator.connection import ConnectionGeometry

from OMSimulatorGui.dialogs.add_connector_dialog import AddConnectorDialog
from OMSimulatorGui.dialogs.add_parameter_file_dialog import AddParameterFileDialog
from OMSimulatorGui.dialogs.add_submodel_dialog import AddSubModelDialog
from OMSimulatorGui.dialogs.add_system_dialog import AddSystemDialog
from OMSimulatorGui.dialogs.connector_value_dialog import ConnectorValueDialog
from OMSimulatorGui.dialogs.create_model_dialog import CreateModelDialog
from OMSimulatorGui.dialogs.edit_parameter_file_dialog import EditParameterFileDialog
from OMSimulatorGui.dialogs.element_properties_dialog import ElementPropertiesDialog
from OMSimulatorGui.dialogs.options_dialog import OptionsDialog
from OMSimulatorGui.dialogs.simulation_settings_dialog import SimulationSettingsDialog
from OMSimulatorGui.dialogs.variants_dialog import VariantsDialog
from OMSimulatorGui.models.system_tree_model import (
    KIND_COMPONENT,
    KIND_COMPONENT_TABLE,
    KIND_CONNECTOR,
    KIND_MODEL,
    KIND_SYSTEM,
    SystemTreeModel,
)
from OMSimulatorGui.simulation.sim_client import SimulationClient
from OMSimulatorGui.views.diagram_canvas import DiagramView, connectorGeometryAt, defaultCanvasCenter, elementGeometryAt
from OMSimulatorGui.views.results_viewer import ResultsViewer
from OMSimulatorGui.views.simulation_progress_panel import SimulationProgressPanel
from OMSimulatorGui.views.system_tree_view import SystemTreeView


def _arrowIcon(color: str, size: int = 24) -> QIcon:
  '''A simple right-pointing triangle, drawn rather than loaded from a file
  since this project has no icon-asset pipeline yet -- used for the
  toolbar's Simulate action (matches OMEdit's own green "run" arrow).'''
  pixmap = QPixmap(size, size)
  pixmap.fill(Qt.GlobalColor.transparent)
  painter = QPainter(pixmap)
  painter.setRenderHint(QPainter.RenderHint.Antialiasing)
  painter.setPen(Qt.PenStyle.NoPen)
  painter.setBrush(QColor(color))
  margin = size * 0.18
  painter.drawPolygon(QPolygonF([
      QPointF(margin, margin),
      QPointF(margin, size - margin),
      QPointF(size - margin, size / 2),
  ]))
  painter.end()
  return QIcon(pixmap)


def _letterIcon(letter: str, color: str, size: int = 24) -> QIcon:
  '''A rounded, colored square with a bold letter -- used for the toolbar's
  Simulation Settings action (an "S" icon, distinct from the green
  Simulate arrow).'''
  pixmap = QPixmap(size, size)
  pixmap.fill(Qt.GlobalColor.transparent)
  painter = QPainter(pixmap)
  painter.setRenderHint(QPainter.RenderHint.Antialiasing)
  painter.setPen(Qt.PenStyle.NoPen)
  painter.setBrush(QColor(color))
  painter.drawRoundedRect(1, 1, size - 2, size - 2, 4, 4)
  painter.setPen(QColor('white'))
  font = painter.font()
  font.setBold(True)
  font.setPixelSize(int(size * 0.65))
  painter.setFont(font)
  painter.drawText(pixmap.rect(), Qt.AlignmentFlag.AlignCenter, letter)
  painter.end()
  return QIcon(pixmap)


class _RootBoxProxy:
  '''Represents the real root System as a single box at the model level,
  without writing its position/size into the real System's own
  elementgeometry -- a root system has no parent to be positioned within,
  so doing that would leak a meaningless <ssd:ElementGeometry> onto the
  <ssd:System> root element itself when the file is exported. The box's
  position/size lives on the proxy instead (session-local: it resets to a
  fallback layout on the next file open, since it was never real data to
  begin with).

  DiagramView resolves double-clicks on a box back to the real system via
  duck-typing (`getattr(item.element, 'system', item.element)`), so this
  needs no special-casing anywhere else.'''

  def __init__(self, system: System):
    self.system = system
    self.elementgeometry = None

  @property
  def name(self):
    return self.system.name

  @property
  def connectors(self):
    return self.system.connectors

  @property
  def parameterResources(self):
    return self.system.parameterResources


class _OpenModel:
  '''One open SSP and its own diagram-navigation state. Any number of these
  can be open at once (see MainWindow._models) -- the tree shows one
  top-level row per open model, and the single shared diagram canvas shows
  whichever model's system the user last navigated to
  (MainWindow._activeModel).'''

  def __init__(self, ssp: SSP, name: str):
    self.ssp = ssp
    self.name = name
    self.modelWrapperSystem: System | None = None
    # Navigation stack for the diagram canvas only -- the tree always shows
    # the full hierarchy; this is a list of (System, displayName) from the
    # synthetic model level down to whatever level is currently shown on the
    # canvas. Index 0 is always the model-level wrapper (see
    # MainWindow._makeModelWrapper) and is excluded from cref paths -- see
    # MainWindow._diagramLevelPath.
    self.diagramStack: list[tuple[System, str]] = []


# Global (QSettings-persisted, Tools > Options) simulation preferences -- see
# MainWindow._onOptionsTriggered/_onSimulateTriggered.
_SETTINGS_ORG = 'OpenModelica'
_SETTINGS_APP = 'OMSimulatorGuiPy'
_SETTINGS_KEY_WORKING_DIR = 'simulation/workingDirectory'
_SETTINGS_KEY_CLI_OPTIONS = 'simulation/commandLineOptions'


class MainWindow(QMainWindow):
  def __init__(self, parent=None):
    super().__init__(parent)
    self.setWindowTitle('OMSimulatorGui')
    self.resize(1200, 800)

    # Every open model (File > New/Open adds one, never replacing an
    # existing entry -- see _addModel/_uniqueModelName), keyed by its own
    # display name. self._activeModel is whichever one the tree/canvas are
    # currently pointed at; self._ssp, self._diagramStack, etc. below are
    # properties reading/writing through to it, so the bulk of this class
    # can keep referring to "the current model" without carrying the
    # distinction explicitly everywhere.
    self._models: dict[str, _OpenModel] = {}
    self._activeModel: _OpenModel | None = None

    # Simulation (see _onSimulateTriggered): only one run at a time across
    # the whole window (not per-model) -- keeping this simple for v1 rather
    # than tracking a run per open model. self._resultsViewers keeps every
    # opened ResultsViewer window alive (nothing else references them once
    # _onSimulationFinished returns).
    self._simulationClient: SimulationClient | None = None
    self._resultsViewers: list[ResultsViewer] = []

    self._treeModel = SystemTreeModel(self)
    self._treeView = SystemTreeView(self)
    self._treeView.setModel(self._treeModel)
    # `clicked` (not `currentChanged`) deliberately: currentChanged fires for
    # ANY current-index change, including Qt's own internal bookkeeping --
    # e.g. QTreeView silently auto-selecting row 0 the first time it becomes
    # visible with no explicit selection, sometimes deferred until the next
    # time Qt processes events (which can coincide with an unrelated
    # interaction, like starting a canvas drag) -- which would otherwise
    # reset the diagram's navigation out from under the user. `clicked` only
    # fires for a genuine user mouse click on a row.
    self._treeView.clicked.connect(self._onTreeItemClicked)
    self._treeView.addSystemRequested.connect(self._onAddSystemRequested)
    self._treeView.addComponentRequested.connect(self._onAddComponentRequested)
    self._treeView.addConnectorRequested.connect(self._onAddConnectorRequested)
    self._treeView.addParameterFileRequested.connect(self._onAddParameterFileRequested)
    self._treeView.editParameterFileRequested.connect(self._onEditParameterFileRequested)
    self._treeView.swapParameterFileRequested.connect(self._onSwapParameterFileRequested)
    self._treeView.removeParameterFileRequested.connect(self._onRemoveParameterFileRequested)
    self._treeView.exportSSVTemplateRequested.connect(self._onExportSSVTemplateRequested)
    self._treeView.exportSSMTemplateRequested.connect(self._onExportSSMTemplateRequested)
    self._treeView.addResourceRequested.connect(self._onAddResourceRequested)
    self._treeView.removeResourceRequested.connect(self._onRemoveResourceRequested)
    self._treeView.deleteRequested.connect(self._onDeleteRequested)
    self._treeView.renameRequested.connect(self._onRenameRequested)
    self._treeView.propertiesRequested.connect(self._onPropertiesRequested)

    self._diagramView = DiagramView(self)
    self._diagramView.systemDrillDownRequested.connect(self._onDrillDownRequested)
    self._diagramView.connectionRequested.connect(self._onConnectionRequested)
    self._diagramView.connectionDeleteRequested.connect(self._onConnectionDeleteRequested)
    self._diagramView.addSystemRequested.connect(self._onCanvasAddSystemRequested)
    self._diagramView.addComponentRequested.connect(self._onCanvasAddComponentRequested)
    self._diagramView.addConnectorRequested.connect(self._onCanvasAddConnectorRequested)
    self._diagramView.elementPropertiesRequested.connect(self._onCanvasPropertiesRequested)
    self._diagramView.elementDeleteRequested.connect(self._onCanvasElementDeleteRequested)
    self._diagramView.connectorDeleteRequested.connect(self._onCanvasConnectorDeleteRequested)
    self._diagramView.connectorValueRequested.connect(self._onCanvasConnectorValueRequested)
    self._diagramView.addParameterFileRequested.connect(self._onCanvasAddParameterFileRequested)
    self._diagramView.editParameterFileRequested.connect(self._onCanvasEditParameterFileRequested)
    # Fits/centers the empty default canvas immediately -- without this,
    # DiagramView.setSystem() (the only place that ever calls setSceneRect
    # and fitInView) never runs until a model is actually loaded, so the
    # blank canvas would sit wherever Qt's unfit default transform happens to
    # place it instead of centered in the view.
    #
    # Must be deferred, not called inline: at this point in __init__ the view
    # isn't parented into any layout yet, so its viewport reports Qt's tiny
    # pre-layout default size -- nonzero, so _fitIfNeeded's own "not sized
    # yet" guard (which only checks .isEmpty()) doesn't catch it, and it
    # happily "succeeds" against that wrong tiny size. Once _hasFitCurrentLevel
    # is set, _fitIfNeeded never retries, so that undersized fit would stick
    # even after the window reaches its real size. Deferring via
    # QTimer.singleShot(0, ...) runs this after the window construction below
    # has finished and the caller's show() has run, so the *first* fit
    # attempt already sees the real viewport.
    QTimer.singleShot(0, lambda: self._diagramView.setSystem(None))

    # The simulation progress panel lives below the canvas rather than as a
    # separate window (see SimulationProgressPanel's own docstring) -- it
    # starts hidden and only takes up space once a simulation is running.
    self._simulationPanel = SimulationProgressPanel(self)
    diagramContainer = QWidget(self)
    diagramLayout = QVBoxLayout(diagramContainer)
    diagramLayout.setContentsMargins(0, 0, 0, 0)
    diagramLayout.addWidget(self._diagramView, 1)
    diagramLayout.addWidget(self._simulationPanel)

    splitter = QSplitter(self)
    splitter.addWidget(self._treeView)
    splitter.addWidget(diagramContainer)
    splitter.setStretchFactor(0, 0)
    splitter.setStretchFactor(1, 1)
    splitter.setSizes([300, 900])
    self.setCentralWidget(splitter)

    self._breadcrumbLabel = QLabel(self)
    diagramToolbar = QToolBar('Diagram', self)
    diagramToolbar.setMovable(False)

    self._upAction = diagramToolbar.addAction('Up')
    self._upAction.setEnabled(False)
    self._upAction.triggered.connect(self._onUpTriggered)
    diagramToolbar.addSeparator()

    # Simulation Settings + Simulate are grouped with the other toolbar
    # controls on the left, ahead of the breadcrumb -- keeps their position
    # fixed regardless of how long the breadcrumb text grows, and avoids a
    # button pair floating in an otherwise-empty toolbar.
    settingsAction = diagramToolbar.addAction(_letterIcon('S', '#455a64'), 'Simulation Settings')
    settingsAction.triggered.connect(self._onSimulationSettingsTriggered)
    simulateAction = diagramToolbar.addAction(_arrowIcon('#2e7d32'), 'Simulate')
    simulateAction.triggered.connect(self._onSimulateTriggered)
    diagramToolbar.addSeparator()

    diagramToolbar.addWidget(self._breadcrumbLabel)

    self.addToolBar(diagramToolbar)

    self.setStatusBar(QStatusBar(self))

    self._buildMenus()

  def _buildMenus(self) -> None:
    fileMenu = self.menuBar().addMenu('&File')

    newAction = fileMenu.addAction('&New SSP Model...')
    newAction.setShortcut('Ctrl+N')
    newAction.triggered.connect(self._onNewTriggered)

    openAction = fileMenu.addAction('&Open...')
    openAction.setShortcut('Ctrl+O')
    openAction.triggered.connect(self._onOpenTriggered)

    saveAsAction = fileMenu.addAction('Save &As...')
    saveAsAction.setShortcut('Ctrl+Shift+S')
    saveAsAction.triggered.connect(self._onSaveAsTriggered)

    fileMenu.addSeparator()

    exitAction = fileMenu.addAction('E&xit')
    exitAction.setShortcut('Ctrl+Q')
    exitAction.triggered.connect(self.close)

    modelMenu = self.menuBar().addMenu('&Model')
    simulationSettingsAction = modelMenu.addAction('&Simulation Settings...')
    simulationSettingsAction.triggered.connect(self._onSimulationSettingsTriggered)
    variantsAction = modelMenu.addAction('&Variants...')
    variantsAction.triggered.connect(self._onVariantsTriggered)
    modelMenu.addSeparator()
    simulateAction = modelMenu.addAction('S&imulate')
    simulateAction.setShortcut('Ctrl+R')
    simulateAction.triggered.connect(self._onSimulateTriggered)

    toolsMenu = self.menuBar().addMenu('&Tools')
    optionsAction = toolsMenu.addAction('&Options...')
    optionsAction.triggered.connect(self._onOptionsTriggered)

  # --- Active-model properties -------------------------------------------------
  # Thin accessors over self._activeModel, so the rest of this class can keep
  # referring to "self._ssp"/"self._diagramStack" etc. as if there were only
  # one model, while actually always meaning "the currently active one".

  @property
  def _ssp(self) -> SSP | None:
    return self._activeModel.ssp if self._activeModel is not None else None

  @property
  def _modelName(self) -> str:
    return self._activeModel.name if self._activeModel is not None else 'Model'

  @property
  def _modelWrapperSystem(self) -> System | None:
    return self._activeModel.modelWrapperSystem if self._activeModel is not None else None

  @_modelWrapperSystem.setter
  def _modelWrapperSystem(self, value: System | None) -> None:
    if self._activeModel is not None:
      self._activeModel.modelWrapperSystem = value

  @property
  def _diagramStack(self) -> list:
    return self._activeModel.diagramStack if self._activeModel is not None else []

  @_diagramStack.setter
  def _diagramStack(self, value: list) -> None:
    if self._activeModel is not None:
      self._activeModel.diagramStack = value

  def _uniqueModelName(self, baseName: str) -> str:
    '''Disambiguates two open models that happen to share the same SSD name
    (e.g. two different files both internally named "default") -- File >
    New/Open must never let a second model silently replace or merge with
    an already-open one just because their names collide.'''
    if baseName not in self._models:
      return baseName
    n = 2
    while f'{baseName} ({n})' in self._models:
      n += 1
    return f'{baseName} ({n})'

  def _renameModel(self, model: '_OpenModel', newBaseName: str) -> None:
    '''Re-keys self._models under a new display name -- currently only used
    after activating a different variant (see _onVariantsTriggered), since
    the tree's top-level row for a model is meant to track whichever variant
    is currently active, the same way it's chosen from the active variant's
    name when the model is first opened (see _addModel). Deduplicates via
    _uniqueModelName exactly like a newly-opened model would, so switching to
    a variant name that collides with some other already-open model's name
    doesn't silently merge the two rows.'''
    if newBaseName == model.name:
      return
    del self._models[model.name]
    model.name = self._uniqueModelName(newBaseName)
    self._models[model.name] = model

  def _activateModelForNode(self, node) -> bool:
    '''Tree context-menu actions can target any open model's node directly
    (SystemTreeView's context menu acts on whatever was right-clicked,
    without first requiring a left-click to select it into focus -- see
    _onTreeItemClicked's own docstring on why left-click uses `clicked` and
    not `currentChanged`). Make sure self._activeModel -- and therefore
    self._ssp, self._diagramStack, etc. -- actually matches the node's own
    model before any operation on it runs, so right-clicking a node that
    belongs to a model other than the currently-active one doesn't silently
    edit the wrong SSP.'''
    modelNode = node
    while modelNode is not None and modelNode.kind != KIND_MODEL:
      modelNode = modelNode.parent
    if modelNode is None:
      return False
    model = self._models.get(modelNode.label)
    if model is None:
      return False
    self._activeModel = model
    return True

  # --- File actions ----------------------------------------------------------

  def _onNewTriggered(self) -> None:
    dialog = CreateModelDialog(self)
    if dialog.exec() != QDialog.DialogCode.Accepted:
      return

    ssp = SSP()
    # SSD.name has no owning-SSP awareness: a plain assignment leaves
    # ssp.variants' dict key and ssp.activeVariantName pointing at the old
    # ('default') name, and SSP.export() decides which SSD becomes the
    # required SystemStructure.ssd by comparing ssd.name == activeVariantName
    # -- so an un-re-keyed rename silently breaks export/reload. Re-key by
    # hand since the library has no renameVariant().
    ssd = ssp.activeVariant
    oldVariantName = ssd.name
    ssd.name = dialog.modelName()
    del ssp.variants[oldVariantName]
    ssp.variants[ssd.name] = ssd
    ssp.activeVariantName = ssd.name

    ssd.system.name = dialog.rootSystemName()
    self._addModel(ssp)
    self.setWindowTitle(f'OMSimulatorGui - {self._activeModel.name}')
    self.statusBar().showMessage('New model created', 5000)

  def _onOpenTriggered(self) -> None:
    path, _ = QFileDialog.getOpenFileName(self, 'Open SSP', '', 'SSP files (*.ssp)')
    if path:
      self.openFile(path)

  def openFile(self, path: str) -> None:
    '''Loads `path` as a newly-open SSP (added alongside any already-open
    models, never replacing one) and refreshes the tree and diagram.'''
    try:
      ssp = SSP(path)
    except Exception as e:
      QMessageBox.critical(self, 'Failed to open', f'Could not open "{path}":\n{e}')
      return

    self._addModel(ssp)

    self.setWindowTitle(f'OMSimulatorGui - {self._activeModel.name}')
    self.statusBar().showMessage(f'Loaded {path}', 5000)

  def _onSaveAsTriggered(self) -> None:
    if self._ssp is None:
      return
    path, _ = QFileDialog.getSaveFileName(self, 'Save SSP', '', 'SSP files (*.ssp)')
    if not path:
      return
    try:
      self._ssp.export(path)
    except Exception as e:
      QMessageBox.critical(self, 'Save failed', f'Could not save "{path}":\n{e}')
      return
    self.setWindowTitle(f'OMSimulatorGui - {Path(path).name}')
    self.statusBar().showMessage(f'Saved {path}', 5000)

  def _addModel(self, ssp: SSP) -> None:
    '''Registers a newly created/opened SSP as a new entry in self._models
    (disambiguated via _uniqueModelName if its name clashes with an
    already-open one) and makes it the active model -- so File > New/Open
    always grows the tree with a new top-level row instead of silently
    replacing whatever was open before.'''
    variant = ssp.activeVariant
    baseName = variant.name if variant is not None else 'Model'
    name = self._uniqueModelName(baseName)

    model = _OpenModel(ssp, name)
    self._rebuildDiagramWrapper(model)
    self._models[name] = model
    self._activeModel = model

    self._refreshTree()
    self._updateDiagram()

  def _rebuildDiagramWrapper(self, model: _OpenModel) -> None:
    '''(Re)builds a model's throwaway model-level diagram wrapper and resets
    its navigation stack from its SSP's *current* active variant. Needed not
    just when a model is first opened but also whenever its active variant
    changes (see _onVariantsTriggered) -- the wrapper/stack are built from a
    particular variant's root System object, so switching variants leaves
    them pointing at a system that's no longer the one being edited.'''
    variant = model.ssp.activeVariant
    rootSystem = variant.system if variant is not None else None
    model.modelWrapperSystem = self._makeModelWrapper(rootSystem) if rootSystem is not None else None
    model.diagramStack = [(model.modelWrapperSystem, model.name)] if rootSystem is not None else []

  def _refreshTree(self) -> None:
    '''Rebuilds the tree from every currently-open model's root System --
    the diagram mirrors whichever one is active with a synthetic "model
    level" showing that model's root system as a single box (its own
    connectors as ports) -- double-clicking it drills in exactly like any
    nested subsystem, reusing the same ElementIconItem/drill-down machinery.
    The model level is never part of any cref -- see _diagramLevelPath.'''
    models = [(model.ssp.activeVariant.system, name, model.ssp) for name, model in self._models.items()
              if model.ssp.activeVariant is not None and model.ssp.activeVariant.system is not None]
    self._treeModel.setModels(models)
    self._treeView.expandAll()

  @staticmethod
  def _makeModelWrapper(rootSystem: System) -> System:
    '''A throwaway System whose only "element" is a _RootBoxProxy wrapping
    the real root system -- never exported, purely so DiagramScene.setSystem()
    can render the root system as a single box (with its own ports) the same
    way it renders any other element. Rebuilt whenever the SSP is (re)loaded;
    edits to rootSystem's own contents are visible through it automatically
    since the proxy delegates to the same live object.

    The proxy starts centered on the default canvas rather than left at its
    initial elementgeometry=None, which would otherwise fall into the
    fallback grid layout's top-left starting slot -- same as any other
    element without a position of its own, just centered instead of
    corner-anchored since this is the one box shown alone at this level.
    Purely a starting point: dragging it (session-local, see the proxy's own
    docstring) overrides this like any other elementgeometry.'''
    wrapper = System(str(rootSystem.name))
    proxy = _RootBoxProxy(rootSystem)
    proxy.elementgeometry = elementGeometryAt(defaultCanvasCenter())
    wrapper.elements = {str(rootSystem.name): proxy}
    return wrapper

  # --- Shared refresh after any edit -----------------------------------------

  def _onModelChanged(self) -> None:
    self._syncModelWrapperKey()
    self._treeModel.refresh()
    self._treeView.expandAll()
    self._updateDiagram()

  def _syncModelWrapperKey(self) -> None:
    '''The model-level wrapper's single "element" entry (see
    _makeModelWrapper) is keyed by the root system's name as of whenever
    that wrapper was last (re)built -- initial load, or a variant switch
    (_rebuildDiagramWrapper) -- not re-derived on every edit. Renaming the
    root system changes rootSystem.name directly (SSD.rename's own
    "renaming the root system itself" branch touches nothing else, there's
    no parent elements dict to re-key for a root) but leaves that stale key
    behind -- and DiagramScene.setSystem's element loop labels a box from
    the dict key (`for name, element in system.elements.items()`), not
    element.name, so the model-level box kept showing the old name after a
    rename. Re-key in place (reusing the same _RootBoxProxy, so its
    session-local position isn't lost) rather than a full
    _rebuildDiagramWrapper, which would also reset diagramStack and discard
    wherever the user is currently drilled into.'''
    model = self._activeModel
    wrapper = model.modelWrapperSystem if model is not None else None
    if wrapper is None:
      return
    variant = model.ssp.activeVariant if model.ssp is not None else None
    rootSystem = variant.system if variant is not None else None
    if rootSystem is None:
      return
    currentKey = next(iter(wrapper.elements), None)
    newKey = str(rootSystem.name)
    if currentKey is not None and currentKey != newKey:
      wrapper.elements = {newKey: wrapper.elements[currentKey]}

  # --- Diagram navigation ------------------------------------------------------

  def _updateDiagram(self) -> None:
    system = self._diagramStack[-1][0] if self._diagramStack else None
    self._diagramView.setSystem(system)
    self._breadcrumbLabel.setText(' > '.join(name for _, name in self._diagramStack))
    self._upAction.setEnabled(len(self._diagramStack) > 1)

  def _onDrillDownRequested(self, system: System, name: str) -> None:
    self._diagramStack.append((system, name))
    self._updateDiagram()

  def _onUpTriggered(self) -> None:
    if len(self._diagramStack) > 1:
      self._diagramStack.pop()
      self._updateDiagram()

  def _diagramLevelPath(self) -> list[str]:
    '''The [rootSystemName, ..., currentLevelName] path for whatever system
    is currently shown on the diagram canvas, skipping the synthetic model
    level at index 0 (the API never sees the model name, only system names).'''
    return [name for _, name in self._diagramStack[1:]]

  def _connectionCref(self, elementName: str, connectorName: str) -> CRef:
    basePath = self._diagramLevelPath()
    if elementName:
      return CRef(*basePath, elementName, connectorName)
    return CRef(*basePath, connectorName)

  def _onConnectionRequested(self, elem1: str, conn1: str, elem2: str, conn2: str, waypoints=None) -> None:
    system = self._diagramStack[-1][0]
    try:
      self._ssp.addConnection(self._connectionCref(elem1, conn1), self._connectionCref(elem2, conn2))
    except Exception as e:
      QMessageBox.critical(self, 'Add Connection failed', str(e))
      return
    if waypoints:
      # System.addConnection may store the connection in flipped order
      # (start/end swapped) if that's the only causality-valid direction --
      # it's always appended last regardless. Reverse the steered waypoints
      # to match if so, so the saved shape still traces the path the user
      # actually dragged rather than running backwards.
      connection = system.connections[-1]
      if str(connection.startElement) != elem1 or str(connection.startConnector) != conn1:
        waypoints = list(reversed(waypoints))
      connection.connectionGeometry = ConnectionGeometry(
          pointsX=[p.x() for p in waypoints], pointsY=[-p.y() for p in waypoints])
    self._onModelChanged()

  def _onConnectionDeleteRequested(self, elem1: str, conn1: str, elem2: str, conn2: str) -> None:
    label1 = f'{elem1}.{conn1}' if elem1 else conn1
    label2 = f'{elem2}.{conn2}' if elem2 else conn2
    if QMessageBox.question(self, 'Delete Connection', f'Delete connection {label1} -> {label2}?') != QMessageBox.StandardButton.Yes:
      return
    try:
      self._ssp.deleteConnection(self._connectionCref(elem1, conn1), self._connectionCref(elem2, conn2))
    except Exception as e:
      QMessageBox.critical(self, 'Delete Connection failed', str(e))
      return
    self._onModelChanged()

  def _onTreeItemClicked(self, index) -> None:
    '''Clicking a tree item navigates the diagram to show that item *in
    context* -- as a box at its parent's level -- rather than drilling into
    its own contents (that's what double-clicking its box on the canvas is
    for). So clicking the model row or the root system both land on the
    model level (where the root system appears as a box); clicking a nested
    system lands one level up, wherever its own box lives.

    Clicking a row that belongs to a different (currently inactive) open
    model switches self._activeModel first, so the diagram switches to that
    model's own navigation stack rather than staying on whichever model was
    active before.'''
    node = self._treeModel.nodeFromIndex(index)
    if node is None:
      return
    if not self._activateModelForNode(node):
      return

    if node.kind == KIND_MODEL:
      self._diagramStack = [(self._modelWrapperSystem, self._modelName)]
      self._updateDiagram()
      return

    if node.kind != KIND_SYSTEM:
      return

    path: list[tuple[System, str]] = []
    n = node
    while n is not None and n.kind == KIND_SYSTEM:
      path.append((n.obj, str(n.obj.name)))
      n = n.parent
    path.reverse()

    parentPath = path[:-1]  # show the clicked system itself, not its insides
    self._diagramStack = [(self._modelWrapperSystem, self._modelName), *parentPath]
    self._updateDiagram()

  # --- Structured editing ------------------------------------------------------

  def _crefPath(self, node) -> list[str]:
    '''Builds the [rootName, ..., thisNodeName] path SSP/SSD/System methods
    expect, by walking the TreeNode ancestor chain. Stops at the KIND_SYSTEM
    boundary -- the tree's model-name wrapper row (KIND_MODEL) sits above the
    root system but is never part of the path; the API always operates on
    system names.'''
    if node.kind == KIND_SYSTEM:
      segments = []
      n = node
      while n is not None and n.kind == KIND_SYSTEM:
        segments.append(str(n.obj.name))
        n = n.parent
      segments.reverse()
      return segments
    if node.kind in (KIND_COMPONENT, KIND_COMPONENT_TABLE):
      return self._crefPath(node.parent) + [str(node.obj.name)]
    if node.kind == KIND_CONNECTOR:
      # node.parent is the "Connectors" group node; its parent is the owning system.
      return self._crefPath(node.parent.parent) + [str(node.obj.name)]
    raise ValueError(f'Cannot build a path for node kind {node.kind!r}')

  def _requireNonEmptyPath(self, path: list[str]) -> bool:
    '''Right-clicking empty canvas at the model level (viewing the root
    system as a box, before drilling into it) has no valid container to add
    into -- _diagramLevelPath() is empty there by design (the model level
    isn't a real, addressable system). Tree-triggered calls never hit this,
    since _crefPath always includes at least the root system's own name.'''
    if path:
      return True
    QMessageBox.information(self, 'Nothing to add to',
                             'Double-click into the root system first, then add to its contents.')
    return False

  def _positionNewElement(self, name: str, scenePos) -> None:
    '''Places a canvas-added system/component at the cursor's scene position
    instead of leaving it for the fallback grid layout, which always fills
    from the top-left corner regardless of where the user actually
    right-clicked. Only called for canvas-triggered adds (scenePos is only
    ever non-None there), where self._diagramStack[-1][0] is exactly the
    system path addressed -- the same one the element was just added to.'''
    system = self._diagramStack[-1][0]
    element = system.elements.get(CRef(name))
    if element is not None:
      element.elementgeometry = elementGeometryAt(scenePos)

  def _onAddSystemRequested(self, node) -> None:
    if not self._activateModelForNode(node):
      return
    self._addSystemAtPath(self._crefPath(node))

  def _onCanvasAddSystemRequested(self, scenePos) -> None:
    self._addSystemAtPath(self._diagramLevelPath(), scenePos)

  def _addSystemAtPath(self, path: list[str], scenePos=None) -> None:
    if not self._requireNonEmptyPath(path):
      return
    dialog = AddSystemDialog(self)
    if dialog.exec() != QDialog.DialogCode.Accepted:
      return
    try:
      # SSD.addSystem validates the cref's first segment against the SSD
      # variant's own name (self._name), not the root System's name -- unlike
      # every sibling method (addComponent/addConnector/delete/rename), which
      # validate via _validateCref against self.system.name. Substitute the
      # variant name for the first segment so this still works even when the
      # root system has been renamed away from the variant name.
      path = [*path]
      path[0] = self._ssp.activeVariant.name
      self._ssp.addSystem(CRef(*path, dialog.name()))
      if scenePos is not None:
        self._positionNewElement(dialog.name(), scenePos)
    except Exception as e:
      QMessageBox.critical(self, 'Add System failed', str(e))
      return
    self._onModelChanged()

  def _onAddComponentRequested(self, node) -> None:
    if not self._activateModelForNode(node):
      return
    self._addComponentAtPath(self._crefPath(node))

  def _onCanvasAddComponentRequested(self, scenePos) -> None:
    self._addComponentAtPath(self._diagramLevelPath(), scenePos)

  def _addComponentAtPath(self, path: list[str], scenePos=None) -> None:
    if not self._requireNonEmptyPath(path):
      return
    dialog = AddSubModelDialog(self)
    if dialog.exec() != QDialog.DialogCode.Accepted:
      return
    try:
      resourceName = f'resources/{Path(dialog.filePath()).name}'
      if resourceName not in self._ssp.resources:
        self._ssp.addResource(dialog.filePath())
      self._ssp.addComponent(CRef(*path, dialog.name()), resourceName)
      if scenePos is not None:
        self._positionNewElement(dialog.name(), scenePos)
    except Exception as e:
      QMessageBox.critical(self, 'Add Component failed', str(e))
      return
    self._onModelChanged()

  def _onAddConnectorRequested(self, node) -> None:
    if not self._activateModelForNode(node):
      return
    self._addConnectorAtPath(self._crefPath(node))

  def _onCanvasAddConnectorRequested(self, scenePos) -> None:
    self._addConnectorAtPath(self._diagramLevelPath(), scenePos)

  def _addConnectorAtPath(self, path: list[str], scenePos=None) -> None:
    if not self._requireNonEmptyPath(path):
      return
    dialog = AddConnectorDialog(self)
    if dialog.exec() != QDialog.DialogCode.Accepted:
      return
    try:
      connector = Connector(dialog.name(), dialog.causality(), dialog.signalType())
      if scenePos is not None:
        boundaryRect = self._diagramView.boundaryRectInScene()
        if boundaryRect is not None:
          connector.connectorGeometry = connectorGeometryAt(scenePos, boundaryRect)
      self._ssp.addConnector(CRef(*path), connector)
    except Exception as e:
      QMessageBox.critical(self, 'Add Connector failed', str(e))
      return
    self._onModelChanged()

  def _onAddParameterFileRequested(self, node) -> None:
    if not self._activateModelForNode(node):
      return
    self._addParameterFileAtPath(self._crefPath(node))

  def _onExportSSVTemplateRequested(self, node) -> None:
    '''SSP.exportSSVTemplate(cref, filename): walks everything reachable
    under `cref` (a system, subsystem, or component) and writes a ready-made
    .ssv file with every connector/parameter target found there, using
    whatever value each one currently has -- a scaffold to hand-edit or
    attach later via "Add Parameter File... > existing resource", not a
    reference that gets attached automatically (no model mutation happens
    here at all, so no _onModelChanged() call is needed).'''
    if not self._activateModelForNode(node):
      return
    path = self._crefPath(node)
    savePath, _ = QFileDialog.getSaveFileName(self, 'Export SSV Template', f'{node.label}.ssv', 'SSV files (*.ssv)')
    if not savePath:
      return
    try:
      self._ssp.exportSSVTemplate(CRef(*path), savePath)
    except Exception as e:
      QMessageBox.critical(self, 'Export SSV Template failed', str(e))

  def _onExportSSMTemplateRequested(self, node) -> None:
    '''Same idea as _onExportSSVTemplateRequested but for a mapping skeleton
    (every target listed with an empty source="", for the user to fill in
    externally) -- SSP.exportSSMTemplate(cref, filename).'''
    if not self._activateModelForNode(node):
      return
    path = self._crefPath(node)
    savePath, _ = QFileDialog.getSaveFileName(self, 'Export SSM Template', f'{node.label}.ssm', 'SSM files (*.ssm)')
    if not savePath:
      return
    try:
      self._ssp.exportSSMTemplate(CRef(*path), savePath)
    except Exception as e:
      QMessageBox.critical(self, 'Export SSM Template failed', str(e))

  def _canvasElementPath(self, elementName: str) -> list[str]:
    '''cref path for a right-clicked canvas box named `elementName`. Usually
    a child at the currently drilled-into level (_diagramLevelPath() +
    elementName); but at the model level, _diagramLevelPath() is empty and
    the one box shown there is the whole root system itself (a
    _RootBoxProxy stand-in, see MainWindow) -- elementName there already
    *is* the root system's own name (ElementIconItem.name / _RootBoxProxy.name
    both resolve to it), so the single-segment path IS the full cref, the
    same shape _crefPath(node) already produces for the root System's own
    tree row.'''
    path = self._diagramLevelPath()
    return [*path, elementName] if path else [elementName]

  def _onCanvasAddParameterFileRequested(self, elementName: str) -> None:
    self._addParameterFileAtPath(self._canvasElementPath(elementName))

  def _addParameterFileAtPath(self, path: list[str]) -> None:
    availableSsvResources = [r for r in self._ssp.listResource() if r.endswith('.ssv')]
    availableSsmResources = [r for r in self._ssp.listResource() if r.endswith('.ssm')]
    dialog = AddParameterFileDialog(availableSsvResources, availableSsmResources, self)
    if dialog.exec() != QDialog.DialogCode.Accepted:
      return
    try:
      ssvResource = dialog.existingSsvResource()
      if ssvResource is None:
        ssvPath = dialog.ssvPath()
        ssvResource = f'resources/{Path(ssvPath).name}'
        if ssvResource not in self._ssp.resources:
          self._ssp.addResource(ssvPath, ssvResource)

      ssmResource = dialog.existingSsmResource()
      if ssmResource is None:
        ssmPath = dialog.ssmPath()
        if ssmPath:
          ssmResource = f'resources/{Path(ssmPath).name}'
          if ssmResource not in self._ssp.resources:
            self._ssp.addResource(ssmPath, ssmResource)

      self._ssp.addSSVReference(CRef(*path), ssvResource, ssmResource)
    except Exception as e:
      QMessageBox.critical(self, 'Add Parameter File failed', str(e))
      return
    self._onModelChanged()

  def _onEditParameterFileRequested(self, node) -> None:
    if not self._activateModelForNode(node):
      return
    ssvResource, ssmResource = node.obj
    # node.parent is the "Parameter Files" group node; its parent is the
    # owning System/Component -- same ancestor-skip _crefPath already does
    # for KIND_CONNECTOR via its "Connectors" group parent (see also
    # _onRemoveParameterFileRequested).
    self._editParameterFileResource(ssvResource, ssmResource, self._crefPath(node.parent.parent))

  def _onSwapParameterFileRequested(self, node) -> None:
    '''SSP.swapSSVReference(cref, resource1, resource2): re-points an
    already-attached binding from resource1 (the currently-attached SSV) to
    a DIFFERENT already-registered SSV resource, at the same cref -- unlike
    "Edit Values...", which edits the current file's own content in place,
    this changes *which* file is referenced (e.g. switching a component
    between two alternate parameter presets both already added via
    "Add Resource..."). Only ever offered on an already-attached entry, so
    resource1 (the current one) always exists; resource2 must be picked
    from other already-registered .ssv resources -- swapSSVReference itself
    only warns (doesn't raise) if asked to swap in something unregistered,
    but there's no "browse a new file" case here, only "pick another
    resource that's already in the pool" (see [[project_omsimulatorguipy]]
    for why this mirrors testsuite/tests/api/swapSSV4.py's own usage).'''
    if not self._activateModelForNode(node):
      return
    ssvResource, ssmResource = node.obj
    choices = [r for r in self._ssp.listResource() if r.endswith('.ssv') and r != ssvResource]
    if not choices:
      QMessageBox.information(self, 'Swap Resource',
                               'No other .ssv resource is registered yet -- use "Add Resource..." first.')
      return
    chosen, ok = QInputDialog.getItem(self, 'Swap Resource', f'Replace "{Path(ssvResource).name}" with:', choices, editable=False)
    if not ok:
      return
    if ssmResource is not None:
      # Component/System.swapSSVReference always re-adds the new resource
      # with no ssm (addSSVReference(resource2), no second arg) -- so an
      # attached mapping is silently dropped by the swap itself. Surface
      # that up front rather than letting it be a quiet surprise.
      if QMessageBox.question(
          self, 'Swap Resource',
          f'"{Path(ssvResource).name}" has a parameter mapping ("{Path(ssmResource).name}") attached. '
          'Swapping will drop that mapping -- the new file will have none until one is added back. Continue?'
      ) != QMessageBox.StandardButton.Yes:
        return
    try:
      self._ssp.swapSSVReference(CRef(*self._crefPath(node.parent.parent)), ssvResource, chosen)
    except Exception as e:
      QMessageBox.critical(self, 'Swap Resource failed', str(e))
      return
    self._onModelChanged()

  def _onCanvasEditParameterFileRequested(self, elementName: str) -> None:
    path = self._canvasElementPath(elementName)
    # Badge-driven edit has no single TreeNode to read the resource pair off
    # of (see _onEditParameterFileRequested) -- ask the model for whatever is
    # attached at this cref instead. listSSVReference can still return
    # already-empty {} entries left behind by a prior removeSSVReference (see
    # _addParameterFileNodes's docstring); flattening via .items() skips
    # those the same way (nothing to iterate in an empty dict).
    entries = self._ssp.listSSVReference(CRef(*path))
    pairs = [(ssvResource, ssmResource) for entry in entries for ssvResource, ssmResource in entry.items()]
    if not pairs:
      return
    ssvResource, ssmResource = pairs[0]
    if len(pairs) > 1:
      choices = [ssv for ssv, _ssm in pairs]
      chosen, ok = QInputDialog.getItem(self, 'Edit Parameter File', 'Choose a file to edit:', choices, editable=False)
      if not ok:
        return
      ssvResource, ssmResource = next((ssv, ssm) for ssv, ssm in pairs if ssv == chosen)
    self._editParameterFileResource(ssvResource, ssmResource, path)

  def _editParameterFileResource(self, ssvResource: str, ssmResource: str | None, path: list[str]) -> None:
    ssv = self._ssp.resources.get(ssvResource)
    if not isinstance(ssv, SSV):
      return
    ssm = self._ssp.resources.get(ssmResource) if ssmResource else None
    availableSsmResources = [r for r in self._ssp.listResource() if r.endswith('.ssm')]
    dialog = EditParameterFileDialog(ssv, ssm if isinstance(ssm, SSM) else None, availableSsmResources, self)
    if dialog.exec() != QDialog.DialogCode.Accepted:
      return

    newSsmPath = dialog.newSsmPath()
    existingSsmResource = dialog.existingSsmResource()
    if newSsmPath:
      # A brand-new mapping was created for a file that had none -- attach it
      # by re-adding the same ssv reference with the new ssm alongside it
      # (removeSSVReference only clears the key, not the whole entry -- see
      # _addParameterFileNodes's docstring -- so the follow-up addSSVReference
      # below always appends a fresh, non-empty entry rather than colliding).
      try:
        ssmResourceName = f'resources/{Path(newSsmPath).name}'
        if ssmResourceName not in self._ssp.resources:
          self._ssp.addResource(newSsmPath, ssmResourceName)
        self._ssp.removeSSVReference(CRef(*path), ssvResource)
        self._ssp.addSSVReference(CRef(*path), ssvResource, ssmResourceName)
      except Exception as e:
        QMessageBox.critical(self, 'Add Parameter Mapping failed', str(e))
        return
    elif existingSsmResource:
      # Picked an already-registered .ssm resource instead -- already in the
      # pool, so no addResource call needed, just re-point the reference.
      try:
        self._ssp.removeSSVReference(CRef(*path), ssvResource)
        self._ssp.addSSVReference(CRef(*path), ssvResource, existingSsmResource)
      except Exception as e:
        QMessageBox.critical(self, 'Add Parameter Mapping failed', str(e))
        return

    self._onModelChanged()

  def _onRemoveParameterFileRequested(self, node) -> None:
    if not self._activateModelForNode(node):
      return
    ssvResource, _ssmResource = node.obj
    if QMessageBox.question(self, 'Remove Parameter File', f'Remove "{node.label}"?') != QMessageBox.StandardButton.Yes:
      return
    try:
      # node.parent is the "Parameter Files" group node; its parent is the
      # owning System/Component -- same ancestor-skip _crefPath already does
      # for KIND_CONNECTOR via its "Connectors" group parent.
      self._ssp.removeSSVReference(CRef(*self._crefPath(node.parent.parent)), ssvResource)
    except Exception as e:
      QMessageBox.critical(self, 'Remove Parameter File failed', str(e))
      return
    self._onModelChanged()

  def _onAddResourceRequested(self, node) -> None:
    '''Registers an arbitrary file in the SSP's shared resource pool
    (SSP.addResource, no cref -- unlike every other add-flow in this file,
    a resource isn't attached to anything by this action alone). Useful for
    resource types with no dedicated attach-flow yet (a .csv/.mat lookup
    table for a ComponentTable, a .dcp) or for staging a file before wiring
    it up elsewhere.'''
    if not self._activateModelForNode(node):
      return
    path, _ = QFileDialog.getOpenFileName(self, 'Add Resource', '')
    if not path:
      return
    try:
      self._ssp.addResource(path)
    except Exception as e:
      QMessageBox.critical(self, 'Add Resource failed', str(e))
      return
    self._onModelChanged()

  def _onRemoveResourceRequested(self, node) -> None:
    if not self._activateModelForNode(node):
      return
    if QMessageBox.question(self, 'Remove Resource', f'Remove "{node.label}"?') != QMessageBox.StandardButton.Yes:
      return
    try:
      self._ssp.deleteResource(node.obj)
    except Exception as e:
      QMessageBox.critical(self, 'Remove Resource failed', str(e))
      return
    self._onModelChanged()

  def _onDeleteRequested(self, node) -> None:
    if not self._activateModelForNode(node):
      return
    if node.kind == KIND_MODEL or (node.kind == KIND_SYSTEM and self._treeModel.isTopLevelSystem(node)):
      # Neither the model's own row nor its root System has a cref
      # SSP.delete() could act on (there's no cref for "the whole SSP" or
      # for a system's own root) -- "Delete" here means closing the model
      # entirely instead, the same result either row would give since
      # nothing else exists at that level once the root system is gone.
      self._closeModel(self._activeModel)
      return
    self._deleteCref(self._crefPath(node), node.label)

  def _closeModel(self, model: '_OpenModel') -> None:
    '''Removes an open model from the GUI entirely -- a tab-close, not a
    structural edit. Picks whatever open model happens to be next as the
    new active one (arbitrary order, same as dict iteration order elsewhere
    in this class); None if this was the last one, which every _activeModel
    accessor already handles (the same state the app starts in before
    anything is opened).'''
    if QMessageBox.question(
        self, 'Close Model', f'Close model "{model.name}"? Any changes not saved to disk will be lost.'
    ) != QMessageBox.StandardButton.Yes:
      return
    del self._models[model.name]
    if self._activeModel is model:
      self._activeModel = next(iter(self._models.values()), None)
    self._refreshTree()
    self._updateDiagram()

  def _deleteCref(self, path: list[str], displayName: str) -> None:
    '''Shared by tree-driven deletes (Delete key or context menu, via
    _onDeleteRequested) and canvas-driven ones (_onCanvasElementDeleteRequested/
    _onCanvasConnectorDeleteRequested) -- both just need a cref path and a
    name to show in the confirmation prompt.'''
    if QMessageBox.question(self, 'Delete', f'Delete "{displayName}"?') != QMessageBox.StandardButton.Yes:
      return
    try:
      self._ssp.delete(CRef(*path))
    except Exception as e:
      QMessageBox.critical(self, 'Delete failed', str(e))
      return
    self._onModelChanged()

  def _onCanvasElementDeleteRequested(self, elementName: str) -> None:
    path = self._diagramLevelPath()
    if not path:
      return  # the model level's own root box isn't a deletable element
    self._deleteCref([*path, elementName], elementName)

  def _onCanvasConnectorDeleteRequested(self, connectorName: str) -> None:
    path = self._diagramLevelPath()
    if not path:
      return
    self._deleteCref([*path, connectorName], connectorName)

  def _onCanvasConnectorValueRequested(self, connectorName: str) -> None:
    path = self._diagramLevelPath()
    if not path:
      return  # the model level's own root box has no addressable connectors
    system = self._diagramStack[-1][0]
    connector = next((c for c in system.connectors if str(c.name) == connectorName), None)
    if connector is None:
      return
    cref = CRef(*path, connectorName)
    try:
      currentValue = self._ssp.getValue(cref)
    except Exception:
      currentValue = None

    dialog = ConnectorValueDialog(connector, currentValue, self)
    if dialog.exec() != QDialog.DialogCode.Accepted:
      return
    newValue = dialog.value()
    if newValue is None or newValue == currentValue:
      return  # untouched, left blank (backend already defaults to 0), or not editable
    try:
      self._ssp.setValue(cref, newValue)
    except Exception as e:
      QMessageBox.critical(self, 'Set Value failed', str(e))

  def _onRenameRequested(self, node) -> None:
    if not self._activateModelForNode(node):
      return
    if node.kind == KIND_MODEL:
      self._renameActiveModel()
      return
    currentName = str(node.obj.name)
    newName, ok = QInputDialog.getText(self, 'Rename', 'New name:', text=currentName)
    newName = newName.strip()
    if not ok or not newName or newName == currentName:
      return
    try:
      self._ssp.rename(CRef(*self._crefPath(node)), CRef(newName))
    except Exception as e:
      QMessageBox.critical(self, 'Rename failed', str(e))
      return
    self._onModelChanged()

  def _renameActiveModel(self) -> None:
    '''Renames the open model itself (its tree row, e.g. "test") -- unlike
    renaming a System/Component inside it, there's no cref for "the whole
    SSP" to call SSP.rename() with. The model row's own name always tracks
    its active variant's own name elsewhere (on open, and after switching
    variants -- see _onVariantsTriggered), so renaming it here means
    renaming that SSD directly, the same re-keying dance _onNewTriggered
    does at creation time (SSD.name has no owning-SSP awareness on its
    own: a plain assignment would leave ssp.variants' dict key and
    ssp.activeVariantName pointing at the old name).'''
    model = self._activeModel
    ssd = model.ssp.activeVariant if model.ssp is not None else None
    if ssd is None:
      return
    newName, ok = QInputDialog.getText(self, 'Rename Model', 'New name:', text=model.name)
    newName = newName.strip()
    if not ok or not newName or newName == model.name:
      return

    oldVariantName = ssd.name
    ssd.name = newName
    del model.ssp.variants[oldVariantName]
    model.ssp.variants[ssd.name] = ssd
    model.ssp.activeVariantName = ssd.name

    self._renameModel(model, newName)
    self._rebuildDiagramWrapper(model)
    self._refreshTree()
    self._updateDiagram()

  def _onPropertiesRequested(self, node) -> None:
    # Not strictly needed for correctness (the edit below operates directly
    # on node.obj, not through self._ssp), but keeps the diagram canvas
    # pointed at the model actually being edited afterward.
    self._activateModelForNode(node)
    self._showElementProperties(node.obj)

  def _onCanvasPropertiesRequested(self, component) -> None:
    self._showElementProperties(component)

  def _showElementProperties(self, component) -> None:
    dialog = ElementPropertiesDialog(component, self)
    if dialog.exec() == QDialog.DialogCode.Accepted:
      self._onModelChanged()

  # --- Simulation settings -----------------------------------------------------

  def _onSimulationSettingsTriggered(self) -> None:
    if self._ssp is None:
      QMessageBox.information(self, 'No model', 'Create or open a model first.')
      return
    ssd = self._ssp.activeVariant
    dialog = SimulationSettingsDialog(ssd, self)
    if dialog.exec() == QDialog.DialogCode.Accepted:
      # The dialog applies its own fields plus the solver configs/component
      # assignments directly to ssd on accept (see its own docstring for why
      # it deviates from the collect-only Add* dialog pattern); nothing left
      # to apply here.
      self._onModelChanged()

  # --- Options (Tools menu) -----------------------------------------------

  def _onOptionsTriggered(self) -> None:
    '''Global, app-wide preferences (persisted via QSettings, not per-model
    or per-.ssp) -- mirrors OMEdit's own Tools > Options > OMSimulator/SSP
    page. Read fresh here rather than cached on self, since QSettings is
    already the single source of truth and nothing else in this class needs
    to react to a change until the next simulation run picks it up.'''
    settings = QSettings(_SETTINGS_ORG, _SETTINGS_APP)
    dialog = OptionsDialog(
        str(settings.value(_SETTINGS_KEY_WORKING_DIR, '')),
        str(settings.value(_SETTINGS_KEY_CLI_OPTIONS, '')),
        self)
    if dialog.exec() != QDialog.DialogCode.Accepted:
      return
    settings.setValue(_SETTINGS_KEY_WORKING_DIR, dialog.workingDirectory())
    settings.setValue(_SETTINGS_KEY_CLI_OPTIONS, dialog.commandLineOptions())

  # --- Variants -----------------------------------------------------------

  def _onVariantsTriggered(self) -> None:
    if self._ssp is None:
      QMessageBox.information(self, 'No model', 'Create or open a model first.')
      return
    model = self._activeModel
    dialog = VariantsDialog(self._ssp, self)
    dialog.exec()
    # VariantsDialog mutates self._ssp directly and immediately (see its own
    # docstring) -- activating a different variant may have swapped out the
    # root System object the tree and diagram wrapper/stack were built from,
    # and the model's own tree-row name should track whichever variant is
    # now active (same as when the model was first opened -- see _addModel).
    # _onModelChanged()'s tree refresh replays the *same* System objects the
    # tree was last built from (correct for in-place edits, e.g. adding a
    # component), which is exactly wrong here -- go through _refreshTree()
    # instead, which re-reads model.ssp.activeVariant.system fresh, same as
    # when a model is first opened.
    activeVariant = model.ssp.activeVariant
    if activeVariant is not None:
      self._renameModel(model, activeVariant.name)
    self._rebuildDiagramWrapper(model)
    self._refreshTree()
    self._updateDiagram()

  # --- Simulation (M6) --------------------------------------------------------

  def _onSimulateTriggered(self) -> None:
    if self._ssp is None:
      QMessageBox.information(self, 'No model', 'Create or open a model first.')
      return
    if self._simulationClient is not None and self._simulationClient.isRunning():
      QMessageBox.information(self, 'Simulation running',
                               'A simulation is already running. Stop it before starting another.')
      return

    # The subprocess gets its own SSP instance loaded from a file (crash
    # isolation means it can't just share the in-memory object we're
    # editing) -- export the *current* state of the active variant to a
    # scratch directory. Working directory is set to the same directory so
    # the model's own (usually relative) resultFile lands somewhere we know
    # to look for it afterward. Tools > Options can override the default
    # fresh-temp-dir-per-run behavior with a fixed directory of the user's
    # choosing (results then land in a known place and get overwritten on
    # each re-run, rather than scattered across a new temp dir every time).
    settings = QSettings(_SETTINGS_ORG, _SETTINGS_APP)
    configuredWorkingDir = str(settings.value(_SETTINGS_KEY_WORKING_DIR, '')).strip()
    if configuredWorkingDir:
      tempDir = configuredWorkingDir
      try:
        os.makedirs(tempDir, exist_ok=True)
      except OSError as e:
        QMessageBox.critical(self, 'Simulation failed',
                              f'Could not create the configured working directory:\n{tempDir}\n{e}')
        return
    else:
      tempDir = tempfile.mkdtemp(prefix='omsimulatorgui_')
    exportPath = str(Path(tempDir) / 'model.ssp')
    try:
      self._ssp.export(exportPath)
    except Exception as e:
      QMessageBox.critical(self, 'Export failed', f'Could not export the model for simulation:\n{e}')
      return

    resultPath = str(Path(tempDir) / self._ssp.activeVariant.resultFile)
    commandLineOptions = str(settings.value(_SETTINGS_KEY_CLI_OPTIONS, '')).strip()

    client = SimulationClient(self)
    try:
      client.start(exportPath, tempDir, commandLineOptions)
    except Exception as e:
      QMessageBox.critical(self, 'Simulation failed', f'Could not start the simulation:\n{e}')
      return

    self._simulationClient = client
    client.finished.connect(lambda success, message: self._onSimulationFinished(success, message, resultPath))
    self._simulationPanel.setClient(client)

  def _onSimulationFinished(self, success: bool, message: str, resultPath: str) -> None:
    self._simulationClient = None
    if not success:
      if message:
        QMessageBox.warning(self, 'Simulation failed', message)
      return
    if not Path(resultPath).exists():
      QMessageBox.warning(self, 'No results',
                           f'Simulation finished but the result file was not found:\n{resultPath}')
      return
    try:
      viewer = ResultsViewer(resultPath, self)
    except Exception as e:
      QMessageBox.critical(self, 'Failed to load results', f'Could not load "{resultPath}":\n{e}')
      return
    self._resultsViewers.append(viewer)
    viewer.show()
