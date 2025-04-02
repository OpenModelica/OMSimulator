## status: correct
## teardown_command: rm setValue3.ssp
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a component and
# set parameter values from the root level

model = SSP()
root = model.activeVariant
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='Add.fmu')
component1 = model.addComponent(CRef('default', 'Add1'), 'Add.fmu')
component2 = model.addComponent(CRef('default', 'Add2'), 'Add.fmu')


root.system.setValue(CRef("Add1", "k1"), 2.0)
root.system.setValue(CRef("Add1", "k2"), 3.0)

root.system.setValue(CRef("Add2", "k1"), 100.0)
root.system.setValue(CRef("Add2", "k2"), 300.0)


model.list()
model.export('setValue3.ssp')

model2 = SSP('setValue3.ssp')
model2.list()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   Add.fmu
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": None
## |-- |-- System: default
## |-- |-- Connectors:
## |-- |-- Elements:
## |-- |-- |-- FMU: (Add1)
## |-- |-- |-- |-- path: Add.fmu
## |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None)
## |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None)
## |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None)
## |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None)
## |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None)
## |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- (Real k1, 2.0, None)
## |-- |-- |-- |-- |-- (Real k2, 3.0, None)
## |-- |-- |-- FMU: (Add2)
## |-- |-- |-- |-- path: Add.fmu
## |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None)
## |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None)
## |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None)
## |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None)
## |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None)
## |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- (Real k1, 100.0, None)
## |-- |-- |-- |-- |-- (Real k2, 300.0, None)
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   Add.fmu
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": <hidden>
## |-- |-- System: default
## |-- |-- Connectors:
## |-- |-- Elements:
## |-- |-- |-- FMU: (Add1)
## |-- |-- |-- |-- path: Add.fmu
## |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None)
## |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None)
## |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None)
## |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None)
## |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None)
## |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- (Real k1, 2.0, None)
## |-- |-- |-- |-- |-- (Real k2, 3.0, None)
## |-- |-- |-- FMU: (Add2)
## |-- |-- |-- |-- path: Add.fmu
## |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None)
## |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None)
## |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None)
## |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None)
## |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None)
## |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- (Real k1, 100.0, None)
## |-- |-- |-- |-- |-- (Real k2, 300.0, None)
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
