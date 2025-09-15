addComponent
------------

The **addComponent** method allows you to instantiate a component within
an SSP model. Components can be FMUs or dummy placeholders, enabling
flexible system architecture design and simulation.

**Key Features:**
  - **Component instantiation** — create components based on FMU resources
    or dummy placeholders.
  - **Hierarchical modeling** — components can be placed in nested systems
    using a structured `CRef` path.
  - **Flexible integration** — components can later be connected via
    connectors, linked to SSV files, or assigned custom solver settings.
  - **Supports multiple components** — multiple instances of the same
    FMU can be added with unique identifiers.

**Syntax**

.. code-block:: python

   addComponent(cref, resource_path)

**Parameters:**
  - ``cref`` (*CRef*): The component reference, specifying the hierarchical
    path and component name within the SSP model.
  - ``resource_path`` (*str*): Path to the FMU or model resource that the
    component should instantiate.

**Returns:**
  - The newly created **Component** object.

**Example**

.. code-block:: python

   from OMSimulator import SSP, CRef, Settings

   Settings.suppressPath = True

   model = SSP()

   # Add resources to the SSP model
   model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
   model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')

   # Instantiate components
   component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
   # instantiate multiple instances of the same FMU
   component2 = model.addComponent(CRef('default', 'Add2'), 'resources/Add.fmu')
   component3 = model.addComponent(CRef('default', 'Gain1'), 'resources/Gain.fmu')
   component4 = model.addComponent(CRef('default', 'Gain2'), 'resources/Gain.fmu')
   # list the components in the model
   model.list()
