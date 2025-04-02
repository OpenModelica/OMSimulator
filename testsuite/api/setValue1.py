## status: correct
## teardown_command: rm setValue1.ssp
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a component and
# set parameter values from the component level

model = SSP()
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='Add.fmu')
root = model.activeVariant
component1 = model.addComponent(CRef('default', 'Add1'), 'Add.fmu')
component1.setValue("k1", 2.0)
component1.setValue("k2", 3.0)
component2 = model.addComponent(CRef('default', 'Add2'), 'Add.fmu')
component2.setValue("k1", 100.0)
component2.setValue("k2", 300.0)


# component1.setValue("param1", 2.0)
# model.setValue(CRef('default', "Add1", "param1"), 2.0)
# root.setValue(("Add1", "param1"), 2.0)

# ssv = SSV()
# ssv.setValue("param1", 3.0)
# #ssv.export("myfile.ssv")
# model.addResource(ssv, "resources/myfile.ssv")
# model.addSSV(CRef('default', 'Add2'), 'resources/myfile.ssv')



model.list()
model.export('setValue1.ssp')

model2 = SSP('setValue1.ssp')
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
## |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real)
## |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real)
## |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real)
## |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real)
## |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real)
## |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- (Real k1, 2.0, None)
## |-- |-- |-- |-- |-- (Real k2, 3.0, None)
## |-- |-- |-- FMU: (Add2)
## |-- |-- |-- |-- path: Add.fmu
## |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real)
## |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real)
## |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real)
## |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real)
## |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real)
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
## |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real)
## |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real)
## |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real)
## |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real)
## |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real)
## |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- (Real k1, 2.0, None)
## |-- |-- |-- |-- |-- (Real k2, 3.0, None)
## |-- |-- |-- FMU: (Add2)
## |-- |-- |-- |-- path: Add.fmu
## |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real)
## |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real)
## |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real)
## |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real)
## |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real)
## |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- (Real k1, 100.0, None)
## |-- |-- |-- |-- |-- (Real k2, 300.0, None)
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
