## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: no

from OMSimulator import SSP, CRef, Settings, Connector, Causality, SignalType

Settings.suppressPath = True


model = SSP()
## add top level system connector
model.activeVariant.system.addConnector(Connector('input1', Causality.input, SignalType.Real))
model.addResource('../resources/Modelica.Blocks.Math.Add3.fmu', new_name='resources/Add.fmu')

## add subsystem
model.addSystem(CRef('default', 'sub-system'))
model.addComponent(CRef('default', 'sub-system', 'Add'), 'resources/Add.fmu')

## add top level sub-system connector
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('input', Causality.input, SignalType.Real))


model.setValue(CRef('default','input1'), 300.0)

model.addConnection(CRef('default', 'input1'), CRef('default', 'sub-system', 'input'))
model.addConnection(CRef('default', 'sub-system', 'input'), CRef('default', 'sub-system', 'Add', 'u1'))
model.list()
instantiated_model = model.instantiate() ## internally generate the json file and also set the model state like virgin,
#print(instantiated_model.dumpApiCalls(), flush=True)
instantiated_model.setResultFile("SimpleSimulation18_res.mat")
#instantiated_model.setValue(CRef('default','input1'), 400.0)
print(f"info: After instantiation:")
print(f"info:    default.input1 : {instantiated_model.getValue(CRef('default', 'input1'))}", flush=True)
print(f"info:    default.sub-system.input: {instantiated_model.getValue(CRef('default', 'sub-system', 'input'))}", flush=True)
print(f"info:    default.sub-system.Add.u1: {instantiated_model.getValue(CRef('default', 'sub-system', 'Add', 'u1'))}", flush=True)


instantiated_model.initialize()
instantiated_model.simulate()
print(f"info: After simulation:")
print(f"info:    default.input1 : {instantiated_model.getValue(CRef('default', 'input1'))}", flush=True)
print(f"info:    default.sub-system.input: {instantiated_model.getValue(CRef('default', 'sub-system', 'input'))}", flush=True)
print(f"info:    default.sub-system.Add.u1: {instantiated_model.getValue(CRef('default', 'sub-system', 'Add', 'u1'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": None
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- (Real input1, 300.0, None, 'None')
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (input, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Elements:
## |-- |-- |-- |-- |-- |-- FMU: Add 'None'
## |-- |-- |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- |-- |-- (u3, Causality.input, SignalType.Real, None, 'Connector of Real input signal 3')
## |-- |-- |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- |-- |-- |-- (k3, Causality.parameter, SignalType.Real, None, 'Gain of input signal 3')
## |-- |-- |-- |-- |-- Connections:
## |-- |-- |-- |-- |-- |-- .input -> Add.u1
## |-- |-- |-- Connections:
## |-- |-- |-- |-- .input1 -> sub-system.input
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## info: After instantiation:
## info:    default.input1 : 300.0
## info:    default.sub-system.input: 300.0
## info:    default.sub-system.Add.u1: 0.0
## info:    Result file: SimpleSimulation18_res.mat (bufferSize=10)
## info: After simulation:
## info:    default.input1 : 300.0
## info:    default.sub-system.input: 300.0
## info:    default.sub-system.Add.u1: 300.0
## endResult
