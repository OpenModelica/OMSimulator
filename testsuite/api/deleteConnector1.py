## status: correct
## teardown_command: rm deleteConnector1.ssp
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

component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
component2 = model.addComponent(CRef('default', 'sub-system', 'Add2'), 'resources/Add.fmu')

solver1 = {'name' : 'solver1',  'method': 'euler', 'tolerance': 1e-6}
model.newSolver(solver1)

model.setSolver(CRef('default', 'Add1'), 'solver1')
model.setSolver(CRef('default', 'sub-system', 'Add2'), 'solver1')

model.addConnection(CRef('default', 'param1'), CRef('default', 'Add1', 'u1'))
model.addConnection(CRef('default', 'Add1', 'y'), CRef('default', 'sub-system', 'input'))
model.addConnection(CRef('default', 'sub-system', 'Add2', 'u1'), CRef('default', 'sub-system', 'input'))

model.export('deleteConnector1.ssp')

model2 = SSP('deleteConnector1.ssp')
model2.delete(CRef('default', 'param1'))
model2.delete(CRef('default', 'sub-system', 'input'))
model2.delete(CRef('default', 'sub-system', 'Add2', 'u1'))
model2.delete(CRef('default', 'Add1', 'u1'))
print("## After deleting connectors:")
print("=============================")
model2.list()

## Result:
## ## After deleting connectors:
## =============================
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/Gain.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- Elements:
## |-- |-- |-- |-- |-- |-- FMU: Add2 'None'
## |-- |-- |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- |-- |-- Connectors:
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
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- |-- |-- name: solver1
## |-- |-- |-- Connections:
## |-- |-- |-- |-- .param1 -> Add1.u1
## |-- |-- |-- |-- Add1.y -> sub-system.input
## |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- (name=solver1, method=euler, tolerance=1e-06)
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
