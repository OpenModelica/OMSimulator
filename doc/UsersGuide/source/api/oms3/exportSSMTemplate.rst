exportSSVTemplate
-----------------

Exports a template SSM (System Structure Parameter mapping) file for a given system,
sub-system, or component within the SSP model.

The generated SSM template contains all parameters that can be configured for the
referenced scope (system or component), with source and target variables.
The source attribute in ssm template will be empty and should be set by user
to map to the desired variable in the SSV file. This is useful
for creating new parameter mapping files, preparing configuration files.

   **Notes**:
    * The exported SSM file contains source and target definitions but source attribute will be empty.
    * The scope of the export depends on the provided `CRef`
         - Root system → exports all parameters for the entire SSP.
         - Sub-system → exports parameters for that sub-system and its components.
         - Component → exports only the parameters of the selected component.
    * The output file is always written to the specified path; existing files will be overwritten.

**Syntax:**

.. code-block:: python

   exportSSMTemplate(cref, ssm_path)

**Parameters:**
  - ``cref`` (*CRef*): Reference to a system, sub-system, or component for which the SSV template should be generated.
  - ``ssm_path`` (*str*): Path where the exported SSV template will be saved.

**Example usage**:

   .. code-block:: python

      from OMSimulator import SSP, CRef

      # Load an existing SSP model
      model = SSP("exportSSMTemplate1.ssp")

      # Export template for the entire SSP
      model.exportSSMTemplate(CRef("default"), "exportSSMTemplate1.ssm")

      # Export template for a sub-system
      model.exportSSMTemplate(CRef("default", "sub-system"), "exportSSMTemplate2.ssm")

      # Export template for a single component (Add1)
      model.exportSSMTemplate(CRef("default", "Add1"), "exportSSMTemplate3.ssm")

      # Export template for another component (Add2)
      model.exportSSMTemplate(CRef("default", "Add2"), "exportSSMTemplate4.ssm")

**Example usage with fmu component**:

   .. code-block:: python

      from OMSimulator import FMU
      fmu = FMU('../resources/Modelica.Blocks.Math.Add.fmu')
      ## export the start values in fmu to ssv template
      fmu.exportSSMTemplate('startValuesSSVTemplate.ssm')