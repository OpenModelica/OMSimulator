.. index:: OMSimulatorLua

OMSimulatorLua
=================

This is a shared library that provides a Lua interface for the OMSimulatorLib library.

.. index:: OMSimulatorLua; Examples

Examples
########

.. code-block:: lua

  oms2_setTempDirectory("./temp/")
  oms2_newFMIModel("model")

  -- instantiate FMUs
  oms2_addFMU("model", "FMUs/submodelA.fmu", "A")
  oms2_addFMU("model", "FMUs/submodelB.fmu", "B")

  -- add connections
  oms2_addConnection("model", "A:in1", "B:out1")
  oms2_addConnection("model", "A:in2", "B:out2")
  oms2_addConnection("model", "A:out1", "B:in1")
  oms2_addConnection("model", "A:out2", "B:in2")

  oms2_describe("model")

  oms2_setStopTime("model", 2.0)
  oms2_setResultFile("model", "results.mat")

  oms2_initialize("model")
  oms2_simulate("model")

  oms2_unloadModel("model")

.. index:: OMSimulatorLua; Scripting Commands

Lua Scripting Commands
######################

.. include:: OMSimulatorLua.inc
