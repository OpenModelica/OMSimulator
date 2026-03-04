## status: correct
## teardown_command: rm -rf LinearTransformation3_res.mat
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, Connector, Causality, SignalType, SSV, SSM

Settings.suppressPath = True


## this example shows how to use linear transformation in parameter mapping

model2 = SSP('../resources/LinearTransformation3.ssp')
model2.list()
instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,
instantiated_model.setResultFile("LinearTransformation3_res.mat")

print(f"info: After instantiation:")

print(f"info:    default.Gain1.k: {instantiated_model.getValue(CRef('default', 'Gain1', 'k'))}", flush=True)
print(f"info:    default.Gain1.u: {instantiated_model.getValue(CRef('default', 'Gain1', 'u'))}", flush=True)
print(f"info:    default.Gain1.y: {instantiated_model.getValue(CRef('default', 'Gain1', 'y'))}", flush=True)
print(f"info:    default.Add1.u1: {instantiated_model.getValue(CRef('default', 'Add1', 'u1'))}", flush=True)


instantiated_model.initialize()
instantiated_model.simulate()
print(f"info: After simulation:")
print(f"info:    default.Gain1.k: {instantiated_model.getValue(CRef('default', 'Gain1', 'k'))}", flush=True)
print(f"info:    default.Gain1.u: {instantiated_model.getValue(CRef('default', 'Gain1', 'u'))}", flush=True)
print(f"info:    default.Gain1.y: {instantiated_model.getValue(CRef('default', 'Gain1', 'y'))}", flush=True)
print(f"info:    default.Add1.u1: {instantiated_model.getValue(CRef('default', 'Add1', 'u1'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()


## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/Gain.fmu
## |--   resources/mappedEntry1.ssm
## |--   |-- Parameter Mapping:
## |--   |-- |-- source: mapped_Gain_u
## |--   |-- |-- |-- targets: [('Gain1.u', LinearTransformation (factor: 2, offset: 10))]
## |--   |-- |-- source: mapped_Gain_k
## |--   |-- |-- |-- targets: [('Gain1.k', LinearTransformation (factor: 4, offset: 5))]
## |--   resources/mappedParameter1.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real mapped_Gain_u, 2.0, None, 'None')
## |--   |-- |-- (Real mapped_Gain_k, 2.0, None, 'None')
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Parameter Bindings: resources/mappedParameter1.ssv
## |-- |-- |-- |-- Parameter Mapping: resources/mappedEntry1.ssm
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- FMU: Gain1 'None'
## |-- |-- |-- |-- |-- path: resources/Gain.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- Connections:
## |-- |-- |-- |-- Gain1.y -> Add1.u1
## |-- UnitDefinitions:
## |-- |-- Unit: 1
## |-- |-- |-- BaseUnit:
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## info: After instantiation:
## info:    default.Gain1.k: 13.0
## info:    default.Gain1.u: 14.0
## info:    default.Gain1.y: 182.0
## info:    default.Add1.u1: 0.0
## info:    Result file: LinearTransformation3_res.mat (bufferSize=1)
## info: After simulation:
## info:    default.Gain1.k: 13.0
## info:    default.Gain1.u: 14.0
## info:    default.Gain1.y: 182.0
## info:    default.Add1.u1: 182.0
## endResult
