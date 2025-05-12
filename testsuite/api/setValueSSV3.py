## status: correct
## teardown_command: rm -rf setValueSSV3.ssp tmp-setValueSSV3/ myfile3.ssv myfile4.ssv myfile5.ssv
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, SSV, Connector, Causality, SignalType

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a component and
# set parameter values to ssv file and reference multiple ssv files to same component

model = SSP(temp_dir="./tmp-setValueSSV2/")

## add top level system connector
model.activeVariant.system.addConnector(Connector('param1', Causality.parameter, SignalType.Real))
model.activeVariant.system.addConnector(Connector('input1', Causality.input, SignalType.Real))

model.addSystem(CRef('default', 'sub-system'))
## add top level sub-system connector
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('param2', Causality.parameter, SignalType.Real))
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('input2', Causality.input, SignalType.Real))

model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
# root = model.activeVariant
component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
component2 = model.addComponent(CRef('default', 'Add2'), 'resources/Add.fmu')


ssv1 = SSV()
ssv1.setValue("param1", 2.0)
ssv1.setValue("input1", 3.0)
ssv1.export("myfile3.ssv")

ssv2 = SSV()
ssv2.setValue("k1", 2.0, "m")
ssv2.setValue("k2", 3.0, "kg")
ssv2.setValue("param3", "hello")
ssv2.export("myfile4.ssv")

ssv3 = SSV()
ssv3.setValue("k1", 200.0, "m")
ssv3.setValue("k2", 300.0, "kg")
ssv3.setValue("param3", "ssp")
ssv3.export("myfile5.ssv")


## add myfile3.ssv to to ssp resources
model.addResource("myfile3.ssv", "resources/myfile3.ssv")
## add myfile4.ssv to to ssp resources
model.addResource("myfile4.ssv", "resources/myfile4.ssv")
## add myfile5.ssv to to ssp resources
model.addResource("myfile4.ssv", "resources/myfile5.ssv")

model.addSSV(CRef('default'), 'resources/myfile3.ssv')
model.addSSV(CRef('default'), 'resources/myfile4.ssv')

model.addSSV(CRef('default', 'sub-system'), 'resources/myfile4.ssv')

## reference myfile1.ssv to Add1
model.addSSV(CRef('default', 'Add1'), 'resources/myfile4.ssv')
## reference myfile1.ssv to Add1
model.addSSV(CRef('default', 'Add1'), 'resources/myfile5.ssv')

model.export('setValueSSV3.ssp')

model2 = SSP('setValueSSV3.ssp')
model2.list()


## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/myfile3.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real param1, 2.0, None, 'None')
## |--   |-- |-- (Real input1, 3.0, None, 'None')
## |--   resources/myfile4.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 2.0, m, 'None')
## |--   |-- |-- (Real k2, 3.0, kg, 'None')
## |--   |-- |-- (String param3, hello, None, 'None')
## |--   resources/myfile5.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 2.0, m, 'None')
## |--   |-- |-- (Real k2, 3.0, kg, 'None')
## |--   |-- |-- (String param3, hello, None, 'None')
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- Parameter Bindings: resources/myfile3.ssv
## |-- |-- |-- Parameter Bindings: resources/myfile4.ssv
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (param2, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (input2, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/myfile4.ssv
## |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/myfile4.ssv
## |-- |-- |-- |-- |-- Parameter Bindings: resources/myfile5.ssv
## |-- |-- |-- |-- FMU: Add2 'None'
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
