.. index:: OMSimulatorPython

.. _oms-python :

OMSimulatorPython
=================

This is a shared library that provides a Python interface for the
OMSimulatorLib library.

Installation using ``pip`` is recommended:

.. code-block::  shell-session

  > pip3 install OMSimulator --upgrade

.. index:: OMSimulatorPython; Examples

Examples
########

.. code-block:: python

  from OMSimulator import OMSimulator

  oms = OMSimulator()
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

The python package also provides a more object oriented API. The
following example is equivalent to the previous one:

.. code-block:: python

  import OMSimulator as oms

  oms.setTempDirectory('./temp/')
  model = oms.newModel("model")
  root = model.addSystem('root', oms.Types.System.SC)

  # instantiate FMUs
  root.addSubModel('system1', 'FMUs/System1.fmu')
  root.addSubModel('system2', 'FMUs/System2.fmu')

  # add connections
  root.addConnection('system1.y', 'system2.u')
  root.addConnection('system2.y', 'system1.u')

  # simulation settings
  model.resultFile = 'results.mat'
  model.stopTime = 0.1
  model.fixedStepSize = 1e-4

  model.instantiate()
  model.setReal('root.system1.x_start', 2.5)
  #or system.setReal('system1.x_start', 2.5)

  model.initialize()
  model.simulate()
  model.terminate()
  model.delete()

.. index:: OMSimulatorPython; Scripting Commands

Python Scripting Commands
#########################

.. include:: OMSimulatorPython.inc
