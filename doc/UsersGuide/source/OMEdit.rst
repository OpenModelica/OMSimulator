.. index:: OMEdit
.. _omedit-graphical-modelling :

Graphical Modelling
===================

OMSimulator has an optional dependency to OpenModelica in order to
utilize the graphical modelling editor OMEdit. This feature requires
to install the full OpenModelica tool suite, which includes
OMSimulator. The independent stand-alone version doesn't provide any
graphical modelling editor.

Composite models are imported and exported in the System Structure Description (SSD) format,
which is part of the System Structure and Parameterization (SSP) standard.

See also `FMI documentation <https://openmodelica.org/doc/OpenModelicaUsersGuide/latest/fmitlm.html>`_
and `SSP documentation <https://ssp-standard.org/>`_.

.. figure :: images/omedit_01.png
  :name: omedit-mainwindow-browsers

  OMEdit MainWindow and Browsers.

New SSP Model
---------------------

A new and empty SSP model can be created from `File->New->SSP` menu item.

.. figure :: images/omedit_02.png

  OMEdit: New SSP Model

That will open a dialog to enter the names of the model and the root
system and to choose the root systems type.

There are three types available:
  - TLM - Transmission Line Modeling System
  - Weakly Coupled - Connected Co-Simulation FMUs System
  - Strongly Coupled - Connected Model-Exchange FMUs System

.. figure :: images/omedit_03.png

  OMEdit: New SSP Model Dialog

.. figure :: images/omedit_04.png

  OMEdit: Newly created empty root system of SSP model

Add System
----------

When a new model is created a root system is always generated.
If you need to have another system in your root system you can
add it with `SSP->Add System`.

For example only a weakly coupled system (Co-Simulation) can integrate strongly coupled
system (Model Exchange). Therefore, the weakly coupled system must
be selected from the Libraries Browser and the respective menu item
can be selected:

.. figure :: images/omedit_05.png

  OMEdit: Add System

That will pop-up a dialog to enter the names of the new system.

.. figure :: images/omedit_06.png

  OMEdit: Add System Dialog

Add SubModel
------------

A sub-model is typically an FMU, but it also can be result file. In
order to import a sub-model, the respective system must be selected
and the action can be selected from the menu bar:

.. figure :: images/omedit_07.png

  OMEdit: Add SubModel

The file browser will open to select an FMU (.fmu) or result file
(.csv) as a subsmodel.
Then a dialog opens to choose the name of the new sub-model.

.. figure :: images/omedit_08.png

  OMEdit: Add SubModel Dialog

.. figure :: images/omedit_09.png

  OMEdit: Root system with added FMU.

Simulate
--------

Select the simulate button (symbol with green arrow) or select
`Simulation->Simulate` from the menu in OMEdit to simulate the
SSP model.

Dual Mass Oscillator Example
----------------------------

The dual mass oscillator example from our testsuite is
a simple example one can recreate using components from the
Modelica Standard Library.
After splitting the model into two models and exporting each
as an Model-Exchange and Co-Simulation FMU.

.. figure :: images/DualMassOscillator.png

  Dual mass oscillator Modelica model (diagramm view) and FMUs


.. figure :: images/omedit_10.png

  OMEdit: Simulate Dual Mass Oscillator SSP model
