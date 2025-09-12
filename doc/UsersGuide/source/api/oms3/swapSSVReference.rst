swapSSVReference
----------------

Swaps the reference to an existing SSV (System Structure parameter and values) file
with a new one for a specific component or subsystem within the SSP model.

This method is essentially a combination of :code:`removeSSVReference` and
:code:`addSSVReference`. It ensures that the old SSV reference is removed and replaced
with the new SSV reference in a single step, reducing the risk of leaving the system
without any SSV mapping during the transition.

   **Notes**:
    * The old SSV file remains in the ``resources/`` folder of the SSP model.
    * Only the *reference* is swapped — no SSV files are deleted.
    * If a parameter mapping file (.ssm) is associated with the old SSV, it will also be swapped accordingly.
    * This method is equivalent to calling:

      .. code-block:: python

         model.removeSSVReference(cref, old_ssv)
         model.addSSVReference(cref, new_ssv)

**Syntax:**

.. code-block:: python

   swapSSVReference(cref, old_ssv, new_ssv)

**Parameters:**
  - ``cref`` (*CRef*): Reference to a system, sub-system, or component where the SSV file should be swapped.
  - ``old_ssv`` (*str*): Path to the currently referenced SSV file relative to the SSP model resources.
  - ``new_ssv`` (*str*): Path to the new SSV file relative to the SSP model resources.

**Example usage**:

   .. code-block:: python

      # Load an existing SSP model
      model = SSP("swapSSV3.ssp")

      # Swap SSV reference from default system (swap5.ssv → swap6.ssv)
      model.swapSSVReference(CRef("default"), "resources/swap5.ssv", "resources/swap6.ssv")

      # Swap SSV reference from sub-system (swap6.ssv → swap5.ssv)
      model.swapSSVReference(CRef("default", "sub-system"), "resources/swap6.ssv", "resources/swap5.ssv")

      print("After swapping swap6.ssv to default and swap5.ssv to sub-system")
      print("===============================================================")
      model.list()
