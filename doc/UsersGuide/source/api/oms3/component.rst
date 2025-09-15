Component
---------

The **Component** class provides a flexible interface for defining
modular building blocks in an SSP model. It supports **architectural
modeling**, allowing components to represent actual FMUs or
“dummy fmus” with expected connectors and parameter values.
This makes it possible to design and validate system architectures
before all FMUs are available.

**Key Features:**
  - **Flexible instantiation** — create a component from an FMU or as a dummy placeholder.
  - **Connector support** — define inputs, outputs, and parameters with explicit causality and signal type.
  - **Parameter management** — assign values to parameters or inputs directly within the component.
  - **SSV integration** — link components to SSV files to define and manage parameter sets externally.
  - **System integration** — add the component to SSP models for hierarchical or flat system architectures.


.. code-block:: python

   from OMSimulator import CRef, Connector, Causality, SignalType, SSV
   from OMSimulator.component import Component

   # Create a dummy component
   component = Component(CRef('add'), 'dummy.fmu')

   # Add connectors
   component.addConnector(Connector('input', Causality.input, SignalType.Real))
   component.addConnector(Connector('output', Causality.output, SignalType.Real))
   component.addConnector(Connector('parameter', Causality.parameter, SignalType.Real))

   # Set parameter values
   component.setValue('parameter', 1.0)
   component.setValue('input', 2.0)

   # Create an SSV file for parameter initialization
   ssv = SSV()
   ssv.setValue('parameter', 1.0)
   ssv.setValue('input', 2.0)
   ssv.export('dummy.ssv')

   # Link SSV file to component
   component.addSSVReference('dummy.ssv')

   # List component details
   component.list()
