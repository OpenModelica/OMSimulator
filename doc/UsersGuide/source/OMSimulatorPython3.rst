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

.. include:: api/oms3/simulate.rst

.. include:: api/oms3/terminate.rst
