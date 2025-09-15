addResource
-----------

The **addResource** method allows you to add external files, such as FMUs,
SSV, SSM, SRMD etc.. to an SSP model’s resource directory. This ensures that all required
model files are bundled with the SSP package and can be referenced
consistently during simulation.

**Key Features:**
  - **File management** — copy or rename external FMUs and resources into
    the SSP project structure only once and use the instances multiple times in the model.
  - **Integration with components** — added resources can then be used
    to instantiate components within the SSP model.

**Syntax**

.. code-block:: python

   addResource(source_path, new_name=None)

**Parameters:**
  - ``source_path`` (*str*): Path to the external resource file (e.g., FMU)
  - ``new_name`` (*str, optional*): New name for the resource within the SSP project structure. If not provided, the original filename is used.

**Example**

.. code-block:: python

   from OMSimulator import SSP, CRef, Settings

   # Add an FMU to the SSP model's resources folder
   model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
   model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')
