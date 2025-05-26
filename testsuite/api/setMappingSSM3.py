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

# map top level parameters
model.mapParameter(CRef('default'), 'connector_param', 'param1')
model.mapParameter(CRef('default'), 'connector_param', 'param2')
model.mapParameter(CRef('default'), 'connector_param', 'param3')
model.mapParameter(CRef('default'), 'connector_input', 'input1')
model.mapParameter(CRef('default'), 'connector_input', 'input2')

model.addSystem(CRef('default', 'sub-system'))
## add top level sub-system connector
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('param2', Causality.parameter, SignalType.Real))
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('input2', Causality.input, SignalType.Real))

model.setValue(CRef('default', 'sub-system', 'connector_param'), 20.0)
model.setValue(CRef('default', 'sub-system', 'connector_input'), 30.0)
## map sub-system parameters
model.mapParameter(CRef('default', 'sub-system'), 'connector_param', 'param2')
model.mapParameter(CRef('default'), 'connector_input', 'input2')

model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
model.setValue(CRef("default", "Add1", "connector_param"), 40.0)
## map component parameters
model.mapParameter(CRef('default', 'Add1'), 'connector_param', 'k1')
model.mapParameter(CRef('default', 'Add1'), 'connector_param', 'k2')
model.list()


model.export('setMappingSSM3.ssp')

model2 = SSP('setMappingSSM3.ssp')


## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": None
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (param2, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (param3, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (input2, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- (Real connector_param, 2.0, None, 'None')
## |-- |-- |-- |-- (Real connector_input, 3.0, None, 'None')
## |-- |-- |-- |-- Inline Parameter Mapping:
## |-- |-- |-- |-- |-- source: connector_param
## |-- |-- |-- |-- |-- |-- targets: ['param1', 'param2', 'param3']
## |-- |-- |-- |-- |-- source: connector_input
## |-- |-- |-- |-- |-- |-- targets: ['input1', 'input2', 'input2']
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (param2, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (input2, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real connector_param, 20.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Real connector_input, 30.0, None, 'None')
## |-- |-- |-- |-- |-- |-- Inline Parameter Mapping:
## |-- |-- |-- |-- |-- |-- |-- source: connector_param
## |-- |-- |-- |-- |-- |-- |-- |-- targets: ['param2']
## |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real connector_param, 40.0, None, 'None')
## |-- |-- |-- |-- |-- |-- Inline Parameter Mapping:
## |-- |-- |-- |-- |-- |-- |-- source: connector_param
## |-- |-- |-- |-- |-- |-- |-- |-- targets: ['k1', 'k2']
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
