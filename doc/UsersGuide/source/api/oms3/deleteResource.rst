deleteResource
--------------

Removes a resource file from the ``resources/`` folder of the SSP model.

This method deletes the specified resource file from the model archive and updates
the SSP structure accordingly. It is useful for cleaning up unused parameter sets,
mapping files, or FMUs.

   **Notes**:
    * The file is physically removed from the SSP archive — it cannot be recovered unless re-added.
    * If the resource is still referenced by components, connectors, or mappings,
      those references will become invalid and may cause errors.
    * Can be used for any type of resource (e.g., FMU, SSV, SSM, or custom files).

**Syntax:**

.. code-block:: python

   deleteResource(resource_path)

**Parameters:**
  - ``resource_path`` (*str*): Path to the resource file to be deleted, relative to the SSP ``resources/`` directory.

**Example usage**:

.. code-block:: python

   model = SSP("deleteResources1.ssp")
   model.list()

   # Delete a parameter set file
   model.deleteResource("resources/delete3.ssv")
   print("After deleting delete3.ssv:")
   model.list()

   # Delete another SSV file
   model.deleteResource("resources/delete4.ssv")
   print("After deleting delete4.ssv:")
   model.list()

   # Delete an FMU resource
   model.deleteResource("resources/Add.fmu")
   print("After deleting resources/Add.fmu:")
   model.list()
