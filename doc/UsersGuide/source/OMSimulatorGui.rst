.. index:: OMSimulatorGui
.. _omsimulatorgui-standalone :

OMSimulatorGui
==============

OMSimulatorGui is a lightweight, standalone graphical editor for SSP models,
built directly on top of the OMSimulator Python API
(:ref:`OMSimulatorPython3 <oms3-python>`). Unlike :ref:`OMEdit
<omedit-graphical-modelling>`, it has no dependency on omc or the rest of the
OpenModelica compiler -- it only needs a plain OMSimulator installation with
a couple of Python GUI packages on top, and it calls the same in-process
Python API used by scripts, with no server process or ZMQ layer in between
for editing.

It covers the everyday SSP editing workflow: building up a system structure
with subsystems, FMU components and connectors, wiring them together on a
diagram canvas, attaching parameter values and mappings (SSV/SSM),
configuring solvers, and running a simulation with live progress and a
result plot. It does not (yet) include a read-only XML/text viewer for the
underlying SSD/SSV/SSM files -- see :ref:`omsimulatorgui-limitations` below.

.. index:: OMSimulatorGui; Requirements

Requirements and Enabling the Build
------------------------------------

OMSimulatorGui is built by default. To disable it (e.g. to skip its extra
Python package requirements below), configure with:

.. code-block:: bash

   cmake -S . -B build -DOMS_ENABLE_OMSimulatorGui=OFF
   cmake --build build --target install

It has no compiled component -- it is pure Python, installed alongside the
``OMSimulator`` Python package -- but it does need a couple of extra Python
packages that the core OMSimulator CLI/API does not require:

- `PySide6 <https://pypi.org/project/PySide6/>`_ -- the Qt widgets, the
  system tree, and the diagram canvas.
- `pyqtgraph <https://pypi.org/project/pyqtgraph/>`_ -- the result plot.
- `scipy <https://pypi.org/project/scipy/>`_ -- reading ``.mat`` result
  files.

Install them into whichever Python environment OMSimulator itself runs
under:

.. code-block:: bash

   pip install -r src/OMSimulatorGui/requirements.txt

.. index:: OMSimulatorGui; Launching

Launching
---------

A build with OMSimulatorGui enabled installs an
``OMSimulatorGui`` launcher script next to ``OMSimulator`` itself:

.. code-block:: bash

   OMSimulatorGui

This sets up ``PYTHONPATH``/``PATH`` the same way ``OMSimulator.bat``/``.sh``
does for the CLI, then runs ``python3 -m OMSimulatorGui``. It can also be
invoked directly once the ``OMSimulator``/``OMSimulatorGui`` packages are
importable:

.. code-block:: bash

   python -m OMSimulatorGui

.. index:: OMSimulatorGui; Main Window

Main Window
-----------

.. figure :: images/omsimulatorguipy_01_overview.png

  OMSimulatorGui: main window with a model open, showing the system tree
  (left), the diagram canvas drilled into the root system "Root" (right),
  and the breadcrumb "test > Root" in the toolbar.

The main window has three parts:

- A **System Tree** on the left, showing every open model as its own
  top-level row, each with its root system, connectors, connections,
  parameter files and registered resources underneath.
- A **Diagram Canvas** on the right, showing one system's contents at a
  time as boxes and connection lines -- systems, components (FMUs) and
  connectors on its boundary.
- A **toolbar** above the canvas with an **Up** button (leave the current
  system and go back to its parent), a **Simulation Settings** button, a
  **Simulate** button (green arrow), and a breadcrumb showing the current
  position in the system hierarchy.

Three menus tie everything together:

- **File** -- New SSP Model... (``Ctrl+N``), Open... (``Ctrl+O``), Save
  As... (``Ctrl+Shift+S``), Exit (``Ctrl+Q``).
- **Model** -- Simulation Settings..., Variants..., Simulate (``Ctrl+R``) --
  all act on whichever model is currently active.
- **Tools** -- Options... (see :ref:`omsimulatorgui-options` below).
- **Help** -- About OMSimulatorGui..., showing the running OMSimulator
  version (the same one ``OMSimulator --version`` reports).

The tree and the canvas always show the same underlying model: editing on
one immediately refreshes the other.

.. index:: OMSimulatorGui; Creating and Opening Models

Creating and Opening Models
----------------------------

**File -> New SSP Model...** opens a dialog asking for the model's name and
the name of its root system, then creates a new, empty SSP model and opens
it as a new tab in the tree.

.. figure :: images/omsimulatorguipy_02_new_model.png

  OMSimulatorGui: New SSP Model dialog, entering the model name and the
  root system's own name.

**File -> Open...** opens a file browser to load an existing ``.ssp`` file.
Several models can be open at the same time; each gets its own top-level row
in the tree, and the diagram canvas and toolbar always reflect whichever
model was last clicked on in the tree.

