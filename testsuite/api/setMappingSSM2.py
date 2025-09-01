## status: correct
## teardown_command: rm -rf setMappingSSM2.ssp tmp-setMappingSSM2/ mappingssm2.ssv mappingssm3.ssv mappingssm2.ssm
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, SSV, SSM, Connector, Causality, SignalType

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a component and
# set parameter values to ssv file and map some parameters to ssm file

model = SSP(temp_dir="./tmp-setMappingSSM2/")

## add top level system connector
model.activeVariant.system.addConnector(Connector('param1', Causality.parameter, SignalType.Real))
model.activeVariant.system.addConnector(Connector('param2', Causality.parameter, SignalType.Real))
model.activeVariant.system.addConnector(Connector('param3', Causality.parameter, SignalType.Real))

model.activeVariant.system.addConnector(Connector('input1', Causality.input, SignalType.Real))
model.activeVariant.system.addConnector(Connector('input2', Causality.input, SignalType.Real))

model.addSystem(CRef('default', 'sub-system'))
## add top level sub-system connector
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('param2', Causality.parameter, SignalType.Real))
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('input2', Causality.input, SignalType.Real))

model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
# root = model.activeVariant
component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')

## set top level parameter values which will be used in all components with parameter mapping
ssv1 = SSV()
ssv1.setValue("connector_param", 2.0)
ssv1.setValue("connector_input", 3.0)
ssv1.export("mappingssm2.ssv")

## set top level parameter values which will be used in all components with parameter mapping
ssv2 = SSV()
ssv2.setValue("connector_param", 4.0)
ssv2.setValue("connector_input", 5.0)
ssv2.export("mappingssm3.ssv")

ssm = SSM()
ssm.mapParameter("connector_param", "param1")
ssm.mapParameter("connector_param", "param2")
ssm.mapParameter("connector_param", "param3")
ssm.mapParameter("connector_input", "input1")
ssm.mapParameter("connector_input", "input2")
ssm.export("mappingssm2.ssm")


## add mappingssm2.ssv to to ssp resources
model.addResource("mappingssm2.ssv", "resources/mappingssm2.ssv")
## add mappingssm3.ssv to to ssp resources
model.addResource("mappingssm3.ssv", "resources/mappingssm3.ssv")
## add mappingssm2.ssm to to ssp resources
model.addResource("mappingssm2.ssm", "resources/mappingssm2.ssm")

## add parameter mapping references to all components
model.addSSVReference(CRef('default'), 'resources/mappingssm2.ssv', 'resources/mappingssm2.ssm')
model.addSSVReference(CRef('default'), 'resources/mappingssm3.ssv', 'resources/mappingssm2.ssm')

model.addSSVReference(CRef('default', 'sub-system'), 'resources/mappingssm2.ssv', 'resources/mappingssm2.ssm')
model.addSSVReference(CRef('default', 'sub-system'), 'resources/mappingssm3.ssv', 'resources/mappingssm2.ssm')

model.addSSVReference(CRef('default', 'Add1'), 'resources/mappingssm2.ssv', 'resources/mappingssm2.ssm')
model.addSSVReference(CRef('default', 'Add1'), 'resources/mappingssm3.ssv', 'resources/mappingssm2.ssm')

model.export('setMappingSSM2.ssp')

model2 = SSP('setMappingSSM2.ssp')
model2.list()


## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/mappingssm2.ssm
## |--   |-- Parameter Mapping:
## |--   |-- |-- source: connector_param
## |--   |-- |-- |-- targets: ['param1', 'param2', 'param3']
## |--   |-- |-- source: connector_input
## |--   |-- |-- |-- targets: ['input1', 'input2']
## |--   resources/mappingssm2.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real connector_param, 2.0, None, 'None')
## |--   |-- |-- (Real connector_input, 3.0, None, 'None')
## |--   resources/mappingssm3.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real connector_param, 4.0, None, 'None')
## |--   |-- |-- (Real connector_input, 5.0, None, 'None')
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
## |-- |-- |-- Parameter Bindings: resources/mappingssm3.ssv
## |-- |-- |-- |-- Parameter Mapping: resources/mappingssm2.ssm
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (param2, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (input2, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/mappingssm2.ssv
## |-- |-- |-- |-- |-- |-- Parameter Mapping: resources/mappingssm2.ssm
## |-- |-- |-- |-- |-- Parameter Bindings: resources/mappingssm3.ssv
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
## |-- |-- |-- |-- |-- Parameter Bindings: resources/mappingssm3.ssv
## |-- |-- |-- |-- |-- |-- Parameter Mapping: resources/mappingssm2.ssm
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
