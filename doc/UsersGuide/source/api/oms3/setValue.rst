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
    * Special FMI-3.0 data types like ``Float64``, ``Float32``, ``Int8``, ``Int16``, ``Int32``, ``Int64``, ``UInt8``, ``UInt16``, ``UInt32``, ``UInt64`` are handled via special
      classes in OMSimulator like ``Float64``, ``Int32``, etc. These classes can be imported from OMSimulator and used to set parameter values with specific fmi3 data types.
    * Parameters can be set before or after instantation.

**Syntax:**

.. code-block:: python

   setValue(cref, value)

**Parameters:**
  - ``cref`` (*CRef*): Reference to the parameter variable within a component. The :class:`CRef` must specify the system, component, and parameter name (e.g., ``CRef("default", "Add1", "k1")``).
  - ``value`` (*float* | *int* | *str* | *bool* | *Float64* | *Float32* | *Int8* | *Int16* | *Int32* | *Int64* | *UInt8* | *UInt16* | *UInt32* | *UInt64*): The value to assign to the parameter.

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


**Example of setting FMI-3.0 specific data types:**

.. code-block:: python

   from OMSimulator import SSP, CRef, Settings, Float64, Int32, UInt16, Float32, Int8, Int16, Int64, UInt8, UInt32, UInt64

   model = SSP()
   model.addResource("../resources/Feedthrough3.fmu", new_name="resources/Feedthrough3.fmu")
   model.addComponent(CRef("default", "Feedthrough1"), "resources/Feedthrough3.fmu")

   model.setValue(CRef("default", "Feedthrough1", "Float64_continuous_input"), Float64(3.5))
   model.setValue(CRef("default", "Feedthrough1", "Int64_input"), Int64(7))
   model.setValue(CRef("default", "Feedthrough1", "Float32_continuous_input"), Float32(1.5))
   model.setValue(CRef("default", "Feedthrough1", "Int8_input"), Int8(3))
   model.setValue(CRef("default", "Feedthrough1", "Boolean_input"), True)

   model.setValue(CRef("default", "Feedthrough1", "Float32_discrete_input"), Float32(2.5))
   model.setValue(CRef("default", "Feedthrough1", "Enumeration_input"), 2)

   model.setValue(CRef("default", "Feedthrough1", "Int16_input"), Int16(1000))
   model.setValue(CRef("default", "Feedthrough1", "UInt16_input"), UInt16(2000))
   model.setValue(CRef("default", "Feedthrough1", "UInt32_input"), UInt32(3000))
   model.setValue(CRef("default", "Feedthrough1", "UInt64_input"), UInt64(4000))
