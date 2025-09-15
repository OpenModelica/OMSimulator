addConnector
------------

The **addConnector** API is used to define external interfaces for systems
and components within an SSP model. A connector specifies the signal
type and causality (e.g., input, output, or parameter) of a variable,
allowing subsystems and components to communicate through well-defined
ports.

Connectors can be added at different hierarchy levels:
  - **Top-level system connectors**, which expose inputs, outputs, and parameters
    of the entire SSP model.
  - **Subsystem connectors**, which define interaction points within nested
    systems and enable modular system design.

**Syntax:**

.. code-block:: python

   addConnector(Connector(name, causality, signal_type))

**Parameters:**
  - ``name`` (*str*): Name of the connector.
  - ``causality`` (*Causality*): Role of the connector (``input``, ``output``, or ``parameter``).
  - ``signal_type`` (*SignalType*): Type of signal (e.g., ``Real``, ``Integer``,``Boolean``, ``String``).

**Example:**

.. code-block:: python

   from OMSimulator import SSP, CRef, Connector, Causality, SignalType

   model = SSP()
   # Add a top-level system connector
   model.activeVariant.system.addConnector(Connector('input1', Causality.input, SignalType.Real))
   # Add a connector to a subsystem
   model.addSystem(CRef('default', 'sub-system'))
   model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('output', Causality.output, SignalType.Real))
