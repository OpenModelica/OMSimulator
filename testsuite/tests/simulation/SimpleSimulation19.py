## status: correct
## linux: no
## ucrt64: yes
## win: yes
## mac: no

from OMSimulator import SSP, Settings, CRef

Settings.suppressPath = True

model = SSP(model_name='model', system_name='root')
model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')
model.addComponent(CRef('root', 'Gain'), 'resources/Gain.fmu')

model.export('SimpleSimulation19.ssp')

model2 = SSP('SimpleSimulation19.ssp')

model2.list()
instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,
instantiated_model.setResultFile("SimpleSimulation19_res.mat")
instantiated_model.setValue(CRef('root', 'Gain', 'k'), 2.0)
instantiated_model.setValue(CRef('root', 'Gain', 'u'), 3.0)
instantiated_model.initialize()
instantiated_model.simulate()
print("", flush=True)
print(f"info: After simulation:")
print(f"info:    root.Gain.k : {instantiated_model.getValue(CRef('root', 'Gain', 'k'))}", flush=True)
print(f"info:    root.Gain.u : {instantiated_model.getValue(CRef('root', 'Gain', 'u'))}", flush=True)
print(f"info:    root.Gain.y : {instantiated_model.getValue(CRef('root', 'Gain', 'y'))}", flush=True)
instantiated_model.terminate()
instantiated_model.delete()


## Result:
## info:    Result file: SimpleSimulation19_res.mat (bufferSize=10)
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Gain.fmu
## |-- Active Variant: model
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "model": <hidden>
## |-- |-- System: root 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: Gain 'None'
## |-- |-- |-- |-- |-- path: resources/Gain.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- UnitDefinitions:
## |-- |-- Unit: 1
## |-- |-- |-- BaseUnit:
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
##
## info: After simulation:
## info:    root.Gain.k : 2.0
## info:    root.Gain.u : 3.0
## info:    root.Gain.y : 6.0
## endResult
