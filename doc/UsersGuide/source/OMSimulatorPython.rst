.. index:: OMSimulatorPython

OMSimulatorPython
=================

This is a shared library that provides a Python interface for the OMSimulatorLib library.

.. index:: OMSimulatorPython; Examples

Examples
########

.. code-block:: python

  from OMSimulator import OMSimulator
  session = OMSimulator()

  session.newFMIModel("model")

  # instantiate FMUs
  session.addFMU("model", "FMUs/submodelA.fmu", "A")
  session.addFMU("model", "FMUs/submodelB.fmu", "B")

  # add connections
  session.addConnection("model", "A:in1", "B:out1")
  session.addConnection("model", "A:in2", "B:out2")
  session.addConnection("model", "A:out1", "B:in1")
  session.addConnection("model", "A:out2", "B:in2")

  session.setStopTime("model", 2.0)
  session.setCommunicationInterval("model", 1e-5)
  session.setResultFile("model", "AB_res.mat")

  session.initialize("model")
  session.simulate("model")
  session.terminate("model")
  session.unloadModel("model")

.. index:: OMSimulatorPython; Scripting Commands

Python Scripting Commands
#########################

.. include:: OMSimulatorPython.inc
