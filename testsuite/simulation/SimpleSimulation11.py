## status: correct
## teardown_command: rm -rf SimpleSimulation11.ssp SimpleSimulation11_res.mat mappedParameter1.ssv mappedEntry1.ssm
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, Connector, Causality, SignalType, SSV, SSM

Settings.suppressPath = True


# This example creates a new SSP file with an FMU instantiated as a component and sets two differents solver for the components and the system.
# It then exports the SSP file and re-imports the ssp and instantiate the model, set parameter values from ssv files and map the parameters to ssm file and simulates the model.

model = SSP()
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')
## add top level system connector
model.activeVariant.system.addConnector(Connector('param1', Causality.parameter, SignalType.Real))
model.activeVariant.system.addConnector(Connector('input1', Causality.input, SignalType.Real))

## add subsystem
model.addSystem(CRef('default', 'sub-system'))
## add top level sub-system connector
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('input', Causality.input, SignalType.Real))

component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
component2 = model.addComponent(CRef('default', 'Gain1'), 'resources/Gain.fmu')

component3 = model.addComponent(CRef('default', 'sub-system', 'Gain2'), 'resources/Gain.fmu')

solver2 = {'name' : 'solver2',  'method': 'cvode', 'tolerance': 1e-4}
model.newSolver(solver2)
model.setSolver(CRef('default', 'sub-system', 'Gain2'), 'solver2')

ssv1 = SSV()
ssv1.setValue('mapped_param1', 200.0)
ssv1.setValue('mapped_input1', 300.0)
ssv1.setValue('mapped_Gain_k', 2.0)
ssv1.setValue('mapped_sub_system_input', 400.0)
ssv1.setValue('mapped_sub_system_Gain2_k', 500.0)

ssv1.export("mappedParameter1.ssv")

ssm = SSM()
ssm.mapParameter("mapped_param1", "param1")
ssm.mapParameter("mapped_input1", "input1")
ssm.mapParameter("mapped_Gain_k", "Gain1.k")
ssm.mapParameter("mapped_sub_system_input", "sub-system.input")
ssm.mapParameter("mapped_sub_system_Gain2_k", "sub-system.Gain2.k")
ssm.export("mappedEntry1.ssm")


## add parameter1.ssv to ssp resources
model.addResource("mappedParameter1.ssv", "resources/mappedParameter1.ssv")
## add mappedEntry1.ssm to ssp resources
model.addResource("mappedEntry1.ssm", "resources/mappedEntry1.ssm")

## reference parameters1.ssv to top level system
model.addSSVReference(CRef('default'), 'resources/mappedParameter1.ssv', 'resources/mappedEntry1.ssm')


## System.Input to Element.Input
model.addConnection(CRef('default', 'input1'), CRef('default', 'Gain1', 'u'))
model.addConnection(CRef('default', 'Gain1', 'y'), CRef('default', 'Add1', 'u1'))

## Sub-system.Input to Element.Input
model.addConnection(CRef('default', 'sub-system', 'input'), CRef('default', 'sub-system', 'Gain2', 'u'))
model.export('SimpleSimulation11.ssp')

model2 = SSP('SimpleSimulation11.ssp')
model2.list()
instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,
# print(instantiated_model.dumpApiCalls(), flush=True)
instantiated_model.setResultFile("SimpleSimulation11_res.mat")

print(f"info: After instantiation:")
print(f"info:    default.param1 : {instantiated_model.getValue(CRef('default', 'param1'))}", flush=True)
print(f"info:    default.input1 : {instantiated_model.getValue(CRef('default', 'input1'))}", flush=True)
print(f"info:    default.Gain1.k: {instantiated_model.getValue(CRef('default', 'Gain1', 'k'))}", flush=True)
print(f"info:    default.Gain1.u: {instantiated_model.getValue(CRef('default', 'Gain1', 'u'))}", flush=True)
print(f"info:    default.Gain1.y: {instantiated_model.getValue(CRef('default', 'Gain1', 'y'))}", flush=True)
print(f"info:    default.Add1.u1: {instantiated_model.getValue(CRef('default', 'Add1', 'u1'))}", flush=True)
print(f"info:    default.sub-system.input: {instantiated_model.getValue(CRef('default', 'sub-system', 'input'))}", flush=True)
print(f"info:    default.sub-system.Gain2.k: {instantiated_model.getValue(CRef('default', 'sub-system', 'Gain2', 'k'))}", flush=True)
print(f"info:    default.sub-system.Gain2.u: {instantiated_model.getValue(CRef('default', 'sub-system', 'Gain2', 'u'))}", flush=True)
print(f"info:    default.sub-system.Gain2.y: {instantiated_model.getValue(CRef('default', 'sub-system', 'Gain2', 'y'))}", flush=True)


