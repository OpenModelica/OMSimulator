listResource
------------

Lists all resource files stored in the ``resources/`` folder of the SSP model.

This method is useful for inspecting the files available in the model, including FMUs,
SSV parameter files, SSM mapping files, and other artifacts. It provides a quick overview
of which resources are packaged inside the SSP archive.

**Syntax:**

.. code-block:: python

   listResource() -> list[str]

**Returns:**
  - (*list[str]*): A list of resource file paths contained in the SSP model.

**Example usage**:

.. code-block:: python

   model = SSP("deleteResources1.ssp")
   model.list()

   print("list of Resources in SSP:")
   print("==========================")
   print(model.listResource())