**File -> Save As...** exports the currently active model to a chosen
``.ssp`` path.

Multiple models can be open side by side. To close one, select its model
row (or its root system row) in the tree and either press ``Delete`` or use
**Delete** from its right-click menu -- both close the whole model, not just
one part of it.

.. index:: OMSimulatorGui; System Tree

The System Tree
----------------

Each open model's row expands into its root system, which in turn shows:

- **Systems and Components** -- nested subsystems, FMU components, and
  result-file lookup tables (all three added via the same **Add
  Component...** action -- see The Diagram Canvas below), each with their
  own Connectors/Connections/Parameter Files/Resources underneath as
  applicable.
- **Connectors** -- the system's own boundary ports.
- **Connections** -- wires between two connectors at this level.
- **Parameter Files** -- SSV/SSM resources attached to this system or
  component (see :ref:`omsimulatorgui-parameter-files` below).
- **Resources** -- every file registered in the SSP's shared resource pool
  (FMUs, SSV/SSM files, tables, ...), shown once under the model's own row.

Right-clicking a row opens a context menu appropriate to that row's kind:

- A **System** row offers Add System..., Add Component..., Add Connector...,
  Add Parameter File..., Export SSV/SSM Template..., Rename..., and Delete.
- A **Component** row offers Properties..., Add Parameter File...,
  Export SSV/SSM Template..., Rename..., and Delete.
- A **Component Table** row (a result-file lookup table, not an FMU)
  offers the same actions minus Properties... -- it has no editable
  metadata of its own.
- A **Connector** row offers Delete.
- A **Parameter File** row offers Edit Values..., Swap Resource..., and
  Remove.
- A **Resource** row offers Remove.
- A model's own top-level row offers Add Resource..., Rename..., and
  Delete (closes the whole model).

.. figure :: images/omsimulatorguipy_11_component_context_menu.png

  OMSimulatorGui: right-click context menu on a Component row ("add") in
  the tree -- Properties..., Add Parameter File..., Export SSV/SSM
  Template..., Rename..., Delete. Right-clicking the same component's box
  on the canvas offers the identical set of actions.

Double-clicking a Component row opens its Properties dialog; double-clicking
a Parameter File row opens Edit Values. Double-clicking a Component Table
row does nothing (there is no Properties dialog for one).

.. index:: OMSimulatorGui; Diagram Canvas
.. _omsimulatorgui-diagram-canvas:

The Diagram Canvas
-------------------

The canvas shows one system's contents at a time. Double-click a subsystem
or component box to drill into it (an FMU component opens its Properties
dialog instead, since it has no contents of its own; a component table box
does nothing on double-click); use the toolbar's **Up** button, or the
breadcrumb, to go back out.

Right-clicking empty canvas space offers **Add System...**, **Add
Component...** and **Add Connector...**, placing the new element at the
click position. Right-clicking an existing box offers the same
system/component actions as the tree (Add Parameter File..., Export
Template..., Rename..., Delete), plus **Properties...** for an FMU
component.

**Add Component...** accepts either an FMU (``.fmu``) or a result-file
lookup table (``.csv``/``.mat``) through the same dialog and file browser --
matching OMEdit's own unified Add SubModel. Which kind is created depends
only on the file's own extension: an FMU becomes a regular Component, while
a ``.csv``/``.mat`` becomes a **Component Table** whose signals are always
read from the file's header and always given ``output`` causality (there is
nothing to configure on one -- no Properties dialog, no double-click
action, matching OMEdit).

.. figure :: images/omsimulatorguipy_15_add_component.png

  OMSimulatorGui: Add Component dialog -- a file (an FMU or a result-file
  table, via Browse...) and a name for the new component.

To connect two ports, drag from one to the other -- the connection is only
created if the two connectors are causality-compatible (e.g. an output to
an input), otherwise nothing happens. A connection defaults to an automatic
routed path but can be reshaped by hand: drag anywhere along it to insert or
move a waypoint, right-click a waypoint to remove it. Moving a box shifts
any connections attached to it by the same amount, so a manually-reshaped
route does not go stale.

.. figure :: images/omsimulatorguipy_17_connection_reshape.png

  OMSimulatorGui: a manually reshaped connection (".input -> add.u1")
  routed below and around both boxes instead of a straight line, with its
  identity shown in a hover tooltip.

Select a box, port, or connection and press ``Delete`` to remove it, or use
Delete from its context menu. Double-clicking a connection itself has no
effect (it does not open or delete anything).

An input or output connector with no incoming connection can be given a
constant value by double-clicking it directly on the canvas -- this opens a
small **Set Value** dialog. Leaving the value field blank clears any set
value (the backend then defaults it to 0); no unit needs to be entered.

