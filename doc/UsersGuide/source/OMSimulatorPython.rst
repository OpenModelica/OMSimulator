.. index:: OMSimulatorPython

OMSimulatorPython
=================

This is a shared library that provides a Python interface for the OMSimulatorLib library.

.. index:: OMSimulatorPython; Examples

Examples
########

.. code-block:: python

  from OMSimulator import OMSimulator
  oms = OMSimulator()

  oms.newFMIModel("model")

  # instantiate FMUs
  oms.addFMU("model", "FMUs/submodelA.fmu", "A")
  oms.addFMU("model", "FMUs/submodelB.fmu", "B")

  # add connections
  oms.addConnection("model", "A:in1", "B:out1")
  oms.addConnection("model", "A:in2", "B:out2")
  oms.addConnection("model", "A:out1", "B:in1")
  oms.addConnection("model", "A:out2", "B:in2")

  oms.setStopTime("model", 2.0)
  oms.setCommunicationInterval("model", 1e-5)
  oms.setResultFile("model", "AB_res.mat")

  oms.initialize("model")
  oms.simulate("model")
  oms.unloadModel("model")

.. index:: OMSimulatorPython; Scripting Commands

Python Scripting Commands
#########################

.. include:: OMSimulatorPython.inc
