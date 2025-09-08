## status: correct
## teardown_command: rm -rf exportJson4.ssp export_json4_res.mat
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, Connector, Causality, SignalType

Settings.suppressPath = True

model = SSP()
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')

model.addSystem(CRef('default', 'sub-system'))
## add top level system connector
model.activeVariant.system.addConnector(Connector('param1', Causality.parameter, SignalType.Real))
## add top level sub-system connector
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('input', Causality.input, SignalType.Real))
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('output', Causality.output, SignalType.Real))

component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
component2 = model.addComponent(CRef('default', 'sub-system', 'Add2'), 'resources/Add.fmu')

solver1 = {'name' : 'solver1',  'method': 'euler', 'tolerance': 1e-6}
model.newSolver(solver1)

model.setSolver(CRef('default', 'Add1'), 'solver1')
model.setSolver(CRef('default', 'sub-system', 'Add2'), 'solver1')

model.addConnection(CRef('default', 'param1'), CRef('default', 'Add1', 'u1'))
model.addConnection(CRef('default', 'Add1', 'y'), CRef('default', 'sub-system', 'output'))
model.addConnection(CRef('default', 'sub-system', 'Add2', 'u1'), CRef('default', 'sub-system', 'input'))

model.export('exportJson4.ssp')

model2 = SSP('exportJson4.ssp')
model.list() ## internally generate the json file and also set the model state like virgin,

instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,
# print(instantiated_model.dumpApiCalls(), flush=True)
instantiated_model.setResultFile("export_json4_res.mat")
instantiated_model.setValue(CRef('default', 'param1'), 2.0)
instantiated_model.setValue(CRef('default', 'Add1', 'u2'), 3.0)

print(f"info: After instantiation:")
print(f"info:    default.param1           : {instantiated_model.getValue(CRef('default', 'param1'))}", flush=True)
print(f"info:    default.sub-system.input : {instantiated_model.getValue(CRef('default', 'sub-system', 'input'))}", flush=True)
print(f"info:    default.sub-system.output: {instantiated_model.getValue(CRef('default', 'sub-system', 'output'))}", flush=True)
print(f"info:    default.Add1.u1          : {instantiated_model.getValue(CRef('default', 'Add1', 'u1'))}", flush=True)
print(f"info:    default.Add1.u2          : {instantiated_model.getValue(CRef('default', 'Add1', 'u2'))}", flush=True)
print(f"info:    default.Add1.y           : {instantiated_model.getValue(CRef('default', 'Add1', 'y'))}", flush=True)

instantiated_model.initialize()
instantiated_model.simulate()
print(f"info: After simulation:")
print(f"info:    default.param1           : {instantiated_model.getValue(CRef('default', 'param1'))}", flush=True)
print(f"info:    default.sub-system.input : {instantiated_model.getValue(CRef('default', 'sub-system', 'input'))}", flush=True)
print(f"info:    default.sub-system.output: {instantiated_model.getValue(CRef('default', 'sub-system', 'output'))}", flush=True)
print(f"info:    default.Add1.u1          : {instantiated_model.getValue(CRef('default', 'Add1', 'u1'))}", flush=True)
print(f"info:    default.Add1.u2          : {instantiated_model.getValue(CRef('default', 'Add1', 'u2'))}", flush=True)
print(f"info:    default.Add1.y           : {instantiated_model.getValue(CRef('default', 'Add1', 'y'))}", flush=True)

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
## |-- Variant "default": None
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (input, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (output, Causality.output, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Elements:
## |-- |-- |-- |-- |-- |-- FMU: Add2 'None'
## |-- |-- |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- |-- |-- |-- |-- name: solver1
## |-- |-- |-- |-- |-- Connections:
## |-- |-- |-- |-- |-- |-- Add2.u1 -> .input
## |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- |-- |-- name: solver1
## |-- |-- |-- Connections:
## |-- |-- |-- |-- .param1 -> Add1.u1
## |-- |-- |-- |-- Add1.y -> sub-system.output
## |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- (name=solver1, method=euler, tolerance=1e-06)
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## info: After instantiation:
## info:    default.param1           : 2.0
## info:    default.sub-system.input : 0.0
## info:    default.sub-system.output: 0.0
## info:    default.Add1.u1          : 0.0
## info:    default.Add1.u2          : 3.0
## info:    default.Add1.y           : 3.0
## info:    maximum step size for 'model.root.solver1': 0.001000
## info:    Result file: export_json4_res.mat (bufferSize=1)
## info: After simulation:
## info:    default.param1           : 2.0
## info:    default.sub-system.input : 0.0
## info:    default.sub-system.output: 5.0
## info:    default.Add1.u1          : 2.0
## info:    default.Add1.u2          : 3.0
## info:    default.Add1.y           : 5.0
## info:    Final Statistics for 'model.root.solver1':
##          NumSteps = 1001 NumRhsEvals  = 1002 NumLinSolvSetups = 51
##          NumNonlinSolvIters = 1001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## endResult
