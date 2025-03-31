## status: correct
## teardown_command: rm setValue2.ssp tmp-setValueSSV2/
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, SSV

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a component and
# set parameter values to ssv file and reference them in ssp file

model = SSP(temp_dir="./tmp-setValueSSV2/")
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='Add.fmu')
# root = model.activeVariant
component1 = model.addComponent(CRef('default', 'Add1'), 'Add.fmu')
component2 = model.addComponent(CRef('default', 'Add2'), 'Add.fmu')

ssv1 = SSV()
ssv1.setValue("k1", 2.0, "m")
ssv1.setValue("k2", 3.0, "kg")
ssv1.setValue("param3", "hello")
ssv1.export("myfile1.ssv")

ssv2 = SSV()
ssv2.setValue("k1", 200.0, "m")
ssv2.setValue("k2", 300.0, "kg")
ssv2.setValue("param3", "ssp")
ssv2.export("myfile2.ssv")

## add myfile1.ssv to to ssp resources
model.addResource("myfile1.ssv", "resources/myfile1.ssv")
## reference myfile1.ssv to Add1
model.addSSV(CRef('default', 'Add1'), 'resources/myfile1.ssv')

## add myfile2.ssv to to ssp resources
model.addResource("myfile2.ssv", "resources/myfile2.ssv")
## reference myfile2.ssv to Add2
model.addSSV(CRef('default', 'Add2'), 'resources/myfile2.ssv')

model.list()
model.export('setValue2.ssp')

model2 = SSP('setValue2.ssp')
model2.list()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   Add.fmu
## |--   resources/myfile1.ssv
## |--   resources/myfile2.ssv
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": None
## |-- |-- System: default
## |-- |-- Connectors:
## |-- |-- Elements:
## |-- |-- |-- FMU: (Add1)
## |-- |-- |-- |-- path: Add.fmu
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (u1, input, Real)
## |-- |-- |-- |-- (u2, input, Real)
## |-- |-- |-- |-- (y, output, Real)
## |-- |-- |-- |-- (k1, parameter, Real)
## |-- |-- |-- |-- (k2, parameter, Real)
## |-- |-- |-- Parameter Bindings: myfile1.ssv
## |-- |-- |-- |-- (Real k1, 2.0, None)
## |-- |-- |-- |-- (Real k2, 3.0, None)
## |-- |-- |-- |-- (String param3, hello, None)
## |-- |-- |-- FMU: (Add2)
## |-- |-- |-- |-- path: Add.fmu
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (u1, input, Real)
## |-- |-- |-- |-- (u2, input, Real)
## |-- |-- |-- |-- (y, output, Real)
## |-- |-- |-- |-- (k1, parameter, Real)
## |-- |-- |-- |-- (k2, parameter, Real)
## |-- |-- |-- Parameter Bindings: myfile2.ssv
## |-- |-- |-- |-- (Real k1, 200.0, None)
## |-- |-- |-- |-- (Real k2, 300.0, None)
## |-- |-- |-- |-- (String param3, ssp, None)
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   Add.fmu
## |--   resources/myfile1.ssv
## |--   resources/myfile2.ssv
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": <hidden>
## |-- |-- System: default
## |-- |-- Connectors:
## |-- |-- Elements:
## |-- |-- |-- FMU: (Add1)
## |-- |-- |-- |-- path: Add.fmu
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (u1, input, Real)
## |-- |-- |-- |-- (u2, input, Real)
## |-- |-- |-- |-- (y, output, Real)
## |-- |-- |-- |-- (k1, parameter, Real)
## |-- |-- |-- |-- (k2, parameter, Real)
## |-- |-- |-- Parameter Bindings: myfile1.ssv
## |-- |-- |-- |-- (Real k1, 2.0, None)
## |-- |-- |-- |-- (Real k2, 3.0, None)
## |-- |-- |-- |-- (String param3, hello, None)
## |-- |-- |-- FMU: (Add2)
## |-- |-- |-- |-- path: Add.fmu
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (u1, input, Real)
## |-- |-- |-- |-- (u2, input, Real)
## |-- |-- |-- |-- (y, output, Real)
## |-- |-- |-- |-- (k1, parameter, Real)
## |-- |-- |-- |-- (k2, parameter, Real)
## |-- |-- |-- Parameter Bindings: myfile2.ssv
## |-- |-- |-- |-- (Real k1, 200.0, None)
## |-- |-- |-- |-- (Real k2, 300.0, None)
## |-- |-- |-- |-- (String param3, ssp, None)
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
