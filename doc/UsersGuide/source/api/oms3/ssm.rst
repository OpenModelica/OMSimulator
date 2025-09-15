SSM
---

The **SSM** class provides an interface for creating and managing
*System Structure and Mapping* (SSM) files. These files define how
parameters declared in an SSV file are mapped to component parameters
within an SSP model. This enables separation of parameter definitions
from their usage, improving reusability and modularity.

Typical use cases include:
  - Linking global parameter definitions (from SSV files) to component parameters in an SSP model.
  - Supporting multiple parameter mappings for the same input across different components.
  - Creating flexible configurations by swapping or updating parameter mappings without modifying the model structure.

**Key Methods**

- ``mapParameter(source, target)``
  Creates a mapping between a parameter defined in an SSV file
  (``source``) and a component parameter or input signal (``target``).

- ``export(filename)``
  Writes the defined parameter mappings to an external `.ssm` file.

**Example**

.. code-block:: python

   from OMSimulator import SSV, SSM, Settings

   Settings.suppressPath = True

   # Define parameters in SSV
   ssv1 = SSV()
   ssv1.setValue("Input1", 2.0)
   ssv1.setValue("param1", 3.0)
   ssv1.setValue("param2", 3)
   ssv1.export("mapping1.ssv")

   # Create parameter mappings in SSM
   ssm1 = SSM()
   ssm1.mapParameter("Input1", "Add1.u1")
   ssm1.mapParameter("Input1", "u2")
   ssm1.mapParameter("Input1", "u3")
   ssm1.mapParameter("param1", "k1")
   ssm1.mapParameter("param2", "k2")
   ssm1.export("mapping2.ssm")
