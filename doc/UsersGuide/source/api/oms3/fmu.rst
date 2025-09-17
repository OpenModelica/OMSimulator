FMU
---

The **FMU** class provides an interface for loading and inspecting
*Functional Mock-up Units (FMUs)*.This class allows users to access key metadata,
states, and variables of an FMU directly from Python, enabling integration, validation and simulation.

Typical use cases include:
  - Loading an FMU into memory for inspection or simulation.
  - Accessing FMU metadata such as name, GUID, and FMI version.
  - Iterating through state variables and retrieving their properties.
  - Using variable metadata (signal type, causality, variability,
    value references) for analysis or parameter mapping.
  - set default experiment settings such as start time, stop time, step size, and tolerance.
  - set parameters and inputs before simulation.

**Syntax**
    fmu = FMU(fmuPath: str, instanceName: str = "")
**Arguments**
  - *fmuPath*: Path to the FMU file.
  - *instanceName*: Optional name for the FMU instance. If not provided, it defaults to the model name of the FMU. The instance name is used in the result file as prefix for the variable names.

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
   ## instantiate the FMU
   fmu.instantiate()
   ## set result file
   fmu.setResultFile('CauerLowPass_res.mat')
   ## set parameter and input values
   fmu.setValue('R1', 1000.0)
   fmu.setValue('C1.u', 1e-6)
   ## initialize the FMU
   fmu.initialize()
   ## simulate the FMU
   fmu.simulate()
   fmu.terminate()
   fmu.delete()