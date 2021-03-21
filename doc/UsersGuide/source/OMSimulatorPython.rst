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

Example: Pi
###########

This example uses a simple Modelica model and FMI-based batch
simulation to approximate the value of pi.

A Modelica model is used to calculate two uniform distributed
pseudo-random numbers between 0 and 1 based on a seed value and
evaluates if the resulting coordinate is inside the unit circle or
not.

.. code-block:: modelica

  model Circle
    parameter Integer globalSeed = 30020 "global seed to initialize random number generator";
    parameter Integer localSeed = 614657 "local seed to initialize random number generator";
    Real x;
    Real y;
    Boolean inside = x*x + y*y < 1.0;
  protected
    Integer state128[4];
  algorithm
    when initial() then
      state128 := Modelica.Math.Random.Generators.Xorshift128plus.initialState(localSeed, globalSeed);
      (x, state128) := Modelica.Math.Random.Generators.Xorshift128plus.random(state128);
      (y, state128) := Modelica.Math.Random.Generators.Xorshift128plus.random(state128);
    end when;
    annotation(uses(Modelica(version="4.0.0")));
  end Circle;

The model is then exported using the FMI interface and the generated
FMU can then be used to run a million simulations in just a few
seconds.

.. code-block:: python
  :caption: Batch simulation of the simple `Cirlce` model with different seed values. All OMSimulator-related comands are highlighted for convenience.
  :linenos:
  :emphasize-lines: 3,5-6,8-10,12-13,22-25,29-30

  import math
  import matplotlib.pyplot as plt
  import OMSimulator as oms

  # redirect logging to file and limit the file size to 65MB
  oms.setLogFile('pi.log', 65)

  model = oms.newModel('pi')
  root = model.addSystem('root', oms.Types.System.SC)
  root.addSubModel('circle', 'Circle.fmu')

  model.resultFile = '' # no result file
  model.instantiate()

  results = list()
  inside = 0

  MIN = 100
  MAX = 1000000
  for i in range(0, MAX+1):
    if i > 0:
      model.reset()
    model.setInteger('root.circle.globalSeed', i)
    model.initialize()
    if model.getBoolean("root.circle.inside"):
      inside = inside + 1
    if i >= MIN:
      results.append(4.0*inside/i)
  model.terminate()
  model.delete()

  plt.plot([MIN, MAX], [math.pi, math.pi], 'r--', range(MIN, MAX+1), results)
  plt.xscale('log')
  plt.ylabel('Approximation of pi')
  plt.savefig('pi.png')

The following figure shows the approximation of pi in relation to the
number of samples.

.. figure :: images/pi.png

  Results of the above batch simulation which approximates the value of pi
