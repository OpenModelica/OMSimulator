.. index:: OMSimulatorLua

OMSimulatorLua
=================

This is a shared library that provides a Lua interface for the OMSimulatorLib
library.

.. index:: OMSimulatorLua; Examples

Examples
########

.. code-block:: lua

  oms3_setTempDirectory("./temp/")
  oms3_newModel("model")
  oms3_addSystem("model.root", oms_system_sc)

  -- instantiate FMUs
  oms3_addSubModel("model.root.system1", "FMUs/System1.fmu")
  oms3_addSubModel("model.root.system2", "FMUs/System2.fmu")

  -- add connections
  oms3_addConnection("model.root.system1.y", "model.root.system2.u")
  oms3_addConnection("model.root.system2.y", "model.root.system1.u")

  -- simulation settings
  oms3_setResultFile("model", "results.mat")
  oms3_setStopTime("model", 0.1)
  oms3_setFixedStepSize("model.root", 1e-4)

  oms3_instantiate("model")
  oms3_setReal("model.root.system1.x_start", 2.5)

  oms3_initialize("model")
  oms3_simulate("model")
  oms3_terminate("model")
  oms3_delete("model")

.. index:: OMSimulatorLua; Scripting Commands

Lua Scripting Commands
######################

.. include:: OMSimulatorLua.inc