.. figure :: images/omsimulatorguipy_12_set_value.png

  OMSimulatorGui: Set Value dialog for the root system's own "input"
  connector.

.. index:: OMSimulatorGui; Element Properties

Element (Component) Properties
-------------------------------

Double-clicking an FMU component (in the tree or on the canvas) opens its
**Properties** dialog, with three tabs. A component table (a ``.csv``/
``.mat`` added via the same Add Component... action) has no such dialog --
double-clicking one does nothing, since it has no metadata or parameters to
edit.

.. figure :: images/omsimulatorguipy_10_element_properties.png

  OMSimulatorGui: Properties dialog for the "Gain" component, Info tab --
  the Parameters and Inputs tabs sit alongside it.

- **Info** -- FMU path, FMI kind (model-exchange/co-simulation/both) and
  other read-only metadata.
- **Parameters** -- editable parameter values (Real/Integer/Boolean; String
  parameters are not yet supported, matching OMEdit).
- **Inputs** -- editable input values, same types as above.

Only values the user actually changes are written back to the model -- an
untouched parameter keeps using whatever default the FMU itself declares.

.. index:: OMSimulatorGui; Parameter Files
.. _omsimulatorgui-parameter-files:

Parameter Files (SSV/SSM)
---------------------------

A **parameter value file** (SSV) holds a named set of parameter/input
values; a **parameter mapping file** (SSM) maps those names onto a
different system or component's own connector names (e.g. renaming or
rescaling a value as it crosses into a subsystem). Both are ordinary SSP
resources that get attached to a system or component -- never to the
"whole model" as a free-floating box, so on the canvas they only ever show
as a small "P" badge on the system/component box they are attached to.

**Add Parameter File...** (on a System or Component row, or its canvas box)
opens a dialog to either:

- create a brand-new SSV file by entering values directly, optionally also
  creating a companion SSM mapping (which can be saved empty, to be filled
  in later), or
- browse to an existing ``.ssv``/``.ssm`` file on disk, or
- pick a resource **already registered** in the SSP (see Resources below)
  instead of creating or browsing a new one.

.. figure :: images/omsimulatorguipy_14_add_parameter_file_create.png

  OMSimulatorGui: Add Parameter File dialog in "Create new file" mode --
  a Name/Value/Type table for the SSV, plus an optional "Also create a
  parameter mapping (.ssm)" table underneath.

.. figure :: images/omsimulatorguipy_04_add_parameter_file.png

  OMSimulatorGui: Add Parameter File dialog in "Use existing file(s)" mode
  -- an SSV file (with an "(Or) Existing resource" combo above it) and an
  optional SSM file, each browsable or pickable from the SSP's own
  registered resources.

**Edit Values...** (double-click a Parameter File row, or its badge on the
canvas) reopens the attached SSV's own values for editing, and can also
attach or repoint the companion SSM the same way Add Parameter File does
(create new / browse / pick an existing resource).

.. figure :: images/omsimulatorguipy_03_parameter_files_tree.png

  OMSimulatorGui: tree with each system/component's own "Parameter Files"
  group expanded, the "P" badge on the "Gain" box on the canvas, and the
  Edit Parameter Values dialog reopened on it -- with the "Add a parameter
  mapping (.ssm)" checkbox and Source picker visible underneath the values
  table.

**Swap Resource...** re-points an already-attached SSV reference at a
*different, already-registered* SSV resource, without touching its values
-- useful for switching between two alternative presets that were both
added via Add Resource beforehand. Swapping drops any attached SSM mapping
(a warning is shown first if one is present), since the mapping is
specific to the file being swapped out.

.. figure :: images/omsimulatorguipy_13_swap_resource.png

  OMSimulatorGui: Swap Resource picker, replacing "add.ssv" with another
  already-registered ``.ssv`` resource.

**Export SSV Template.../Export SSM Template...** (on a System or Component
row) writes a ready-made external file listing every parameter/connector
target reachable under that row -- an SSV template with each target's
*current* value, or an SSM template with every target listed with an empty
``source=""`` for hand-editing. Neither of these attaches anything to the
model; they are a scaffold to hand-edit externally and bring back in later
via Add Parameter File's "pick an existing resource" option.

.. figure :: images/omsimulatorguipy_18_export_template.png

  OMSimulatorGui: Export SSV Template.../Export SSM Template... on a
  Component row's context menu. Picking either one opens a standard Save
  File dialog, pre-filled with a default name.

.. index:: OMSimulatorGui; Resources

Resources
---------

Every FMU, SSV/SSM file, or other resource referenced anywhere in the model
is registered once in the SSP's shared resource pool and listed under the
model's own **Resources** group in the tree.

