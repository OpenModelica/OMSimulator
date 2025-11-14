## status: correct
## teardown_command: rm -rf SimpleSimulation15_res.mat
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Connector, Causality, SignalType

model = SSP()
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
model.addComponent(CRef("default", "Add1"), "resources/Add.fmu")
model.addComponent(CRef("default", "Add3"), "resources/Add.fmu")

model.activeVariant.system.addConnector(Connector('input1', Causality.input, SignalType.Real))


model.addSystem(CRef("default", "sub-system"))
subsystem = model.activeVariant.system.elements[CRef("sub-system")]

model.addComponent(CRef("default", "sub-system", "Add2"), "resources/Add.fmu")


subsystem.addConnector(Connector(CRef("u"), Causality.input, SignalType.Real))
subsystem.addConnector(Connector(CRef("y"), Causality.output, SignalType.Real))

## System.input -> Element.input
subsystem.addConnection("", "u", "Add2", "u1")
## System.input -> Element.input
subsystem.addConnection("", "u", "Add2", "u2")
## Element.output -> System.output
subsystem.addConnection("", "y", "Add2", "y")

## Element.output -> Element.input
model.addConnection(CRef("default", "Add1", "y"), CRef("default", "sub-system", "u"))
## flip connection to test Element.output -> Element.input
model.addConnection(CRef("default", "Add3", "u1"), CRef("default", "sub-system", "y"))
## Element.output -> Element.input
model.addConnection(CRef("default", "sub-system", "y"), CRef("default", "Add3", "u2"))

## check failing connection and fix it
## model.addConnection(CRef("default", "input1"), CRef("default", "Add1", "y"))

model.list()
instantiated_model = model.instantiate()
#print(instantiated_model.dumpApiCalls())

instantiated_model.setResultFile("SimpleSimulation15_res.mat")
instantiated_model.setValue(CRef('default', 'Add1', 'u1'), 400.0)

instantiated_model.initialize()
instantiated_model.simulate()
print(f"info:    default.sub-system.u      : {instantiated_model.getValue(CRef('default', 'sub-system', 'u'))}", flush=True)
print(f"info:    default.sub-system.Add2.u1: {instantiated_model.getValue(CRef('default', 'sub-system', 'Add2', 'u1'))}", flush=True)
print(f"info:    default.sub-system.Add2.u2: {instantiated_model.getValue(CRef('default', 'sub-system', 'Add2', 'u2'))}", flush=True)
print(f"info:    default.sub-system.Add2.y : {instantiated_model.getValue(CRef('default', 'sub-system', 'Add2', 'y'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()

## Result:
## info:    Result file: SimpleSimulation15_res.mat (bufferSize=1)
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": None
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- FMU: Add3 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Elements:
## |-- |-- |-- |-- |-- |-- FMU: Add2 'None'
## |-- |-- |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Connections:
## |-- |-- |-- |-- |-- |-- .u -> Add2.u1
## |-- |-- |-- |-- |-- |-- .u -> Add2.u2
## |-- |-- |-- |-- |-- |-- Add2.y -> .y
## |-- |-- |-- Connections:
## |-- |-- |-- |-- Add1.y -> sub-system.u
## |-- |-- |-- |-- sub-system.y -> Add3.u1
## |-- |-- |-- |-- sub-system.y -> Add3.u2
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## info:    default.sub-system.u      : 400.0
## info:    default.sub-system.Add2.u1: 400.0
## info:    default.sub-system.Add2.u2: 400.0
## info:    default.sub-system.Add2.y : 800.0
## endResult
