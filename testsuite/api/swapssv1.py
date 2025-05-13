## status: correct
## teardown_command: rm -rf swapssv1.ssp tmp-swapssv/ swap1.ssv swap2.ssv
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, SSV

Settings.suppressPath = True


model = SSP(temp_dir="./tmp-setValueSSV2/")
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')

component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
component2 = model.addComponent(CRef('default', 'Add2'), 'resources/Add.fmu')

ssv1 = SSV()
ssv1.setValue("k1", 2.0, "m")
ssv1.setValue("k2", 3.0, "kg")
ssv1.setValue("param3", "hello")
ssv1.export("swap1.ssv")

ssv2 = SSV()
ssv2.setValue("k1", 200.0, "m")
ssv2.setValue("k2", 300.0, "kg")
ssv2.setValue("param3", "ssp")
ssv2.export("swap2.ssv")

## add myfile1.ssv to to ssp resources
model.addResource("swap1.ssv", "resources/swap1.ssv")
## reference myfile1.ssv to Add1
model.addSSV(CRef('default', 'Add1'), 'resources/swap1.ssv')


## add myfile1.ssv to to ssp resources
model.addResource("swap2.ssv", "resources/swap2.ssv")
model.addSSV(CRef('default', 'Add2'), 'resources/swap2.ssv')

model.export('swapssv1.ssp')

model2 = SSP('swapssv1.ssp')
model2.list()

## remove ssv reference from Add1
model2.removeSSVReference(CRef('default', 'Add1'), 'resources/swap1.ssv')
print("After removing swap1.ssv from Add1")
print("==================================")

model2.list()

model2.addSSV(CRef('default', 'Add1'), 'resources/swap2.ssv')
print("After swapping swap2.ssv to Add1")
print("==============================")
model2.list()


## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/swap1.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 2.0, m, 'None')
## |--   |-- |-- (Real k2, 3.0, kg, 'None')
## |--   |-- |-- (String param3, hello, None, 'None')
## |--   resources/swap2.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 200.0, m, 'None')
## |--   |-- |-- (Real k2, 300.0, kg, 'None')
## |--   |-- |-- (String param3, ssp, None, 'None')
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/swap1.ssv
## |-- |-- |-- |-- FMU: Add2 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/swap2.ssv
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## After removing swap1.ssv from Add1
## ==================================
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/swap1.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 2.0, m, 'None')
## |--   |-- |-- (Real k2, 3.0, kg, 'None')
## |--   |-- |-- (String param3, hello, None, 'None')
## |--   resources/swap2.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 200.0, m, 'None')
## |--   |-- |-- (Real k2, 300.0, kg, 'None')
## |--   |-- |-- (String param3, ssp, None, 'None')
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- FMU: Add2 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/swap2.ssv
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## After swapping swap2.ssv to Add1
## ==============================
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/swap1.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 2.0, m, 'None')
## |--   |-- |-- (Real k2, 3.0, kg, 'None')
## |--   |-- |-- (String param3, hello, None, 'None')
## |--   resources/swap2.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 200.0, m, 'None')
## |--   |-- |-- (Real k2, 300.0, kg, 'None')
## |--   |-- |-- (String param3, ssp, None, 'None')
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/swap2.ssv
## |-- |-- |-- |-- FMU: Add2 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/swap2.ssv
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
