.. index:: OMSimulatorPython3

.. _oms3-python :

OMSimulatorPython3
==================

This section documents the new **oms3 Python API**, designed to improve
modularity and reduce overhead, while providing tighter integration with
Python. The updated architecture separates the simulation core from the
SSP (System Structure and Parameterization) standards, offering greater flexibility,
interoperability and efficient model management.


Core Capabilities
-----------------

The **oms3 Python API** provides an extensive set of commands for
building, configuring, and validating SSP models. The following table summarizes
the main functionalities provided by the package.

+----------------------+-------------------------------------------------------------+
| **Category**         | **Functionality**                                           |
+======================+=============================================================+
| **Model Management** | - Create new SSP models                                     |
|                      | - Create, activate, and duplicate variants                  |
|                      | - Add or delete resources, components, systems, connectors  |
|                      | - Create nested systems                                     |
|                      | - Set parameters and solvers                                |
|                      | - Swap SSV references                                       |
|                      |                                                             |
+----------------------+-------------------------------------------------------------+
| **Connectivity**     | - Add components and connectors                             |
|                      | - Define and manage connections                             |
+----------------------+-------------------------------------------------------------+
| **SSP Standards**    | - Import and export SSP models                              |
|                      | - Create SSV and SSM descriptions                           |
|                      | - Export SSV and SSM templates                              |
|                      | - Validate SSP, SSV, and SSM                                |
+----------------------+-------------------------------------------------------------+
| **FMU Integration**  | - Import and validate FMUs as components                    |
+----------------------+-------------------------------------------------------------+
| **Utilities**        | - Error handling, logging, and debugging support            |
+----------------------+-------------------------------------------------------------+


.. index:: OMSimulatorPython3; Command Line Interface

Command Line Interface
-----------------------

In addition to the Python API, the **oms3 Python** package installs a
``OMSimulator`` command line entry point that can run a ``.ssp`` or
``.fmu`` or ``.py`` file directly, without having to write a driver script:

.. code-block:: bash

  OMSimulator model.ssp
  OMSimulator model.fmu

All simulation settings (start/stop time, tolerance, ...) are taken from the
model itself; use the flags below to override them. Both file types are
validated against their schema (FMI for ``.fmu``, SSP for ``.ssp``) before
simulation.

The full list of command line arguments can also be printed at any time with:

.. code-block:: bash

  OMSimulator --help

The table below summarizes the most commonly used flags. For a complete list,
refer to :ref:`OMSimulator Flags <omsimulator-flags>`.

.. list-table:: OMSimulator command line flags
   :header-rows: 1
   :widths: 20 20 60

   * - Flag
     - Value
     - Description
   * - ``--resultFile``
     - ``FILE``
     - Override the result file name.
   * - ``--startTime``
     - ``TIME``
     - Override the simulation start time.
   * - ``--stopTime``
     - ``TIME``
     - Override the simulation stop time.
   * - ``--tolerance``
     - ``TOL``
     - Override the solver tolerance (``.fmu`` only).
   * - ``--stepSize``
     - ``SIZE``
     - Override the (maximum) simulation step size (``.fmu`` only).
   * - ``--mode``
     - ``cs`` | ``me``
     - Force co-simulation or model exchange for FMUs that export both
       kinds (``.fmu`` only).
   * - ``--solver``
     - ``euler`` | ``cvode``
     - Set the ODE solver for model-exchange FMUs (``.fmu``, requires
       ``--mode me``).
   * - ``--stripRoot``
     - —
     - Remove the root system prefix from exported signal names (applies to
       both ``.fmu`` and ``.ssp``).
   * - ``--validate``
     - ``.fmu`` | ``.ssp``
     - Only validate the file against its schema; do not simulate.
   * - ``--calibrate``
     - ``[ACTIVITY]``
     - Fit the parameters of a Calibration activity instead of simulating
       (``.ssp`` only), see :ref:`oms3-calibration`.
   * - ``--calibrationOutput``
     - ``FILE``
     - SSP file the calibrated model is written to.
   * - ``--uqFile``
     - ``PATH``
     - UQ file inside the SSP, if the SSP does not reference it.

