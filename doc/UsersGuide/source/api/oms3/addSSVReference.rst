addSSVReference
---------------

The **addSSVReference** API is used to associate an SSV (System Structure Parameterer and values) file
with a specific component in an SSP model. This allows parameter values defined in the SSV file
to be applied directly to the component, enabling flexible configuration and reuse of parameter sets.

The API also supports an optional third argument for specifying a **parameter mapping file** (.ssm),
which defines how the variables in the SSV file map to the parameters of the component.

**Syntax:**

.. code-block:: python

   addSSVReference(cref, ssv_path: str, ssm_path: str = None)

**Parameters:**
  - ``cref`` (*CRef*): Reference to a system, sub-system or component to which the SSV file should be applied.
  - ``ssv_path`` (*str*): Path to the SSV file relative to the SSP model resources.
  - ``ssm_path`` (*str, optional*): Path to a parameter mapping file (.ssm) that defines how
    SSV variables correspond to component parameters.

**Example:**

.. code-block:: python

  from OMSimulator import SSP, CRef, Settings, SSV, SSM, Connector, Causality, SignalType

  Settings.suppressPath = True

  # This example creates a new SSP file with an FMU instantiated as a component and
  # set parameter values to ssv file and map some parameters to ssm file

  model = SSP()
  model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
  component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
  component2 = model.addComponent(CRef('default', 'Add2'), 'resources/Add.fmu')

  ## create a ssv file
  ssv1 = SSV()
  ssv1.setValue("connector_param", 2.0)
  ssv1.setValue("connector_input", 3.0)
  ssv1.export("myfile3.ssv")

  ## create a ssm file
  ssm = SSM()
  ssm.mapParameter("connector_param", "u1")
  ssm.mapParameter("connector_input", "u2")
  ssm.export("myfile4.ssm")


   ## Add  SSV resource to the model
   model.addResource("myfile3.ssv", "resources/myfile3.ssv")
   ## Add ssm resources to the model
   model.addResource("myfile3.ssv", "resources/myfile4.ssm")

   # Reference the SSV to a specific component
   model.addSSVReference(CRef('default', 'Add1'), 'resources/myfile3.ssv')

   # With an optional SSM mapping file
   model.addSSVReference(CRef('default', 'Add2'), 'resources/myfile4.ssv', 'resources/myfile4.ssm')
