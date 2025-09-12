listSSVReference
----------------

Retrieves the list of SSV (System Structure parameter and values) files currently referenced
by a system, sub-system, or component within the SSP model.

   **Notes**:
    * Only the references are listed — the method does not open or parse the SSV files.
    * If no SSV file is associated with the given scope, an empty list is returned.
    * The returned paths are relative to the ``resources/`` directory of the SSP archive.

**Syntax:**

.. code-block:: python

   listSSVReference(cref) -> list[dict]

**Parameters:**
  - ``cref`` (*CRef*): Reference to a system, sub-system, or component for which SSV references should be listed.

**Returns:**
  - (*list[dict]*): A list of dict referenced by the given system, sub-system, or component.
    - Each dict contains:
        - ``ssv`` (*str*): Path to the referenced SSV file relative to the SSP model resources.
        - ``ssm`` (*str* | *None*): Path to the associated SSM file if one exists; otherwise, ``None``.

**Example usage**:

.. code-block:: python

   model = SSP("listSSVReference1.ssp")
   model.list()

   print("List of SSV references")
   print("=======================")
   print(f"Top level system SSV resources    : {model.listSSVReference(CRef('default'))}")
   print(f"Top level sub-system SSV resources: {model.listSSVReference(CRef('default', 'sub-system'))}")
   print(f"Component Add1 SSV resources      : {model.listSSVReference(CRef('default', 'Add1'))}")
   print(f"Component Add2 SSV resources      : {model.listSSVReference(CRef('default', 'Add2'))}")
   print(f"Component Add3 SSV resources      : {model.listSSVReference(CRef('default', 'sub-system', 'Add3'))}")
