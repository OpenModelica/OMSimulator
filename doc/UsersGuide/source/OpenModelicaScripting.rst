.. index:: OpenModelicaScripting

OpenModelicaScripting
=====================

This is a shared library that provides a OpenModelica Scripting interface for the
OMSimulatorLib library.

.. index:: OpenModelicaScripting; Examples

Examples
--------

.. code-block:: modelica

  loadOMSimulator();
  oms_setTempDirectory("./temp/");
  oms_newModel("model");
  oms_addSystem("model.root", OpenModelica.Scripting.oms_system.oms_system_sc);

  // instantiate FMUs
  oms_addSubModel("model.root.system1", "FMUs/System1.fmu");
  oms_addSubModel("model.root.system2", "FMUs/System2.fmu");

  // add connections
  oms_addConnection("model.root.system1.y", "model.root.system2.u");
  oms_addConnection("model.root.system2.y", "model.root.system1.u");

  // simulation settings
  oms_setResultFile("model", "results.mat");
  oms_setStopTime("model", 0.1);
  oms_setFixedStepSize("model.root", 1e-4);

  oms_instantiate("model");
  oms_setReal("model.root.system1.x_start", 2.5);

  oms_initialize("model");
  oms_simulate("model");
  oms_terminate("model");
  oms_delete("model");
  unloadOMSimulator();

.. index:: OpenModelicaScripting; Scripting Commands

OpenModelica Scripting Commands
-------------------------------

.. include:: OMCScripting.inc
