## status: correct
## teardown_command: rm -rf setMappingSSM2.ssp tmp-setMappingSSM2/ myfile5.ssv myfile5.ssm
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
ssv1.export("myfile5.ssv")

ssm = SSM()
ssm.map("connector_param", "param1")
ssm.map("connector_param", "param2")
ssm.map("connector_param", "param3")
ssm.map("connector_input", "input1")
ssm.map("connector_input", "input2")
ssm.export("myfile5.ssm")





## add myfile5.ssv to to ssp resources
model.addResource("myfile5.ssv", "resources/myfile5.ssv")
## add myfile5.ssm to to ssp resources
model.addResource("myfile5.ssm", "resources/myfile5.ssm")


model.addSSVReference(CRef('default'), 'resources/myfile5.ssv')

# model.addSSVReference(CRef('default', 'sub-system'), 'resources/myfile6.ssv', 'resources/myfile2.ssm')

model.export('setMappingSSM2.ssp')

model2 = SSP('setMappingSSM2.ssp')
model2.list()


## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/myfile5.ssm
## |--   |-- Parameter Mapping:
## |--   |-- |-- source: connector_param
## |--   |-- |-- |-- target: param1
## |--   |-- |-- |-- target: param2
## |--   |-- |-- |-- target: param3
## |--   |-- |-- source: connector_input
## |--   |-- |-- |-- target: input1
## |--   |-- |-- |-- target: input2
## |--   resources/myfile5.ssv
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
## |-- |-- |-- Parameter Bindings: resources/myfile5.ssv
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (param2, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (input2, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