.. index:: OMSimulator; Command Line Interface Examples

Examples
^^^^^^^^

.. code-block:: bash

  # Simulate an FMU using its own declared defaults
  OMSimulator model.fmu

  # Force model-exchange mode with the Euler solver and a custom step size
  OMSimulator model.fmu --mode me --solver euler --stepSize 1e-3

  # Only validate an FMU or SSP against its schema, without simulating
  OMSimulator model.fmu --validate
  OMSimulator model.ssp --validate

  # Override the stop time and result file of an SSP
  OMSimulator model.ssp --stopTime 10 --resultFile out.mat

  # Drop the "model.root" prefix from exported signal names (.fmu or .ssp)
  OMSimulator model.fmu --stripRoot
  OMSimulator model.ssp --stripRoot

  # Calibrate the parameters of activity cal_01 against its reference data
  OMSimulator model.ssp --calibrate cal_01 --calibrationOutput calibrated.ssp

.. index:: OMSimulatorPython3; Parameter Calibration

.. _oms3-calibration :

Parameter Calibration
---------------------

The ``OMSimulator.calibration`` module fits parameters of an SSP model to
reference data. The problem is described by a ``Calibration`` activity of the
OpenSCALING UQ layered standard, in a UQ file inside the SSP that the SSD
references with an ``ssc:MetaData`` element of type
``application/x-uncertainty-quantification``:

- ``CalibrationTargets``: the parameters to fit, with initial value and bounds
- ``ObservedVariables``: the simulated variables compared to the reference data
- ``ReferenceData``: a CSV file; the first column is the time
- ``ParameterSet``: further parameter values to set
- ``SimulationSetting``: stop time and tolerance

The fit is a bounded nonlinear least squares problem solved with
``scipy.optimize.least_squares``, so it needs numpy and scipy
(``pip install OMSimulator[calibration]``). Each evaluation resets the model,
sets the parameters, initializes it and steps through the time points of the
reference data, so the FMUs are instantiated only once.

The standard does not yet define how observed variables map to the reference
data, their weights, the calibration time window or optimizer options. These
are read from an optional ``KeyValues`` element of type
``org.openmodelica.omsimulator.calibration`` in the ``SimulationSetting``:

.. list-table:: Calibration options
   :header-rows: 1
   :widths: 30 70

   * - Key
     - Description
   * - ``startTime``, ``stopTime``
     - Calibration time window (default: the reference data up to the
       ``stopTime`` of the ``SimulationSetting``).
   * - ``dataName:<variable>``
     - Reference data column of an observed variable (default: the column
       named like the variable, optionally followed by ``|unit``).
   * - ``weight:<variable>``
     - Residual weight of an observed variable (default: 1).
   * - ``scaling``
     - ``none`` (default), ``range`` or ``std``: divide the residuals of
       each observed variable by the range or standard deviation of its
       reference data.
   * - ``relativeTolerance``
     - Optimizer tolerance on cost, step and gradient (default: 1e-8).
   * - ``maxEvaluations``
     - Maximum number of model evaluations.
   * - ``diffStep``
     - Relative finite difference step for the Jacobian.

The calibrated values are written to ``<activity>_calibrated.ssv`` next to
the UQ file and the ``ResultSet`` of the activity gets one ``ErrorMetric``
of type ``RMSE`` per observed variable, referring to that parameter set.

.. code-block:: python

  from OMSimulator.calibration import calibrateSSP

  result = calibrateSSP("model.ssp", "cal_01", output="calibrated.ssp")
  print(result.parameters, result.rmse)

``CalibrationProblem.fromXML`` and ``Calibration`` run the same fit on an
already instantiated model, e.g., to set further values before calibrating.

Quick start Example
-------------------
The following example demonstrates how to

