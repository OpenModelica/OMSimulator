removeSSVReference
------------------

Removes the reference to a given SSV (System Structure parameter and values) file from a specific component or subsystem
within the SSP model.This method is useful when a component or subsystem should no longer be
associated with a parameter set defined in an SSV file.

   **Notes**:
    * The SSV file itself remains in the ``resources/`` folder of the SSP.
    * Only the *reference* from the specified component is removed.
    * If a parameter mapping file (.ssm) is associated with the SSV, it is also removed.
    * If the component had multiple SSV references, only the matching one is cleared.

**Syntax:**

.. code-block:: python

   removeSSVReference(cref, ssv_path)

**Parameters:**
  - ``cref`` (*CRef*): Reference to a system, sub-system or component to which the SSV file should be applied.
  - ``ssv_path`` (*str*): Path to the SSV file relative to the SSP model resources.


**Example usage**:

   .. code-block:: python

      # Load an existing SSP model
      model = SSP("swapSSV1.ssp")

      # Remove SSV reference from top-level component Add1
      model.removeSSVReference(CRef("default", "Add1"), "resources/swap1.ssv")

      # Remove SSV reference from Add3 inside the subsystem
      model.removeSSVReference(CRef("default", "sub-system", "Add3"), "resources/swap1.ssv")
      # List the model details to verify removal
      model.list()