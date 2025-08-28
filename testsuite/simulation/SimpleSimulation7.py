## status: correct
## teardown_command: rm -rf SimpleSimulation7.ssp SimpleSimulation7_res.mat
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, Connector, Causality, SignalType

Settings.suppressPath = True


# This example creates a new SSP file with an FMU instantiated as a component and sets two differents solver for the components and the system.
# It then exports the SSP file and re-imports it to verify the solver settings and the simulates the model.

model = SSP()
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')
## add top level system connector
model.activeVariant.system.addConnector(Connector('param1', Causality.parameter, SignalType.Real))
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

model.addConnection(CRef('default', 'Gain1', 'y'), CRef('default', 'Add1', 'u1'))
## System.Input to Element.Input
model.addConnection(CRef('default', 'sub-system', 'input'), CRef('default', 'sub-system', 'Gain2', 'u'))
model.export('SimpleSimulation7.ssp')

model2 = SSP('SimpleSimulation7.ssp')
#model2.list()
instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,
# print(instantiated_model.dumpApiCalls(), flush=True)
instantiated_model.setResultFile("SimpleSimulation7_res.mat")
instantiated_model.setValue(CRef('default', 'Gain1', 'k'), 2.0)
instantiated_model.setValue(CRef('default', 'Gain1', 'u'), 6.0)
instantiated_model.setValue(CRef('default', 'param1'), 100.0)
instantiated_model.setValue(CRef('default', 'sub-system', 'input'), 200.0)

print(f"info: After instantiation:")
print(f"info:    default.param1 : {instantiated_model.getValue(CRef('default', 'param1'))}", flush=True)
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
## info: After instantiation:
## info:    default.param1 : 100.0
## info:    default.Gain1.k: 2.0
## info:    default.Gain1.u: 6.0
## info:    default.Gain1.y: 12.0
## info:    default.Add1.u1: 0.0
## info:    default.sub-system.input: 200.0
## info:    default.sub-system.Gain2.k: 1.0
## info:    default.sub-system.Gain2.u: 0.0
## info:    default.sub-system.Gain2.y: 0.0
## info:    maximum step size for 'model.root.solver2': 0.001000
## info:    Result file: SimpleSimulation7_res.mat (bufferSize=1)
## info: After simulation:
## info:    default.param1 : 100.0
## info:    default.Gain1.k: 2.0
## info:    default.Gain1.u: 6.0
## info:    default.Gain1.y: 12.0
## info:    default.Add1.u1: 12.0
## info:    default.sub-system.input: 200.0
## info:    default.sub-system.Gain2.k: 1.0
## info:    default.sub-system.Gain2.u: 200.0
## info:    default.sub-system.Gain2.y: 200.0
## info:    Final Statistics for 'model.root.solver2':
##          NumSteps = 1001 NumRhsEvals  = 1002 NumLinSolvSetups = 51
##          NumNonlinSolvIters = 1001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## endResult