- Create a new SSP model
- Add FMU components as resources
- Establish connections between components
- Export the model to an SSP file
- Re-import the exported SSP file
- Instantiate the model, set parameter values, and run a simulation

.. code-block:: python

  from OMSimulator import SSP, CRef, Settings
  Settings.suppressPath = True

  # This example creates a new SSP file with an FMU instantiated as a component.
  # It then exports the SSP file and re-imports and set run time value on the instantiated model and the simulates the model.

  model = SSP()
  model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
  model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')

  component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
  component3 = model.addComponent(CRef('default', 'Gain1'), 'resources/Gain.fmu')


  model.addConnection(CRef('default', 'Gain1', 'y'), CRef('default', 'Add1', 'u1'))

  #model.list()
  model.export('SimpleSimulation5.ssp')
  ## import the exported SSP file
  model2 = SSP('SimpleSimulation5.ssp')

  instantiated_model = model2.instantiate()
  instantiated_model.setResultFile("SimpleSimulation5_res.mat")
  instantiated_model.setValue(CRef('default', 'Gain1', 'k'), 2.0)
  instantiated_model.setValue(CRef('default', 'Gain1', 'u'), 6.0)

  print(f"info: After instantiation:")
  print(f"info:    default.Gain1.k: {instantiated_model.getValue(CRef('default', 'Gain1', 'k'))}", flush=True)
  print(f"info:    default.Gain1.u: {instantiated_model.getValue(CRef('default', 'Gain1', 'u'))}", flush=True)
  print(f"info:    default.Gain1.y: {instantiated_model.getValue(CRef('default', 'Gain1', 'y'))}", flush=True)
  print(f"info:    default.Add1.u1: {instantiated_model.getValue(CRef('default', 'Add1', 'u1'))}", flush=True)

  instantiated_model.initialize()
  instantiated_model.simulate()
  print(f"info: After simulation:")
  print(f"info:    default.Gain1.k: {instantiated_model.getValue(CRef('default', 'Gain1', 'k'))}", flush=True)
  print(f"info:    default.Gain1.u: {instantiated_model.getValue(CRef('default', 'Gain1', 'u'))}", flush=True)
  print(f"info:    default.Gain1.y: {instantiated_model.getValue(CRef('default', 'Gain1', 'y'))}", flush=True)
  print(f"info:    default.Add1.u1: {instantiated_model.getValue(CRef('default', 'Add1', 'u1'))}", flush=True)

  instantiated_model.terminate()
  instantiated_model.delete()

.. .. include:: OMSimulatorPython3.inc

.. include:: api/oms3/ssp.rst

.. include:: api/oms3/ssd.rst

.. include:: api/oms3/ssv.rst

.. include:: api/oms3/ssm.rst

.. include:: api/oms3/fmu.rst

.. include:: api/oms3/component.rst

.. include:: api/oms3/addSystem.rst

.. include:: api/oms3/addConnector.rst

.. include:: api/oms3/addConnection.rst

.. include:: api/oms3/addResource.rst

.. include:: api/oms3/addComponent.rst

.. include:: api/oms3/addSSVReference.rst

.. include:: api/oms3/removeSSVReference.rst

.. include:: api/oms3/swapSSVReference.rst

.. include:: api/oms3/listSSVReference.rst

.. include:: api/oms3/exportSSVTemplate.rst

.. include:: api/oms3/exportSSMTemplate.rst

.. include:: api/oms3/setValue.rst

.. include:: api/oms3/getValue.rst

.. include:: api/oms3/mapParameter.rst

.. include:: api/oms3/duplicateAndActivateVariant.rst

.. include:: api/oms3/getVariant.rst

.. include:: api/oms3/listResource.rst

.. include:: api/oms3/deleteResource.rst

.. include:: api/oms3/delete.rst

.. include:: api/oms3/instantiate.rst

.. include:: api/oms3/setResultFile.rst

.. include:: api/oms3/initialize.rst

.. include:: api/oms3/setSolver.rst

.. include:: api/oms3/simulate.rst

.. include:: api/oms3/terminate.rst
