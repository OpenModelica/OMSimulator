getVariant
----------

Get the list of available variants for a given system or sub-system within the SSP model.

**Syntax:**

.. code-block:: python

   getVariant(cref : None | str) -> SSD | None

**Parameters:**
  - ``cref`` : variant name (e.g) SystemStructure, VarA, VarB, if None is given then it will return the current active variant.

**Example usage**:

   .. code-block:: python

      from OMSimulator import SSP, CRef

      # Load an existing SSP model
      model = SSP("exportSSMTemplate1.ssp")
      ## get list of available variants
      model.getAllVariantNames()
      ## get current active variant
      varA = model.getVariant("varA")