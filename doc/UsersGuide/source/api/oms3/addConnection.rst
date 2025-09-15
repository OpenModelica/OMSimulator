addConnection
-------------

The **addConnection** API is used to link the output of one component to the input of another
within an SSP model. Connections define the signal flow between components, ensuring that
data is transmitted correctly during simulation.

Connections are defined using source and target references:
  - **Source**: Specifies the output variable of a component.
  - **Target**: Specifies the input variable of another component.

**Syntax:**

.. code-block:: python

   addConnection(source: CRef, target: CRef)

**Parameters:**
  - ``source`` (*CRef*): Reference to the output variable of the source component.
  - ``target`` (*CRef*): Reference to the input variable of the target component.

**Example:**

.. code-block:: python

  from OMSimulator import SSP, CRef, Settings

  model = SSP()
  model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
  model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')

  model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
  model.addComponent(CRef('default', 'Gain1'), 'resources/Gain.fmu')
  ## add a connection from Gain1.y to Add1.u1
  model.addConnection(CRef('default', 'Gain1', 'y'), CRef('default', 'Add1', 'u1'))
  ## list the components in the model and connections
  model.list()
