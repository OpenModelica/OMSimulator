delete
------

Deletes a connector, component, or subsystem from the SSP model.

This method removes the specified element from the model hierarchy. It can be used to:
- Remove individual connectors (parameters or inputs/outputs) from a system or component.
- Remove components from a subsystem or top-level system.
- Remove entire subsystems, including all their child components and connectors.

   **Notes**:
    * Deleting a connector removes it from the system/component, but does not affect other connectors or components.
    * Deleting a component removes all its connectors and any associated parameter values.
    * Deleting a subsystem removes the subsystem and all nested components and connectors.
    * Any SSV/SSM references pointing to deleted elements may become invalid.
    * The operation modifies the active variant; other variants are unaffected unless explicitly modified.

**Syntax:**

.. code-block:: python

   delete(cref)

**Parameters:**
  - ``cref`` (*CRef*): Reference to the element to delete. Can point to a connector, component, or subsystem. Examples:
      - Connector: ``CRef("default", "param1")``
      - Component: ``CRef("default", "Add1")``
      - Subsystem: ``CRef("default", "sub-system")``

**Example usage**:

.. code-block:: python

   from OMSimulator import SSP, CRef

   # Load an existing SSP model
   model2 = SSP("deleteConnector1.ssp")

   # Delete individual connectors
   model2.delete(CRef("default", "param1"))
   model2.delete(CRef("default", "sub-system", "input"))
   model2.delete(CRef("default", "sub-system", "Add2", "u1"))
   model2.delete(CRef("default", "Add1", "u1"))

   print("## After deleting connectors:")
   model2.list()

   # Delete an entire component
   model2.delete(CRef("default", "sub-system", "Add2"))
   print("## After deleting component Add2 from sub-system:")
   model2.list()
   # Delete an entire sub-system
   model2.delete(CRef('default', 'sub-system'))
   print("## After deleting sub-system:")
   model2.list()
   # Delete the entire system
   model2.delete(CRef('default'))
   print("## After deleting full system:")
   model2.list()
