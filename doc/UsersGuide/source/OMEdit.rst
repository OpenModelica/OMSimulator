.. index:: OMEdit
.. _omedit-graphical-modelling :

Graphical Modelling
===================

OMSimulator provides a graphical modelling environment through OMEdit, the
OpenModelica Connection Editor. This feature requires a full OpenModelica
installation that includes OMSimulator.

Composite models are imported and exported in the System Structure Description
(SSD) format, which is part of the System Structure and Parameterization (SSP)
standard.

See also `FMI documentation <https://openmodelica.org/doc/OpenModelicaUsersGuide/latest/fmitlm.html>`_
and `SSP documentation <https://ssp-standard.org/>`_.

Architecture
------------

OMEdit communicates with OMSimulator through a Python-based ZMQ server:

- **OMSimulatorGuiServer.py** — handles all GUI-driven requests (model
  management, element queries, solver settings, connections, etc.)
- **OMSimulatorSimulationServer.py** — handles simulation execution and
  result streaming

The GUI server is started automatically when the first SSP model is opened
or created. A notification in the Messages Browser confirms the server is
running, including the script path and ZMQ endpoint.

New SSP Model
-------------

A new and empty SSP model can be created from ``File -> New -> SSP`` menu item.

.. figure :: images/omedit_02.png

  OMEdit: New SSP Model

A dialog opens to enter the model name and the name of the root system.

.. figure :: images/omedit_03.png

  OMEdit: New SSP Model Dialog

.. figure :: images/omedit_01.png

  OMEdit: Newly created empty root system of SSP model

Open SSP Model
--------------

An existing SSP file (``.ssp``) can be opened from ``File -> Open Model/Library``.
If a model with the same name is already loaded, an error is reported and the
file is not loaded again.

Add System and Sub-Systems
--------------------------

A root system is always created together with the model. Additional subsystems
can be added inside the root system via ``SSP -> Add System``.

.. figure :: images/omedit_05.png

  OMEdit: Add System

A dialog opens to enter the name of the new system.

.. figure :: images/omedit_06.png

  OMEdit: Add System Dialog


Add SubModel
------------

A sub-model is typically an FMU, but it can also be a result file (CSV). To
add a sub-model, select the target system in the Libraries Browser and choose
``SSP -> Add SubModel``.

.. figure :: images/omedit_07.png

  OMEdit: Add SubModel

A file browser opens to select an FMU (``.fmu``) or result file (``.csv``).
A dialog then opens to set the name for the new sub-model.

.. figure :: images/omedit_08.png

  OMEdit: Add SubModel Dialog

.. figure :: images/omedit_09.png

  OMEdit: Root system with added FMU.

Replace SubModel
----------------

An existing sub-model can be replaced with a different FMU by right-clicking
on the component in the diagram view and selecting ``Replace SubModel`` from
the context menu.

.. figure :: images/omedit_replace_submodel.png

  OMEdit: Replace SubModel Dialog.

A file browser opens to select the new FMU. The replacement is performed in
two steps:

1. **Dry run** (``dryRun=true``) — OMEdit first performs a dry run without
   making any changes to the model. It checks for interface differences between
   the old and new FMU, such as added, removed, or modified connectors and
   parameters. If any changes are detected, they are reported in the Messages
   Browser and a warning dialog is shown. The model remains unchanged at this
   stage, allowing the user to review the impact before deciding to proceed.

2. **Replacement** (``dryRun=false``) — If the user confirms, the replacement
   is carried out and all detected interface changes are applied to the model.
   Existing connections to removed or renamed connectors are dropped, and new
   connectors are available for connecting.

.. figure :: images/omedit_replace_submodel_contextmenu.png

  OMEdit: Replace SubModel context menu.



Simulation Setup
----------------

Select ``Simulation -> Simulation Setup`` to configure simulation parameters
before running. The dialog has two tabs: **General** and **Solver Settings**.

The **General** tab covers start time, stop time, and result file settings.

.. figure :: images/omedit_simulation_setup.png

  OMEdit: Simulation Setup — Solver Settings tab with a solver configuration.

The **Solver Settings** tab is divided into two parts:

**Solver Configurations**

This table lists the named solver configurations available in the model.
Each configuration has a name and a method. Click **Add** to create a new
solver configuration — a new row appears in the table with a default method
of ``oms-ma``. The method can be changed using the combo box in the Method
column.

.. figure :: images/omedit_simulation_setup_solver_settings.png

  OMEdit: Simulation Setup — Solver Settings tab with a solver configuration.

To fine-tune a solver's numerical parameters, select the row and click
**Edit**. This opens the Solver Settings dialog:

.. figure :: images/omedit_simulation_setup_solver_edit.png

  OMEdit: Solver Settings dialog.

The available fields depend on the solver method:

- **Fixed-step solvers** (``oms-ma``, ``euler``) — Fixed Step Size is editable;
  Initial Step Size, Minimum Step Size, and Maximum Step Size are disabled.
- **Variable-step solvers** (``oms-mav``, ``oms-mav-2``, ``cvode``) — Initial
  Step Size, Minimum Step Size, Maximum Step Size, and Relative Tolerance are
  editable; Fixed Step Size is disabled.

Click **Remove** to delete the selected solver configuration.

**Component Assignments**

This table lists all FMU components in the model. Each component can be
assigned one of the named solver configurations using the combo box in the
Solver column. The available solvers are filtered by the FMI kind of each
component:

- **Co-Simulation FMUs** — only co-simulation masters are selectable
  (``oms-ma``, ``oms-mav``, ``oms-mav-2``).
- **Model-Exchange FMUs** — only ODE integrators are selectable
  (``cvode``, ``euler``).
- **me+cs FMUs** — all solvers are available.


.. figure :: images/omedit_component_assignment_solver.png

  OMEdit: Solver Assignments in Components

Incompatible solvers are shown as disabled in the combo box with a tooltip
explaining why. If no solver is assigned, ``(none)`` is used and the
system default applies.

Simulate
--------

Select the simulate button (green arrow) or ``Simulation -> Simulate`` to
run the SSP model. Results are streamed back in real time via the simulation
server.


Dual Mass Oscillator Example
----------------------------

The dual mass oscillator example from the test suite demonstrates a typical
SSP workflow. The model is split into two sub-models, each exported as an FMU,
and then connected in an SSP.

.. figure :: images/DualMassOscillator.png

  Dual mass oscillator Modelica model (diagram view) and FMUs

.. figure :: images/omedit_10.png

  OMEdit: Simulate Dual Mass Oscillator SSP model