.. figure :: images/omsimulatorguipy_16_resources.png

  OMSimulatorGui: model row's right-click menu (Add Resource.../Rename.../
  Delete) and the Resources group listing every file registered in the SSP.

**Add Resource...** (on the
model's own row) registers an arbitrary file without attaching it to
anything -- useful for staging a file (e.g. a lookup table for a component
table) before wiring it up elsewhere, or for building up a pool of SSV/SSM
presets ahead of time so Add Parameter File's/Swap Resource's "existing
resource" pickers have something to offer. **Remove** unregisters a
resource; it fails with an in-place error if something in the model still
references it.

.. index:: OMSimulatorGui; Variants

Variants
--------

**Model -> Variants...** opens the SSP Variants dialog, listing every
variant (alternate top-level system structure) stored in the model,
letting the user activate a different one or duplicate the active one under
a new name.

.. figure :: images/omsimulatorguipy_09_variants.png

  OMSimulatorGui: SSP Variants dialog for a model with four variants --
  the active one marked "(active)", with Activate and Duplicate... actions.

Activating a variant swaps the root system the tree and canvas
are built from -- the model's own tree row always tracks whichever
variant is currently active by name.

.. index:: OMSimulatorGui; Simulation Settings

Simulation Settings
--------------------

**Model -> Simulation Settings...** (or the toolbar's "S" button) opens a
per-model dialog with two tabs:

.. figure :: images/omsimulatorguipy_06_simulation_settings.png

  OMSimulatorGui: Simulation Setup dialog, General tab.

- **General** -- start time, stop time, tolerance, (maximum) step size,
  result file name, result file buffer size, and logging interval.
- **Solver Settings** -- named solver configurations (a name plus a
  method, e.g. ``oms-ma``, ``oms-mav``, ``cvode``, ``euler``) and a
  per-component assignment table listing every FMU component in the model.
  A component left unassigned falls back to the automatic default for its
  FMI kind.

.. figure :: images/omsimulatorguipy_06b_solver_settings.png

  OMSimulatorGui: Simulation Setup dialog, Solver Settings tab -- two named
  solver configurations, each FMU component assigned to one -- with the
  Solver Configuration dialog open on top for editing a configuration's own
  numerical parameters (step sizes, relative tolerance).

.. index:: OMSimulatorGui; Options
.. _omsimulatorgui-options:

Options (Working Directory and Command Line Options)
-------------------------------------------------------

**Tools -> Options...** holds two global preferences, applied to every
simulation run regardless of which model is open:

.. figure :: images/omsimulatorguipy_05_options.png

  OMSimulatorGui: Tools -> Options dialog, with a Working Directory set.

- **Working Directory** -- when set, every simulation exports the model
  into this fixed folder and runs there, so the result file lands
  somewhere predictable and gets overwritten on each re-run. Left blank
  (the default), each run instead uses a fresh temporary directory, same
  as before this setting existed.
- **Command Line Options** -- free text, forwarded as one or more native
  flags before the model is instantiated -- the same flags and the same
  underlying mechanism :ref:`OMSimulator's own CLI
  <omsimulator-flags>` uses (e.g. ``--suppressPath=true``,
  ``--logLevel=1``). An unrecognized flag makes the simulation fail
  immediately with the native flag parser's own error message.

.. index:: OMSimulatorGui; Simulate

Simulating and Viewing Results
--------------------------------

**Simulate** (the toolbar's green arrow, ``Ctrl+R``, or Model -> Simulate)
exports the active model's current state and runs it in a separate
subprocess -- crash isolation from a misbehaving FMU never takes the GUI
down with it. A progress panel appears at the bottom of the window with a
progress bar and **Pause**, **Stop** and **Hide** buttons; pausing and
resuming take effect immediately.

.. figure :: images/omsimulatorguipy_07_simulation_progress.png

  OMSimulatorGui: simulation progress panel after a run has finished --
  progress at 100%, the solver's own log output above it, and Pause/Stop
  disabled now that there is nothing left to control.

When the simulation finishes, its result file (``.mat`` or ``.csv``) opens
automatically in a **Results** window: a checkable list of every signal on
the left, plotted on the right as they are checked.

.. figure :: images/omsimulatorguipy_08_results_viewer.png

  OMSimulatorGui: Results window for a bouncing ball model -- the "h"
  (height) signal checked in the tree on the left, plotted on the right.

.. _omsimulatorgui-limitations:

Current Limitations
--------------------

- No read-only XML/text viewer for the underlying SSD/SSV/SSM files yet
  (planned as a low-risk, self-contained addition).
- String-valued parameters/inputs are not editable (matches OMEdit's own
  current limitation).
- Only one variant's system structure is shown/edited at a time; switching
  variants resets the diagram canvas back to the model level.
