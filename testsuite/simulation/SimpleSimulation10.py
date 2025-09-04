## status: correct
## teardown_command: rm -rf SimpleSimulation10.ssp SimpleSimulation10_res.mat parameters2.ssv
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, Connector, Causality, SignalType, SSV

Settings.suppressPath = True


# This example creates a new SSP file with an FMU instantiated as a component and sets two differents solver for the components and the system.
# It then exports the SSP file and re-imports the ssp and instantiate the model, set parameter values from ssv files at different level and simulates.

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
ssv1.setValue('param1', 200.0)
ssv1.setValue('input1', 300.0)
ssv1.export("parameters2.ssv")


ssv2 = SSV()
ssv2.setValue('k', 2.0)
ssv2.export("parameters3.ssv")

ssv3 = SSV()
ssv3.setValue('input', 400.0)
ssv3.export("parameters4.ssv")

ssv4 = SSV()
ssv4.setValue('k', 500.0)
ssv4.export("parameters5.ssv")

## add parameter2.ssv to to ssp resources
model.addResource("parameters2.ssv", "resources/parameters2.ssv")
## reference parameters2.ssv to top level system
model.addSSVReference(CRef('default'), 'resources/parameters2.ssv')

## add parameter3.ssv to to ssp resources
model.addResource("parameters3.ssv", "resources/parameters3.ssv")
## reference parameters3.ssv to Gain1 components
model.addSSVReference(CRef('default','Gain1'), 'resources/parameters3.ssv')

## add parameter4.ssv to to ssp resources
model.addResource("parameters4.ssv", "resources/parameters4.ssv")
## reference parameters4.ssv to Gain1 components
model.addSSVReference(CRef('default','sub-system'), 'resources/parameters4.ssv')

## add parameter5.ssv to to ssp resources
model.addResource("parameters5.ssv", "resources/parameters5.ssv")
## reference parameters5.ssv to subsystem-Gain2 components
model.addSSVReference(CRef('default','sub-system', 'Gain2'), 'resources/parameters5.ssv')

## System.Input to Element.Input
model.addConnection(CRef('default', 'input1'), CRef('default', 'Gain1', 'u'))
model.addConnection(CRef('default', 'Gain1', 'y'), CRef('default', 'Add1', 'u1'))

## Sub-system.Input to Element.Input
model.addConnection(CRef('default', 'sub-system', 'input'), CRef('default', 'sub-system', 'Gain2', 'u'))
model.export('SimpleSimulation10.ssp')

model2 = SSP('SimpleSimulation10.ssp')
model2.list()
instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,
#print(instantiated_model.dumpApiCalls(), flush=True)
instantiated_model.setResultFile("SimpleSimulation10_res.mat")

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
## |--   resources/parameters2.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real param1, 200.0, None, 'None')
## |--   |-- |-- (Real input1, 300.0, None, 'None')
## |--   resources/parameters3.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k, 2.0, None, 'None')
## |--   resources/parameters4.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real input, 400.0, None, 'None')
## |--   resources/parameters5.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real k, 500.0, None, 'None')
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- Parameter Bindings: resources/parameters2.ssv
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (input, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/parameters4.ssv
## |-- |-- |-- |-- |-- Elements:
## |-- |-- |-- |-- |-- |-- FMU: Gain2 'None'
## |-- |-- |-- |-- |-- |-- |-- path: resources/Gain.fmu
## |-- |-- |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- |-- |-- |-- |-- Parameter Bindings: resources/parameters5.ssv
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
## |-- |-- |-- |-- |-- Parameter Bindings: resources/parameters3.ssv
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
## info:    Result file: SimpleSimulation10_res.mat (bufferSize=1)
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
