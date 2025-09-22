## status: correct
## teardown_command: rm -rf Enumtest1.ssp
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a component and
# set parameter values from the component level

model = SSP()
model.addResource('../resources/Enum1.fmu', new_name='resources/Enum1.fmu')
model.addComponent(CRef('default', 'Enum1'), 'resources/Enum1.fmu')

model.export('Enumtest1.ssp')

model2 = SSP('Enumtest1.ssp')
model2.list()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Enum1.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: Enum1 'None'
## |-- |-- |-- |-- |-- path: resources/Enum1.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Enumeration, None, 'None')
## |-- EnumerationDefinitions:
## |-- |-- EnumName: E
## |-- |-- |-- EnumItems: one: 1, two: 2, three: 3
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
