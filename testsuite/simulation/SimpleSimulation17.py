## status: correct
## teardown_command: rm -rf SimpleSimulation17.ssp SimpleSimulation17_res.mat
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, Connector, Causality, SignalType

Settings.suppressPath = True


# This example creates a new SSP file with only top level system connectors and a sub-system with a connector.
# It then exports the SSP file and re-imports and simulates the model. The example also shows how to set and get values of top level system connectors and sub-system connectors.

model = SSP()
## add top level system connector
model.activeVariant.system.addConnector(Connector('input1', Causality.input, SignalType.Real))
model.setValue(CRef('default', 'input1'), 100.0)
## add subsystem
model.addSystem(CRef('default', 'sub-system'))
## add top level sub-system connector
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('input2', Causality.input, SignalType.Real))
model.setValue(CRef('default', 'sub-system', 'input2'), 200.0)

model.export('SimpleSimulation17.ssp')

model2 = SSP('SimpleSimulation17.ssp')
model2.list()
instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,
instantiated_model.setResultFile("SimpleSimulation17_res.mat")
print(f"info: After instantiation:")
print(f"info:    default.input1           : {instantiated_model.getValue(CRef('default', 'input1'))}", flush=True)
print(f"info:    default.sub-system.input2: {instantiated_model.getValue(CRef('default', 'sub-system', 'input2'))}", flush=True)
instantiated_model.initialize()
instantiated_model.simulate()
print(f"info: After simulation:")
print(f"info:    default.input1           : {instantiated_model.getValue(CRef('default', 'input1'))}", flush=True)
print(f"info:    default.sub-system.input2: {instantiated_model.getValue(CRef('default', 'sub-system', 'input2'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- (Real input1, 100.0, None, 'None')
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (input2, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real input2, 200.0, None, 'None')
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## info: After instantiation:
## info:    default.input1           : 100.0
## info:    default.sub-system.input2: 200.0
## info:    Result file: SimpleSimulation17_res.mat (bufferSize=10)
## info: After simulation:
## info:    default.input1           : 100.0
## info:    default.sub-system.input2: 200.0
## endResult