instantiated_model.initialize()
instantiated_model.simulate()
print(f"info: After simulation:")
print(f"info:    default.param1 : {instantiated_model.getValue(CRef('default', 'param1'))}", flush=True)
print(f"info:    default.input1 : {instantiated_model.getValue(CRef('default', 'input1'))}", flush=True)
print(f"info:    default.Gain1.k: {instantiated_model.getValue(CRef('default', 'Gain1', 'k'))}", flush=True)
print(f"info:    default.Gain1.u: {instantiated_model.getValue(CRef('default', 'Gain1', 'u'))}", flush=True)
print(f"info:    default.Gain1.y: {instantiated_model.getValue(CRef('default', 'Gain1', 'y'))}", flush=True)
print(f"info:    default.Add1.u1: {instantiated_model.getValue(CRef('default', 'Add1', 'u1'))}", flush=True)
print(f"info:    default.sub-system.input: {instantiated_model.getValue(CRef('default', 'sub-system', 'input'))}", flush=True)
print(f"info:    default.sub-system.Gain2.k: {instantiated_model.getValue(CRef('default', 'sub-system', 'Gain2', 'k'))}", flush=True)
print(f"info:    default.sub-system.Gain2.u: {instantiated_model.getValue(CRef('default', 'sub-system', 'Gain2', 'u'))}", flush=True)
print(f"info:    default.sub-system.Gain2.y: {instantiated_model.getValue(CRef('default', 'sub-system', 'Gain2', 'y'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()

## Result:
## info:    model doesn't contain any continuous state
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/Gain.fmu
## |--   resources/mappedEntry1.ssm
## |--   |-- Parameter Mapping:
## |--   |-- |-- source: mapped_param1
## |--   |-- |-- |-- targets: ['param1']
## |--   |-- |-- source: mapped_input1
## |--   |-- |-- |-- targets: ['input1']
## |--   |-- |-- source: mapped_Gain_k
## |--   |-- |-- |-- targets: ['Gain1.k']
## |--   |-- |-- source: mapped_sub_system_input
## |--   |-- |-- |-- targets: ['sub-system.input']
## |--   |-- |-- source: mapped_sub_system_Gain2_k
## |--   |-- |-- |-- targets: ['sub-system.Gain2.k']
## |--   resources/mappedParameter1.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real mapped_param1, 200.0, None, 'None')
## |--   |-- |-- (Real mapped_input1, 300.0, None, 'None')
## |--   |-- |-- (Real mapped_Gain_k, 2.0, None, 'None')
## |--   |-- |-- (Real mapped_sub_system_input, 400.0, None, 'None')
## |--   |-- |-- (Real mapped_sub_system_Gain2_k, 500.0, None, 'None')
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- Parameter Bindings: resources/mappedParameter1.ssv
## |-- |-- |-- |-- Parameter Mapping: resources/mappedEntry1.ssm
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (input, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Elements:
## |-- |-- |-- |-- |-- |-- FMU: Gain2 'None'
## |-- |-- |-- |-- |-- |-- |-- path: resources/Gain.fmu
## |-- |-- |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
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
## |-- |-- |-- Connections:
## |-- |-- |-- |-- .input1 -> Gain1.u
## |-- |-- |-- |-- Gain1.y -> Add1.u1
## |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- (name=solver2, method=cvode, tolerance=0.0001)
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
## info:    maximum step size for 'model.root.solver2': 0.001000
## info:    Result file: SimpleSimulation11_res.mat (bufferSize=1)
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
## info:    Final Statistics for 'model.root.solver2':
##          NumSteps = 1001 NumRhsEvals  = 1002 NumLinSolvSetups = 51
##          NumNonlinSolvIters = 1001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## endResult
