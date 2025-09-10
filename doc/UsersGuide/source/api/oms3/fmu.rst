FMU
---

The **FMU** class provides an interface for loading and inspecting
*Functional Mock-up Units (FMUs)*.This class allows users to access key metadata,
states, and variables of an FMU directly from Python, enabling integration and validation.

Typical use cases include:
  - Loading an FMU into memory for inspection or simulation.
  - Accessing FMU metadata such as name, GUID, and FMI version.
  - Iterating through state variables and retrieving their properties.
  - Using variable metadata (signal type, causality, variability,
    value references) for analysis or parameter mapping.

**Example**

.. code-block:: python

   from OMSimulator import FMU

   # Load FMU
   fmu = FMU('../resources/Modelica.Electrical.Analog.Examples.CauerLowPassAnalog.fmu')

   # Print metadata
   print("name:", fmu.modelName)
   print("guid:", fmu.guid)
   print("fmi version:", fmu.fmiVersion)

   # Inspect state variables
   print("States:")
   for var in sorted(fmu.states, key=lambda x: x.name):
       print({
           'name': var.name,
           'signal_type': var.signal_type.name,
           'valueReference': var.valueReference,
           'variability': var.variability,
           'causality': var.causality.name
       })
