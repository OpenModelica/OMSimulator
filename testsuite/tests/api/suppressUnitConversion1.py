## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: no
## the FMUs this test loads leak memory inside their own binaries

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

model = SSP()
model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')
model.addComponent(CRef('default', 'Gain1'), 'resources/Gain.fmu')
model.addComponent(CRef('default', 'Gain2'), 'resources/Gain.fmu')
model.addComponent(CRef('default', 'Gain3'), 'resources/Gain.fmu')

model.addConnection(CRef('default', 'Gain1', 'y'), CRef('default', 'Gain2', 'u'), suppressUnitConversion=True)
model.addConnection(CRef('default', 'Gain2', 'y'), CRef('default', 'Gain3', 'u'))
model.export('suppressUnitConversion1.ssp')

model2 = SSP('suppressUnitConversion1.ssp')
model2.list()

instantiated_model = model2.instantiate()
instantiated_model.initialize()
instantiated_model.terminate()
instantiated_model.delete()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Gain.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: Gain1 'None'
## |-- |-- |-- |-- |-- path: resources/Gain.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- |-- FMU: Gain2 'None'
## |-- |-- |-- |-- |-- path: resources/Gain.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- |-- FMU: Gain3 'None'
## |-- |-- |-- |-- |-- path: resources/Gain.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- Connections:
## |-- |-- |-- |-- Gain1.y -> Gain2.u
## |-- |-- |-- |-- |-- suppressUnitConversion: true
## |-- |-- |-- |-- Gain2.y -> Gain3.u
## |-- UnitDefinitions:
## |-- |-- Unit: 1
## |-- |-- |-- BaseUnit:
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## info:    Result file: default_res.mat (bufferSize=10)
## endResult
