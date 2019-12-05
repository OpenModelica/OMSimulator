.. index:: OMSimulatorPython

.. _oms-python :

OMSimulatorPython
=================

This is a shared library that provides a Python interface for the
OMSimulatorLib library.

.. index:: OMSimulatorPython; Examples

Examples
########

.. code-block:: python

  from OMSimulator import OMSimulator
  oms.setTempDirectory("./temp/")
  oms.newModel("model")
  oms.addSystem("model.root", oms.system_sc)

  # instantiate FMUs
  oms.addSubModel("model.root.system1", "FMUs/System1.fmu")
  oms.addSubModel("model.root.system2", "FMUs/System2.fmu")

  # add connections
  oms.addConnection("model.root.system1.y", "model.root.system2.u")
  oms.addConnection("model.root.system2.y", "model.root.system1.u")

  # simulation settings
  oms.setResultFile("model", "results.mat")
  oms.setStopTime("model", 0.1)
  oms.setFixedStepSize("model.root", 1e-4)

  oms.instantiate("model")
  oms.setReal("model.root.system1.x_start", 2.5)

  oms.initialize("model")
  oms.simulate("model")
  oms.terminate("model")
  oms.delete("model")

.. index:: OMSimulatorPython; Scripting Commands

Python Scripting Commands
#########################

.. include:: OMSimulatorPython.inc
