duplicate and activate Variant
------------------------------

Creates a copy of an existing variant (SSD file) within an SSP model and assigns it a new name.

A variant in SSP represents a specific configuration of a system (stored as an SSD file).
By duplicating a variant, users can create an alternative configuration without modifying
the original one. This is particularly useful for scenario testing, parameter studies,
or creating multiple versions of a system with different inputs, parameters, or components.

   **Notes**:
    * The duplicate is a *deep copy* of the variant — all systems, components, and parameter values are copied.
    * The duplicate can be modified independently of the original (e.g., by adding components, changing parameter values).
    * Duplicated variants must be explicitly added back to the model using :code:`model.add(variant)`.
    * The active variant of a model can be switched using :code:`model.activeVariantName = "VariantName"`.

**Syntax:**

.. code-block:: python

   variant_copy = model.variants['Variant-Name'].duplicate(new_name)

**Parameters:**
  - ``new_name`` (*str*): Name for the duplicated variant.

**Returns:**
  - (*SSD*): A new :class:`SSD` object representing the duplicated variant.

**Example usage**:

.. code-block:: python

   from OMSimulator import SSD, SSP, Settings, CRef

   Settings.suppressPath = True

   # Create a new SSP model with a default variant
   model = SSP(temp_dir="./tmp-duplicateVariant2/")
   model.addResource("../resources/Modelica.Blocks.Math.Add.fmu")
   model.addComponent(CRef("default", "Add1"), "resources/Modelica.Blocks.Math.Add.fmu")

   # Set parameter values in default variant
   model.setValue(CRef("default", "Add1", "u1"), 10.0)
   model.setValue(CRef("default", "Add1", "u2"), 20.0)
   model.setValue(CRef("default", "Add1", "k1"), 30.0)

   # Duplicate the default variant as "Variant-B"
   variantB = model.variants["default"].duplicate("Variant-B")
   model.add(variantB)

   # Modify parameter values in Variant-B
   variantB.setValue(CRef("default", "Add1", "u1"), 100.0)
   variantB.setValue(CRef("default", "Add1", "u2"), 200.0)

   # Export SSP with both variants
   model.export("duplicateVariant2.ssp")

   # Load the model again and duplicate Variant-B as Variant-C
   model2 = SSP("duplicateVariant2.ssp")
   variantC = model2.variants["Variant-B"].duplicate("Variant-C")
   model2.add(variantC)

   # Modify Variant-C independently
   variantC.setValue(CRef("default", "Add1", "u1"), -100.0)
   variantC.addComponent(CRef("default", "Add2"), "resources/Modelica.Blocks.Math.Add.fmu")

   # Switch active variant to Variant-C
   model2.activeVariantName = "Variant-C"

   # Add new component only to Variant-C
   model2.addComponent(CRef("default", "Add3"), "resources/Modelica.Blocks.Math.Add.fmu")
   variantC.setValue(CRef("default", "Add3", "u1"), -200.0)
   variantC.setValue(CRef("default", "Add3", "k1"), -30.0)

   # Print the structure of the model
   model2.list()
