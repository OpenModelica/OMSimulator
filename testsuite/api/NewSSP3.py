## status: correct
## teardown_command: rm NewSSP3.ssp
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a component.

model = SSP()
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
component2 = model.addComponent(CRef('default', 'Add2'), 'resources/Add.fmu')

model.list()
model.export('NewSSP3.ssp')

model2 = SSP('NewSSP3.ssp')
model2.list()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": None
## |-- |-- System: default
## |-- |-- Connectors:
## |-- |-- Elements:
## |-- |-- |-- FMU: (Add1)
## |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- (u1, input, Real)
## |-- |-- |-- |-- |-- (u2, input, Real)
## |-- |-- |-- |-- |-- (y, output, Real)
## |-- |-- |-- |-- |-- (k1, parameter, Real)
## |-- |-- |-- |-- |-- (k2, parameter, Real)
## |-- |-- |-- FMU: (Add2)
## |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- (u1, input, Real)
## |-- |-- |-- |-- |-- (u2, input, Real)
## |-- |-- |-- |-- |-- (y, output, Real)
## |-- |-- |-- |-- |-- (k1, parameter, Real)
## |-- |-- |-- |-- |-- (k2, parameter, Real)
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": <hidden>
## |-- |-- System: default
## |-- |-- Connectors:
## |-- |-- Elements:
## |-- |-- |-- FMU: (Add1)
## |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- (u1, input, Real)
## |-- |-- |-- |-- |-- (u2, input, Real)
## |-- |-- |-- |-- |-- (y, output, Real)
## |-- |-- |-- |-- |-- (k1, parameter, Real)
## |-- |-- |-- |-- |-- (k2, parameter, Real)
## |-- |-- |-- FMU: (Add2)
## |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- (u1, input, Real)
## |-- |-- |-- |-- |-- (u2, input, Real)
## |-- |-- |-- |-- |-- (y, output, Real)
## |-- |-- |-- |-- |-- (k1, parameter, Real)
## |-- |-- |-- |-- |-- (k2, parameter, Real)
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
