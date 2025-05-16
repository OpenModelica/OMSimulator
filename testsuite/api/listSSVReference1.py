## status: correct
## teardown_command: rm -rf listSSVReference1.ssp tmp-listssvreferece1/ list1.ssv list2.ssv
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, SSV

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a components Add1, Add2 and Add3
# and set parameter values to ssv file and reference them. It add test to swap ssv reference from list1.ssv
# to the same component Add1 and Add3 with list2.ssv.


model = SSP(temp_dir="./tmp-listssvreference1/")
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')

## add subsystem
model.addSystem(CRef('default', 'sub-system'))

component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
component2 = model.addComponent(CRef('default', 'Add2'), 'resources/Add.fmu')

model.addComponent(CRef('default', 'sub-system', 'Add3'), 'resources/Add.fmu')

ssv1 = SSV()
ssv1.setValue("k1", 2.0, "m")
ssv1.setValue("k2", 3.0, "kg")
ssv1.setValue("param3", "hello")
ssv1.export("list1.ssv")

ssv2 = SSV()
ssv2.setValue("k1", 200.0, "m")
ssv2.setValue("k2", 300.0, "kg")
ssv2.setValue("param3", "ssp")
ssv2.export("list2.ssv")

## add list1.ssv to to ssp resources
model.addResource("list1.ssv", "resources/list1.ssv")
## add list2.ssv to to ssp resources
model.addResource("list2.ssv", "resources/list2.ssv")

## reference list1.ssv to top level system
model.addSSVReference(CRef('default'), 'resources/list1.ssv')
## reference list2.ssv to top level sub-system
model.addSSVReference(CRef('default','sub-system'), 'resources/list2.ssv')

## reference list1.ssv and list2.ssv to Add1
model.addSSVReference(CRef('default', 'Add1'), 'resources/list1.ssv')
model.addSSVReference(CRef('default', 'Add1'), 'resources/list2.ssv')

## reference list2.ssv to Add2
model.addSSVReference(CRef('default', 'Add2'), 'resources/list2.ssv')

## reference list1.ssv to Add3
model.addSSVReference(CRef('default', 'sub-system', 'Add3'), 'resources/list1.ssv')

model.export('listSSVReference1.ssp')

model2 = SSP('listSSVReference1.ssp')
model2.list()

print("List of SSV references")
print("=======================")
print(f"Top level system SSV resources    : {model2.listSSVReference(CRef('default'))}")
print(f"Top level sub-system SSV resources: {model2.listSSVReference(CRef('default', 'sub-system'))}")
print(f"Component Add1 SSV resources      : {model2.listSSVReference(CRef('default', 'Add1'))}")
print(f"Component Add2 SSV resources      : {model2.listSSVReference(CRef('default', 'Add2'))}")
print(f"Component Add3 SSV resources      : {model2.listSSVReference(CRef('default', 'sub-system', 'Add3'))}")


## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/list1.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 2.0, m, 'None')
## |--   |-- |-- (Real k2, 3.0, kg, 'None')
## |--   |-- |-- (String param3, hello, None, 'None')
## |--   resources/list2.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k1, 200.0, m, 'None')
## |--   |-- |-- (Real k2, 300.0, kg, 'None')
## |--   |-- |-- (String param3, ssp, None, 'None')
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Parameter Bindings: resources/list1.ssv
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- Parameter Bindings: resources/list2.ssv
## |-- |-- |-- |-- |-- Elements:
## |-- |-- |-- |-- |-- |-- FMU: Add3 'None'
## |-- |-- |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- |-- |-- Parameter Bindings: resources/list1.ssv
## |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/list1.ssv
## |-- |-- |-- |-- |-- Parameter Bindings: resources/list2.ssv
## |-- |-- |-- |-- FMU: Add2 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/list2.ssv
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## List of SSV references
## =======================
## Top level system SSV resources    : ['resources/list1.ssv']
## Top level sub-system SSV resources: ['resources/list2.ssv']
## Component Add1 SSV resources      : ['resources/list1.ssv', 'resources/list2.ssv']
## Component Add2 SSV resources      : ['resources/list2.ssv']
## Component Add3 SSV resources      : ['resources/list1.ssv']
## endResult
