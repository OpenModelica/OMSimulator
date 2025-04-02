## status: correct
## teardown_command: rm -rf setValueSSV2.ssp tmp-setValueSSV2/ myfile3.ssv myfile4.ssv
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, SSV

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a component and
# set parameter values to ssv file and reference them in ssp file

model = SSP(temp_dir="./tmp-setValueSSV2/")
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
# root = model.activeVariant
component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
component2 = model.addComponent(CRef('default', 'Add2'), 'resources/Add.fmu')

ssv1 = SSV()
ssv1.setValue("k1", 2.0, "m")
ssv1.setValue("k2", 3.0, "kg")
ssv1.setValue("param3", "hello")
ssv1.export("myfile3.ssv")

ssv2 = SSV()
ssv2.setValue("k1", 200.0, "m")
ssv2.setValue("k2", 300.0, "kg")
ssv2.setValue("param3", "ssp")
ssv2.export("myfile4.ssv")

## add myfile1.ssv to to ssp resources
model.addResource("myfile3.ssv", "resources/myfile3.ssv")
## reference myfile1.ssv to Add1
model.addSSV(CRef('default', 'Add1'), 'resources/myfile3.ssv')

## add myfile2.ssv to to ssp resources
model.addResource("myfile4.ssv", "resources/myfile4.ssv")
## reference myfile2.ssv to Add2
model.addSSV(CRef('default', 'Add2'), 'resources/myfile4.ssv')

model.list()
model.export('setValueSSV2.ssp')

model2 = SSP('setValueSSV2.ssp')
model2.list()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/myfile3.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 2.0, m)
## |--   |-- |-- (Real k2, 3.0, kg)
## |--   |-- |-- (String param3, hello, None)
## |--   resources/myfile4.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 200.0, m)
## |--   |-- |-- (Real k2, 300.0, kg)
## |--   |-- |-- (String param3, ssp, None)
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": None
## |-- |-- System: default
## |-- |-- Connectors:
## |-- |-- Elements:
## |-- |-- |-- FMU: (Add1)
## |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real)
## |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real)
## |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real)
## |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real)
## |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real)
## |-- |-- |-- |-- Parameter Bindings: resources/myfile3.ssv
## |-- |-- |-- FMU: (Add2)
## |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real)
## |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real)
## |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real)
## |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real)
## |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real)
## |-- |-- |-- |-- Parameter Bindings: resources/myfile4.ssv
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/myfile3.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 2.0, m)
## |--   |-- |-- (Real k2, 3.0, kg)
## |--   |-- |-- (String param3, hello, None)
## |--   resources/myfile4.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 200.0, m)
## |--   |-- |-- (Real k2, 300.0, kg)
## |--   |-- |-- (String param3, ssp, None)
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": <hidden>
## |-- |-- System: default
## |-- |-- Connectors:
## |-- |-- Elements:
## |-- |-- |-- FMU: (Add1)
## |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real)
## |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real)
## |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real)
## |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real)
## |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real)
## |-- |-- |-- |-- Parameter Bindings: resources/myfile3.ssv
## |-- |-- |-- FMU: (Add2)
## |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real)
## |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real)
## |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real)
## |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real)
## |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real)
## |-- |-- |-- |-- Parameter Bindings: resources/myfile4.ssv
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
