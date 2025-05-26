## status: correct
## teardown_command: rm -rf setMappingSSM3.ssp
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, SSV, SSM, Connector, Causality, SignalType

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a component and
# set temporary parameter value and maps the parameter to a connector or fmu thorugh parameter mapping inline

model = SSP(temp_dir="./tmp-setMappingSSM3/")

## add top level system connector
model.activeVariant.system.addConnector(Connector('param1', Causality.parameter, SignalType.Real))
model.activeVariant.system.addConnector(Connector('param2', Causality.parameter, SignalType.Real))
model.activeVariant.system.addConnector(Connector('param3', Causality.parameter, SignalType.Real))

model.activeVariant.system.addConnector(Connector('input1', Causality.input, SignalType.Real))
model.activeVariant.system.addConnector(Connector('input2', Causality.input, SignalType.Real))

model.setValue(CRef('default','connector_param'), 2.0)
model.setValue(CRef('default','connector_input'), 3.0)

model.addSystem(CRef('default', 'sub-system'))
## add top level sub-system connector
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('param2', Causality.parameter, SignalType.Real))
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('input2', Causality.input, SignalType.Real))

model.setValue(CRef('default', 'sub-system', 'connector_param_1'), 20.0)
model.setValue(CRef('default', 'sub-system', 'connector_input_2'), 30.0)

model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
# root = model.activeVariant
component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')


model.export('setMappingSSM3.ssp')

model2 = SSP('setMappingSSM3.ssp')
model2.list()


## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/mappingssm2.ssm
## |--   |-- Parameter Mapping:
## |--   |-- |-- source: connector_param
## |--   |-- |-- |-- target: param1
## |--   |-- |-- |-- target: param2
## |--   |-- |-- |-- target: param3
## |--   |-- |-- source: connector_input
## |--   |-- |-- |-- target: input1
## |--   |-- |-- |-- target: input2
## |--   resources/mappingssm2.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real connector_param, 2.0, None, 'None')
## |--   |-- |-- (Real connector_input, 3.0, None, 'None')
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (param2, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (param3, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (input2, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- Parameter Bindings: resources/mappingssm2.ssv
## |-- |-- |-- |-- Parameter Mapping: resources/mappingssm2.ssm
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (param2, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (input2, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/mappingssm2.ssv
## |-- |-- |-- |-- |-- |-- Parameter Mapping: resources/mappingssm2.ssm
## |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/mappingssm2.ssv
## |-- |-- |-- |-- |-- |-- Parameter Mapping: resources/mappingssm2.ssm
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
