setValue
--------

Assigns a numerical value to a parameter of a component within the SSP model.

This method is used to directly set start values or parameter values of model variables
defined in an FMU component. The values are stored in the SSP model and will be applied
when the model is simulated or exported.

   **Notes**:
    * The parameter must exist in the referenced FMU/component or a top level system connectors; otherwise, an error will be raised.
    * Values are stored at the SSP level and exported together with the model structure.
    * Supported data types are numerical (``float``, ``int``, ``str``, ``bool``).
    * Parameters can be set before or after instantation.

**Syntax:**

.. code-block:: python

   setValue(cref, value)

**Parameters:**
  - ``cref`` (*CRef*): Reference to the parameter variable within a component. The :class:`CRef` must specify the system, component, and parameter name (e.g., ``CRef("default", "Add1", "k1")``).
  - ``value`` (*float* | *int* | *str* | *bool* ): The value to assign to the parameter.

**Example usage**:

.. code-block:: python

   from OMSimulator import SSP, CRef, Settings

   Settings.suppressPath = True

   # Create a new SSP model
   model = SSP()

   # Add FMU resource
   model.addResource("../resources/Modelica.Blocks.Math.Add.fmu", new_name="Add.fmu")

   # Add two components
   model.addComponent(CRef("default", "Add1"), "Add.fmu")
   model.addComponent(CRef("default", "Add2"), "Add.fmu")

   # Set parameter values for Add1
   model.setValue(CRef("default", "Add1", "k1"), 2.0)
   model.setValue(CRef("default", "Add1", "k2"), 3.0)

   # Set parameter values for Add2
   model.setValue(CRef("default", "Add2", "k1"), 100.0)
   model.setValue(CRef("default", "Add2", "k2"), 300.0)

   # Display model details
   model.list()

   # Export the SSP with parameter values applied
   model.export("setValue2.ssp")
