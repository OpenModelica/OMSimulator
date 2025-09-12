mapParameter
------------

Creates a parameter mapping between a connector and one or more parameters within the SSP model.

   **Notes**:
    * Parameter mapping enables hierarchical parameterization of systems and sub-systems.
    * A single connector can be mapped to multiple parameters (fan-out mapping).
    * Mappings can be stored inline or in the associated SSM (System Structure Mapping) file.

**Syntax:**

.. code-block:: python

   mapParameter(cref, connector, parameter)

**Parameters:**
  - ``cref`` (*CRef*): Reference to the system, sub-system, or component where the mapping is applied.
    Typically, this is the root system (e.g., ``CRef("default")``).
  - ``connector`` (*str*): Name of the connector providing the source value.
  - ``parameter`` (*str*): Name of the target parameter or input to be mapped to the connector.

**Example usage**:

.. code-block:: python

   from OMSimulator import SSP, CRef, Connector, Causality, SignalType

   # Create a new SSP model
   model = SSP()

   # Add top-level system connectors
   model.activeVariant.system.addConnector(Connector("param1", Causality.parameter, SignalType.Real))
   model.activeVariant.system.addConnector(Connector("param2", Causality.parameter, SignalType.Real))
   model.activeVariant.system.addConnector(Connector("param3", Causality.parameter, SignalType.Real))
   model.activeVariant.system.addConnector(Connector("input1", Causality.input, SignalType.Real))
   model.activeVariant.system.addConnector(Connector("input2", Causality.input, SignalType.Real))

   # Assign values to virtual connectors
   model.setValue(CRef("default", "connector_param"), 2.0)
   model.setValue(CRef("default", "connector_input"), 3.0)

   # Map top-level parameters
   model.mapParameter(CRef("default"), "connector_param", "param1")
   model.mapParameter(CRef("default"), "connector_param", "param2")
   model.mapParameter(CRef("default"), "connector_param", "param3")

   # Map inputs
   model.mapParameter(CRef("default"), "connector_input", "input1")
   model.mapParameter(CRef("default"), "connector_input", "input2")
