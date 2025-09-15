exportSSVTemplate
-----------------

Exports a template SSV (System Structure Parameter Values) file for a given system,
sub-system, or component within the SSP model.

The generated SSV template contains all parameters that can be configured for the
referenced scope (system or component), but without assigned values. This is useful
for creating new parameter sets, preparing configuration files, or documenting the
available tunable parameters in a model.

   **Notes**:
    * The exported SSV file contains parameter definitions but not their actual values.
    * The scope of the export depends on the provided `CRef`
         - Root system → exports all parameters for the entire SSP.
         - Sub-system → exports parameters for that sub-system and its components.
         - Component → exports only the parameters of the selected component.
    * The output file is always written to the specified path; existing files will be overwritten.

**Syntax:**

.. code-block:: python

   exportSSVTemplate(cref, ssv_path)

**Parameters:**
  - ``cref`` (*CRef*): Reference to a system, sub-system, or component for which the SSV template should be generated.
  - ``ssv_path`` (*str*): Path where the exported SSV template will be saved.

**Example usage**:

   .. code-block:: python

      from OMSimulator import SSP, CRef

      # Load an existing SSP model
      model = SSP("exportSSVTemplate1.ssp")

      # Export template for the entire SSP
      model.exportSSVTemplate(CRef("default"), "exportSSVTemplate1.ssv")

      # Export template for a sub-system
      model.exportSSVTemplate(CRef("default", "sub-system"), "exportSSVTemplate2.ssv")

      # Export template for a single component (Add1)
      model.exportSSVTemplate(CRef("default", "Add1"), "exportSSVTemplate3.ssv")

      # Export template for another component (Add2)
      model.exportSSVTemplate(CRef("default", "Add2"), "exportSSVTemplate4.ssv")

**Example usage with fmu component**:
   .. code-block:: python

      from OMSimulator import FMU
      fmu = FMU('../resources/Modelica.Blocks.Math.Add.fmu')
      ## export the start values in fmu to ssv template
      fmu.exportSSVTemplate('startValuesSSVTemplate.ssv')