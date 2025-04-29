## status: correct
## teardown_command: rm exportJson3.ssp
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True


# This example creates a new SSP file with an FMU instantiated as a component and sets two differents solver for the components and the system.
# It then exports the SSP file and re-imports it to verify the solver settings.
model = SSP()
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')

component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
component2 = model.addComponent(CRef('default', 'Add2'), 'resources/Add.fmu')
component3 = model.addComponent(CRef('default', 'Gain1'), 'resources/Gain.fmu')
component4 = model.addComponent(CRef('default', 'Gain2'), 'resources/Gain.fmu')

solver1 = {'name' : 'solver1',  'method': 'euler', 'tolerance': 1e-6}
model.newSolver(solver1)

solver2 = {'name' : 'solver2',  'method': 'cvode', 'tolerance': 1e-4}
model.newSolver(solver2)

model.setSolver(CRef('default', 'Add1'), 'solver1')
model.setSolver(CRef('default', 'Add2'), 'solver2')
model.setSolver(CRef('default', 'Gain1'), 'solver1')
model.setSolver(CRef('default', 'Gain2'), 'solver2')

model.addConnection(CRef('default', 'Gain1', 'y'), CRef('default', 'Add1', 'u1'))
model.addConnection(CRef('default', 'Gain2', 'y'), CRef('default', 'Add2', 'u1'))
model.addConnection(CRef('default', 'Gain2', 'y'), CRef('default', 'Add2', 'u2'))

#model.list()
model.export('exportJson3.ssp')

model2 = SSP('exportJson3.ssp')

model2.list()
model2.instantiate() ## internally generate the json file and also set the model state like virgin,

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
## |-- |-- |-- Elements:
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
## |-- |-- |-- |-- FMU: Add2 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- |-- |-- name: solver2
## |-- |-- |-- |-- FMU: Gain1 'None'
## |-- |-- |-- |-- |-- path: resources/Gain.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- |-- |-- name: solver1
## |-- |-- |-- |-- FMU: Gain2 'None'
## |-- |-- |-- |-- |-- path: resources/Gain.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- |-- |-- name: solver2
## |-- |-- |-- Connections:
## |-- |-- |-- |-- Gain1.y -> Add1.u1
## |-- |-- |-- |-- Gain2.y -> Add2.u1
## |-- |-- |-- |-- Gain2.y -> Add2.u2
## |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- (name=solver1, method=euler, tolerance=1e-06)
## |-- |-- |-- |-- (name=solver2, method=cvode, tolerance=0.0001)
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## {
##   "simulation units": [
##     {
##       "components": [
##         {
##           "name": [
##             "default",
##             "Add1"
##           ],
##           "type": "me_cs",
##           "path": "resources/Add.fmu"
##         },
##         {
##           "name": [
##             "default",
##             "Gain1"
##           ],
##           "type": "me_cs",
##           "path": "resources/Gain.fmu"
##         }
##       ],
##       "solver": {
##         "type": "co-simulation",
##         "name": "solver1"
##       },
##       "connections": [
##         {
##           "start element": [
##             "default",
##             "Gain1"
##           ],
##           "start connector": "y",
##           "end element": [
##             "default",
##             "Add1"
##           ],
##           "end connector": "u1"
##         }
##       ]
##     },
##     {
##       "components": [
##         {
##           "name": [
##             "default",
##             "Add2"
##           ],
##           "type": "me_cs",
##           "path": "resources/Add.fmu"
##         },
##         {
##           "name": [
##             "default",
##             "Gain2"
##           ],
##           "type": "me_cs",
##           "path": "resources/Gain.fmu"
##         }
##       ],
##       "solver": {
##         "type": "co-simulation",
##         "name": "solver2"
##       },
##       "connections": [
##         {
##           "start element": [
##             "default",
##             "Gain2"
##           ],
##           "start connector": "y",
##           "end element": [
##             "default",
##             "Add2"
##           ],
##           "end connector": "u1"
##         },
##         {
##           "start element": [
##             "default",
##             "Gain2"
##           ],
##           "start connector": "y",
##           "end element": [
##             "default",
##             "Add2"
##           ],
##           "end connector": "u2"
##         }
##       ]
##     }
##   ],
##   "result file": "simulation_result.csv",
##   "simulation settings": {
##     "start time": 0,
##     "stop time": 10,
##     "tolerance": 1e-06
##   }
## }
## Status.ok
## endResult
