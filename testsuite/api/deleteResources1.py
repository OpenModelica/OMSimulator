## status: correct
## teardown_command: rm -rf deleteResources1.ssp tmp-deleteResources1/ delete3.ssv delete4.ssv
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, SSV

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a component and
# set parameter values to ssv file and reference them in ssp file and then delete the resources
# and references from the ssp file.
# It also demonstrates how to delete resources and references from an existing SSP file.

model = SSP()
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
# root = model.activeVariant
component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
component2 = model.addComponent(CRef('default', 'Add2'), 'resources/Add.fmu')

ssv1 = SSV()
ssv1.setValue("k1", 2.0, "m")
ssv1.setValue("k2", 3.0, "kg")
ssv1.setValue("param3", "hello")
ssv1.export("delete3.ssv")

ssv2 = SSV()
ssv2.setValue("k1", 200.0, "m")
ssv2.setValue("k2", 300.0, "kg")
ssv2.setValue("param3", "ssp")
ssv2.export("delete4.ssv")

## add delete1.ssv to to ssp resources
model.addResource("delete3.ssv", "resources/delete3.ssv")
## reference delete1.ssv to Add1
model.addSSVReference(CRef('default', 'Add1'), 'resources/delete3.ssv')

## add delete2.ssv to to ssp resources
model.addResource("delete4.ssv", "resources/delete4.ssv")
## reference delete2.ssv to Add2
model.addSSVReference(CRef('default', 'Add2'), 'resources/delete4.ssv')

#model.list()
model.export('deleteResources1.ssp')

model2 = SSP('deleteResources1.ssp')
model2.list()
print("list of Resources in SSP:")
print("==========================")
print(model2.listResource())
model2.deleteResource('resources/delete3.ssv')
print("After deleting delete3.ssv:")
print("==========================")
model2.list()
model2.deleteResource('resources/delete4.ssv')
print("After deleting delete4.ssv:")
print("==========================")
model2.list()
model2.deleteResource('resources/Add.fmu')
print("After deleting resources/Add.fmu:")
print("=================================")
model2.list()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/delete3.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 2.0, m, 'None')
## |--   |-- |-- (Real k2, 3.0, kg, 'None')
## |--   |-- |-- (String param3, hello, None, 'None')
## |--   resources/delete4.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 200.0, m, 'None')
## |--   |-- |-- (Real k2, 300.0, kg, 'None')
## |--   |-- |-- (String param3, ssp, None, 'None')
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
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
## |-- |-- |-- |-- |-- Parameter Bindings: resources/delete3.ssv
## |-- |-- |-- |-- FMU: Add2 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/delete4.ssv
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## list of Resources in SSP:
## ==========================
## ['resources/Add.fmu', 'resources/delete3.ssv', 'resources/delete4.ssv']
## After deleting delete3.ssv:
## ==========================
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/delete4.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 200.0, m, 'None')
## |--   |-- |-- (Real k2, 300.0, kg, 'None')
## |--   |-- |-- (String param3, ssp, None, 'None')
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
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
## |-- |-- |-- |-- |-- Parameter Bindings: resources/delete4.ssv
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## After deleting delete4.ssv:
## ==========================
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
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
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## After deleting resources/Add.fmu:
## =================================
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
