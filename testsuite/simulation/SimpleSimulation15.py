## status: correct
## teardown_command: rm -rf SimpleSimulation15_res.mat
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Connector, Connection, Causality, SignalType

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

subsystem.addConnection("", "u", "Add2", "u1")
subsystem.addConnection("", "u", "Add2", "u2")
subsystem.addConnection("", "y", "Add2", "y")


model.addConnection(CRef("default", "Add1", "y"), CRef("default", "sub-system", "u"))
model.addConnection(CRef("default", "sub-system", "y"), CRef("default", "Add3", "u1"))
model.addConnection(CRef("default", "sub-system", "y"), CRef("default", "Add3", "u2"))

## check failing connection and fix it
# model.addConnection(CRef("default", "input1"), CRef("default", "Add1", "y"))

model.list()
instantiated_model = model.instantiate()
#print(instantiated_model.dumpApiCalls())

instantiated_model.setResultFile("SimpleSimulation15_res.mat")
instantiated_model.setValue(CRef('default', 'sub-system', 'u'), 400.0)

instantiated_model.initialize()
instantiated_model.simulate()
print(f"info:    default.sub-system.u: {instantiated_model.getValue(CRef('default', 'sub-system', 'u'))}", flush=True)
print(f"info:    default.sub-system.Add2.u1: {instantiated_model.getValue(CRef('default', 'sub-system', 'Add2', 'u1'))}", flush=True)
print(f"info:    default.sub-system.Add2.u2: {instantiated_model.getValue(CRef('default', 'sub-system', 'Add2', 'u2'))}", flush=True)
print(f"info:    default.sub-system.Add2.y : {instantiated_model.getValue(CRef('default', 'sub-system', 'Add2', 'y'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()

## Result:
## info:    model doesn't contain any continuous state
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/Gain.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- (Real mapped_param1, 200.0, None, 'None')
## |-- |-- |-- |-- (Real mapped_input1, 300.0, None, 'None')
## |-- |-- |-- |-- Inline Parameter Mapping:
## |-- |-- |-- |-- |-- source: mapped_param1
## |-- |-- |-- |-- |-- |-- targets: ['param1']
## |-- |-- |-- |-- |-- source: mapped_input1
## |-- |-- |-- |-- |-- |-- targets: ['input1']
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (input, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real mapped_sub_system_input, 400.0, None, 'None')
## |-- |-- |-- |-- |-- |-- Inline Parameter Mapping:
## |-- |-- |-- |-- |-- |-- |-- source: mapped_sub_system_input
## |-- |-- |-- |-- |-- |-- |-- |-- targets: ['input']
## |-- |-- |-- |-- |-- Elements:
## |-- |-- |-- |-- |-- |-- FMU: Gain2 'None'
## |-- |-- |-- |-- |-- |-- |-- path: resources/Gain.fmu
## |-- |-- |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- |-- |-- (Real mapped_sub_system_Gain2_k, 500.0, None, 'None')
## |-- |-- |-- |-- |-- |-- |-- |-- Inline Parameter Mapping:
## |-- |-- |-- |-- |-- |-- |-- |-- |-- source: mapped_sub_system_Gain2_k
## |-- |-- |-- |-- |-- |-- |-- |-- |-- |-- targets: ['k']
## |-- |-- |-- |-- |-- |-- |-- |-- UnitDefinitions:
## |-- |-- |-- |-- |-- |-- |-- |-- |-- Unit: 1
## |-- |-- |-- |-- |-- |-- |-- |-- |-- |-- BaseUnit:
## |-- |-- |-- |-- |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- |-- |-- |-- |-- name: solver2
## |-- |-- |-- |-- |-- Connections:
## |-- |-- |-- |-- |-- |-- .input -> Gain2.u
## |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- FMU: Gain1 'None'
## |-- |-- |-- |-- |-- path: resources/Gain.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real mapped_Gain_k, 2.0, None, 'None')
## |-- |-- |-- |-- |-- |-- Inline Parameter Mapping:
## |-- |-- |-- |-- |-- |-- |-- source: mapped_Gain_k
## |-- |-- |-- |-- |-- |-- |-- |-- targets: ['k']
## |-- |-- |-- |-- |-- |-- UnitDefinitions:
## |-- |-- |-- |-- |-- |-- |-- Unit: 1
## |-- |-- |-- |-- |-- |-- |-- |-- BaseUnit:
## |-- |-- |-- Connections:
## |-- |-- |-- |-- .input1 -> Gain1.u
## |-- |-- |-- |-- Gain1.y -> Add1.u1
## |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- (name=solver2, method=cvode, tolerance=0.0001)
## |-- UnitDefinitions:
## |-- |-- Unit: 1
## |-- |-- |-- BaseUnit:
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## info: After instantiation:
## info:    default.param1 : 200.0
## info:    default.input1 : 300.0
## info:    default.Gain1.k: 2.0
## info:    default.Gain1.u: 0.0
## info:    default.Gain1.y: 0.0
## info:    default.Add1.u1: 0.0
## info:    default.sub-system.input: 400.0
## info:    default.sub-system.Gain2.k: 500.0
## info:    default.sub-system.Gain2.u: 0.0
## info:    default.sub-system.Gain2.y: 0.0
## info:    maximum step size for 'model.root': 0.001000
## info:    Result file: SimpleSimulation14_res.mat (bufferSize=1)
## info: After simulation:
## info:    default.param1 : 200.0
## info:    default.input1 : 300.0
## info:    default.Gain1.k: 2.0
## info:    default.Gain1.u: 300.0
## info:    default.Gain1.y: 600.0
## info:    default.Add1.u1: 600.0
## info:    default.sub-system.input: 400.0
## info:    default.sub-system.Gain2.k: 500.0
## info:    default.sub-system.Gain2.u: 400.0
## info:    default.sub-system.Gain2.y: 200000.0
## info:    Final Statistics for 'model.root':
##          NumSteps = 1001 NumRhsEvals  = 1002 NumLinSolvSetups = 51
##          NumNonlinSolvIters = 1001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## endResult
