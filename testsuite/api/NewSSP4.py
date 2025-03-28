## status: correct
## teardown_command: rm -rf NewSSP4.ssp /
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a component and
# set parameter values on the components

model = SSP()
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='Add.fmu')
root = model.activeVariant
component1 = model.addComponent(CRef('default', 'Add1'), 'Add.fmu')
component1.setValue("k1", 2.0)
component1.setValue("k2", 3.0)
component2 = model.addComponent(CRef('default', 'Add2'), 'Add.fmu')
component2.setValue("k1", 100.0)
component2.setValue("k2", 300.0)


# component1.setValue("param1", 2.0)
# model.setValue(CRef('default', "Add1", "param1"), 2.0)
# root.setValue(("Add1", "param1"), 2.0)

# ssv = SSV()
# ssv.setValue("param1", 3.0)
# #ssv.export("myfile.ssv")
# model.addResource(ssv, "resources/myfile.ssv")
# model.addSSV(CRef('default', 'Add2'), 'resources/myfile.ssv')



model.list()
model.export('NewSSP4.ssp')

model2 = SSP('NewSSP4.ssp')
model2.list()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   Add.fmu
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": None
## |-- |-- System: default
## |-- |-- Connectors:
## |-- |-- Elements:
## |-- |-- |-- FMU: (Add1)
## |-- |-- |--   path: Add.fmu
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (u1, input, Real)
## |-- |-- |-- |-- (u2, input, Real)
## |-- |-- |-- |-- (y, output, Real)
## |-- |-- |-- |-- (k1, parameter, Real)
## |-- |-- |-- |-- (k2, parameter, Real)
## |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- (k1, 2.0, Real)
## |-- |-- |-- |-- (k2, 3.0, Real)
## |-- |-- |-- FMU: (Add2)
## |-- |-- |--   path: Add.fmu
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (u1, input, Real)
## |-- |-- |-- |-- (u2, input, Real)
## |-- |-- |-- |-- (y, output, Real)
## |-- |-- |-- |-- (k1, parameter, Real)
## |-- |-- |-- |-- (k2, parameter, Real)
## |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- (k1, 100.0, Real)
## |-- |-- |-- |-- (k2, 300.0, Real)
## |-- DefaultExperiment
## |--   startTime: 0.0
## |--   stopTime: 1.0
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   Add.fmu
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": <hidden>
## |-- |-- System: default
## |-- |-- Connectors:
## |-- |-- Elements:
## |-- |-- |-- FMU: (Add1)
## |-- |-- |--   path: Add.fmu
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (u1, input, Real)
## |-- |-- |-- |-- (u2, input, Real)
## |-- |-- |-- |-- (y, output, Real)
## |-- |-- |-- |-- (k1, parameter, Real)
## |-- |-- |-- |-- (k2, parameter, Real)
## |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- (k1, 2.0, Real)
## |-- |-- |-- |-- (k2, 3.0, Real)
## |-- |-- |-- FMU: (Add2)
## |-- |-- |--   path: Add.fmu
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (u1, input, Real)
## |-- |-- |-- |-- (u2, input, Real)
## |-- |-- |-- |-- (y, output, Real)
## |-- |-- |-- |-- (k1, parameter, Real)
## |-- |-- |-- |-- (k2, parameter, Real)
## |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- (k1, 100.0, Real)
## |-- |-- |-- |-- (k2, 300.0, Real)
## |-- DefaultExperiment
## |--   startTime: 0.0
## |--   stopTime: 1.0
## endResult
