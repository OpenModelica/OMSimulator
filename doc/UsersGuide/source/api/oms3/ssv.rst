SSV
---

The **SSV** class provides an interface for creating and managing
*System Structure Parameter Values* (SSV) files. These files are used to
define parameter values, units, and variability information for
components within an SSP model.

Typical use cases include:
  - Defining scalar variables and their default values
  - Assigning units and data types to parameters
  - Storing both numeric and string-based parameter values
  - Exporting parameter sets to `.ssv` files for reuse across simulations

**Key Methods**

- ``setValue(name, value, unit=None)``
  Assigns a parameter value. Optionally, a unit can be specified.

- ``export(filename)``
  Writes the defined parameters to an external `.ssv` file.

**Example**

.. code-block:: python

   from OMSimulator import SSV, Settings

   Settings.suppressPath = True

   # Create a new SSV file and define parameters
   ssv1 = SSV()
   ssv1.setValue("k1", 2.0, "m")
   ssv1.setValue("k2", 3.0)
   ssv1.setValue("k3", 3)
   ssv1.setValue("k4", False)
   ssv1.setValue("param3", "hello")

   # Export to file
   ssv1.export("myfile1.ssv")
