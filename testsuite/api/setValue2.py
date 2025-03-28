## status: correct
## teardown_command: rm -rf setValue2.ssp
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a component and
# set parameter values from the model level

model = SSP()
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='Add.fmu')
component1 = model.addComponent(CRef('default', 'Add1'), 'Add.fmu')
component2 = model.addComponent(CRef('default', 'Add2'), 'Add.fmu')

model.setValue(CRef('default', "Add1", "k1"), 2.0)
model.setValue(CRef('default', "Add1", "k2"), 3.0)

model.setValue(CRef('default', "Add2", "k1"), 100.0)
model.setValue(CRef('default', "Add2", "k2"), 300.0)


model.list()
model.export('setValue2.ssp')

model2 = SSP('setValue2.ssp')
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
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (u1, input, Real)
## |-- |-- |-- |-- (u2, input, Real)
## |-- |-- |-- |-- (y, output, Real)
## |-- |-- |-- |-- (k1, parameter, Real)
## |-- |-- |-- |-- (k2, parameter, Real)
## |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- (k1, 2.0, Real)
## |-- |-- |-- |-- (k2, 3.0, Real)
## |-- |-- |-- FMU: (Add2)
## |-- |-- |-- |-- path: Add.fmu
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (u1, input, Real)
## |-- |-- |-- |-- (u2, input, Real)
## |-- |-- |-- |-- (y, output, Real)
## |-- |-- |-- |-- (k1, parameter, Real)
## |-- |-- |-- |-- (k2, parameter, Real)
## |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- (k1, 100.0, Real)
## |-- |-- |-- |-- (k2, 300.0, Real)
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
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (u1, input, Real)
## |-- |-- |-- |-- (u2, input, Real)
## |-- |-- |-- |-- (y, output, Real)
## |-- |-- |-- |-- (k1, parameter, Real)
## |-- |-- |-- |-- (k2, parameter, Real)
## |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- (k1, 2.0, Real)
## |-- |-- |-- |-- (k2, 3.0, Real)
## |-- |-- |-- FMU: (Add2)
## |-- |-- |-- |-- path: Add.fmu
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (u1, input, Real)
## |-- |-- |-- |-- (u2, input, Real)
## |-- |-- |-- |-- (y, output, Real)
## |-- |-- |-- |-- (k1, parameter, Real)
## |-- |-- |-- |-- (k2, parameter, Real)
## |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- (k1, 100.0, Real)
## |-- |-- |-- |-- (k2, 300.0, Real)
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
