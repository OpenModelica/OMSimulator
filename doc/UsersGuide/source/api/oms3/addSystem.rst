addSystem
---------

The **addSystem** method creates a new system within an SSP model.
Systems serve as containers for components and connectors, and can be
organized hierarchically to support **nested-system architectures**.
This enables modular, scalable, and reusable system designs.

**Key Features:**
  - **System instantiation** — create new systems within the SSP model.
  - **Hierarchical modeling** — use structured ``CRef`` paths to create
    nested systems (e.g., a subsystem inside another subsystem).
  - **Organization** — group related components, connectors, and resources
    within a defined subsystem for clarity and reusability.
  - **Scalability** — build complex system hierarchies with multiple levels.

**Syntax**

.. code-block:: python

   addSystem(cref)

**Parameters:**
  - ``cref`` (*CRef*): The system reference, specifying the hierarchical path and system name within the SSP model.


**Example**

.. code-block:: python

   from OMSimulator import SSP, CRef

   model = SSP()

   # Create systems
   model.addSystem(CRef('default', 'sub-system'))
   model.addSystem(CRef('default', 'sub-system2'))

   # Create a nested subsystem inside 'sub-system2'
   model.addSystem(CRef('default', 'sub-system2', 'sub-sub-system'))
