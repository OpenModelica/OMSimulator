## status: correct
## teardown_command: rm -rf NestedSystemSimulation1.ssp NestedSystemSimulation1_res.mat
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, Connector, Causality, SignalType

Settings.suppressPath = True


# This example creates a new SSP file with an FMU instantiated as a component and sets two differents solver for the components and the system.
# It then exports the SSP file and re-imports it to verify the solver settings and the simulates the model.

model = SSP()
## add top level system connector
model.activeVariant.system.addConnector(Connector('input1', Causality.input, SignalType.Real))
model.activeVariant.system.addConnector(Connector('param1', Causality.parameter, SignalType.Real))

## add subsystem
model.addSystem(CRef('default', 'sub-system1'))
## add top level sub-system connector with same name as the top level system to verify the handling of name conflicts for nested systems, e.g. sub-system1_input1, sub-system1_param1
model.activeVariant.system.elements[CRef('sub-system1')].addConnector(Connector('input1', Causality.input, SignalType.Real))
model.activeVariant.system.elements[CRef('sub-system1')].addConnector(Connector('param1', Causality.parameter, SignalType.Real))

## add resouces
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')

## add components to default system
model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
model.addComponent(CRef('default', 'Gain1'), 'resources/Gain.fmu')

## add components to sub-system1 with same name as the top level system to verify the handling of name conflicts for nested systems, e.g. sub-system1_Add1, sub-system1_Gain1
model.addComponent(CRef('default', 'sub-system1', 'Add1'), 'resources/Add.fmu')
model.addComponent(CRef('default', 'sub-system1', 'Gain1'), 'resources/Gain.fmu')

model.setValue(CRef('default', 'input1'), 3.0)
model.setValue(CRef('default', 'sub-system1', 'input1'), 10.0)

model.addConnection(CRef('default', 'input1'), CRef('default', 'Gain1', 'u'))
model.addConnection(CRef('default', 'sub-system1', 'input1'), CRef('default', 'sub-system1', 'Gain1', 'u'))
model.addConnection(CRef('default', 'Gain1', 'y'), CRef('default', 'Add1', 'u1'))
model.addConnection(CRef('default', 'sub-system1', 'Gain1', 'y'), CRef('default', 'sub-system1', 'Add1', 'u1'))

#model.list()
# solver1 = {'name' : 'solver1',  'method': 'euler', 'tolerance': 1e-4}
# model.newSolver(solver1)

# model.setSolver(CRef('default', 'Gain1'), 'solver1')
# model.setSolver(CRef('default', 'Add1'), 'solver1')

# solver2 = {'name' : 'solver2',  'method': 'cvode', 'tolerance': 1e-4}
# model.newSolver(solver2)

# model.setSolver(CRef('default', 'sub-system1', 'Gain2'), 'solver2')
# model.setSolver(CRef('default', 'sub-system1', 'Add2'), 'solver2')

model.export('NestedSystemSimulation1.ssp')

model2 = SSP('NestedSystemSimulation1.ssp')
model2.list()

instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,
#print(instantiated_model.dumpApiCalls(), flush=True)

instantiated_model.setResultFile("NestedSystemSimulation1_res.mat")
# instantiated_model.setValue(CRef('default', 'Gain1', 'k'), 2.0)
# instantiated_model.setValue(CRef('default', 'Gain1', 'u'), 6.0)

print(f"info: After instantiation:")
print(f"info:    default.input1: {instantiated_model.getValue(CRef('default', 'input1'))}", flush=True)
print(f"info:    default.param1: {instantiated_model.getValue(CRef('default', 'param1'))}", flush=True)
print(f"info:    default.Gain1.k: {instantiated_model.getValue(CRef('default', 'Gain1', 'k'))}", flush=True)
print(f"info:    default.Gain1.u: {instantiated_model.getValue(CRef('default', 'Gain1', 'u'))}", flush=True)
print(f"info:    default.Gain1.y: {instantiated_model.getValue(CRef('default', 'Gain1', 'y'))}", flush=True)
print(f"info:    default.Add1.u1: {instantiated_model.getValue(CRef('default', 'Add1', 'u1'))}", flush=True)
print(f"info:    default.sub-system1.input1: {instantiated_model.getValue(CRef('default', 'sub-system1', 'input1'))}", flush=True)
print(f"info:    default.sub-system1.param1: {instantiated_model.getValue(CRef('default', 'sub-system1', 'param1'))}", flush=True)
print(f"info:    default.sub-system1.Gain1.k: {instantiated_model.getValue(CRef('default', 'sub-system1', 'Gain1', 'k'))}", flush=True)
print(f"info:    default.sub-system1.Gain1.u: {instantiated_model.getValue(CRef('default', 'sub-system1', 'Gain1', 'u'))}", flush=True)
print(f"info:    default.sub-system1.Gain1.y: {instantiated_model.getValue(CRef('default', 'sub-system1', 'Gain1', 'y'))}", flush=True)
print(f"info:    default.sub-system1.Add1.u1: {instantiated_model.getValue(CRef('default', 'sub-system1', 'Add1', 'u1'))}", flush=True)


