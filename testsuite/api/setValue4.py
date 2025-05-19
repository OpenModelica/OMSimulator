## status: correct
## teardown_command: rm setValue4.ssp
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, Connector, Causality, SignalType

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a component and
# set parameter values from the root level

model = SSP()

## add top level system connector
model.activeVariant.system.addConnector(Connector('param1', Causality.parameter, SignalType.Real))
model.activeVariant.system.addConnector(Connector('input1', Causality.input, SignalType.Real))

model.setValue(CRef('default','param1'), 200.0)
model.setValue(CRef('default','input1'), 300.0)

model.addSystem(CRef('default', 'sub-system'))
## add top level sub-system connector
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('param2', Causality.parameter, SignalType.Real))
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('input2', Causality.input, SignalType.Real))
model.setValue(CRef('default', 'sub-system', 'param2'), 400.0)
model.setValue(CRef('default', 'sub-system', 'input2'), 500.0)

model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='Add.fmu')
component1 = model.addComponent(CRef('default', 'Add1'), 'Add.fmu')
component2 = model.addComponent(CRef('default', 'Add2'), 'Add.fmu')


model.setValue(CRef("default", "Add1", "k1"), 2.0)
model.setValue(CRef("default", "Add1", "k2"), 3.0)

model.setValue(CRef("default", "Add2", "k1"), 100.0)
model.setValue(CRef("default", "Add2", "k2"), 300.0)


#model.list()
model.export('setValue4.ssp')

model2 = SSP('setValue4.ssp')
model2.list()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   Add.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- (Real param1, 200.0, None, 'None')
## |-- |-- |-- |-- (Real input1, 300.0, None, 'None')
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (param2, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (input2, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real param2, 400.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Real input2, 500.0, None, 'None')
## |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- path: Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real k1, 2.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Real k2, 3.0, None, 'None')
## |-- |-- |-- |-- FMU: Add2 'None'
## |-- |-- |-- |-- |-- path: Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real k1, 100.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Real k2, 300.0, None, 'None')
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
