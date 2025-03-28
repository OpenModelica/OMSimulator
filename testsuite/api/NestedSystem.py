## status: correct
## teardown_command:
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a component.

model = SSP()
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
model.addSystem(CRef('default', 'sub-system'))
model.addSystem(CRef('default', 'sub-system2'))
model.addSystem(CRef('default', 'sub-system2', 'sub-sub-system'))
component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
component2 = model.addComponent(CRef('default', 'sub-system', 'Add2'), 'resources/Add.fmu')

model.list()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": None
## |-- |-- System: default
## |-- |-- Connectors:
## |-- |-- Elements:
## |-- |-- |-- System: sub-system
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: (Add2)
## |-- |-- |-- |--   path: resources/Add.fmu
## |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- * (u1, input, Real)
## |-- |-- |-- |-- |-- * (u2, input, Real)
## |-- |-- |-- |-- |-- * (y, output, Real)
## |-- |-- |-- |-- |-- * (k1, parameter, Real)
## |-- |-- |-- |-- |-- * (k2, parameter, Real)
## |-- |-- |-- System: sub-system2
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-sub-system
## |-- |-- |-- |-- Connectors:
## |-- |-- |-- FMU: (Add1)
## |-- |-- |--   path: resources/Add.fmu
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- * (u1, input, Real)
## |-- |-- |-- |-- * (u2, input, Real)
## |-- |-- |-- |-- * (y, output, Real)
## |-- |-- |-- |-- * (k1, parameter, Real)
## |-- |-- |-- |-- * (k2, parameter, Real)
## |-- DefaultExperiment
## |--   startTime: 0.0
## |--   stopTime: 1.0
## endResult