instantiated_model.initialize()
instantiated_model.simulate()
print(f"info: After simulation:")
print(f"info:    default.input1: {instantiated_model.getValue(CRef('default', 'input1'))}", flush=True)
print(f"info:    default.param1: {instantiated_model.getValue(CRef('default', 'param1'))}", flush=True)
print(f"info:    default.Gain1.k: {instantiated_model.getValue(CRef('default', 'Gain1', 'k'))}", flush=True)
print(f"info:    default.Gain1.u: {instantiated_model.getValue(CRef('default', 'Gain1', 'u'))}", flush=True)
print(f"info:    default.Gain1.y: {instantiated_model.getValue(CRef('default', 'Gain1', 'y'))}", flush=True)
print(f"info:    default.Add1.u1: {instantiated_model.getValue(CRef('default', 'Add1', 'u1'))}", flush=True)
print(f"info:    default.sub-system1.input1: {instantiated_model.getValue(CRef('default', 'sub-system1', 'input1'))}", flush=True)
print(f"info:    default.sub-system1.param1: {instantiated_model.getValue(CRef('default', 'sub-system1', 'param1'))}", flush=True)
print(f"info:    default.sub-system1.Gain1.k: {instantiated_model.getValue(CRef('default', 'sub-system1', 'Gain1', 'k'))}", flush=True)
print(f"info:    default.sub-system1.Gain1.u: {instantiated_model.getValue(CRef('default', 'sub-system1', 'Gain1', 'u'))}", flush=True)
print(f"info:    default.sub-system1.Gain1.y: {instantiated_model.getValue(CRef('default', 'sub-system1', 'Gain1', 'y'))}", flush=True)
print(f"info:    default.sub-system1.Add1.u1: {instantiated_model.getValue(CRef('default', 'sub-system1', 'Add1', 'u1'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()


## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/Gain.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- (Real input1, 3.0, None, 'None')
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system1 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real input1, 10.0, None, 'None')
## |-- |-- |-- |-- |-- Elements:
## |-- |-- |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- |-- FMU: Gain1 'None'
## |-- |-- |-- |-- |-- |-- |-- path: resources/Gain.fmu
## |-- |-- |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- |-- |-- Connections:
## |-- |-- |-- |-- |-- |-- .input1 -> Gain1.u
## |-- |-- |-- |-- |-- |-- Gain1.y -> Add1.u1
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
## |-- UnitDefinitions:
## |-- |-- Unit: 1
## |-- |-- |-- BaseUnit:
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## info: After instantiation:
## info:    default.input1: 3.0
## info:    default.param1: 0.0
## info:    default.Gain1.k: 1.0
## info:    default.Gain1.u: 0.0
## info:    default.Gain1.y: 0.0
## info:    default.Add1.u1: 0.0
## info:    default.sub-system1.input1: 10.0
## info:    default.sub-system1.param1: 0.0
## info:    default.sub-system1.Gain1.k: 1.0
## info:    default.sub-system1.Gain1.u: 0.0
## info:    default.sub-system1.Gain1.y: 0.0
## info:    default.sub-system1.Add1.u1: 0.0
## info:    Result file: NestedSystemSimulation1_res.mat (bufferSize=1)
## info: After simulation:
## info:    default.input1: 3.0
## info:    default.param1: 0.0
## info:    default.Gain1.k: 1.0
## info:    default.Gain1.u: 3.0
## info:    default.Gain1.y: 3.0
## info:    default.Add1.u1: 3.0
## info:    default.sub-system1.input1: 10.0
## info:    default.sub-system1.param1: 0.0
## info:    default.sub-system1.Gain1.k: 1.0
## info:    default.sub-system1.Gain1.u: 10.0
## info:    default.sub-system1.Gain1.y: 10.0
## info:    default.sub-system1.Add1.u1: 10.0
## endResult
