## status: correct
## teardown_command: rm -rf pwm_res.mat
## linux: no
## ucrt64: yes
## win: yes
## mac: no

from OMSimulator import SSP, Settings, CRef

Settings.suppressPath = True

model = SSP('../resources/PWMTest.ssp')
# solver2 = {'name' : 'solver2',  'method': 'cvode', 'tolerance': 1e-4}
# model.newSolver(solver2)
# model.setSolver(CRef('Root', 'SCSystem', 'circuit'), 'solver2')

model.list()
print("", flush=True)
instantiated_model = model.instantiate() ## internally generate the json file and also set the model state like virgin,
#print(instantiated_model.dumpApiCalls())
instantiated_model.setResultFile("pwm_res.mat")
instantiated_model.initialize()
instantiated_model.simulate()
instantiated_model.terminate()
instantiated_model.delete()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/0001_replacepulse.fmu
## |--   resources/0004_circuit.fmu
## |--   resources/signalFilter.xml
## |-- Active Variant: PWMTest
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "PWMTest": <hidden>
## |-- |-- System: Root 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: SCSystem 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.199528, y:0.490094)
## |-- |-- |-- |-- |-- ElementGeometry:
## |-- |-- |-- |-- |-- |-- (x1:-1.30189, y1:-9.119497, x2:18.69811, y2:10.880503, rotation:0.0, icon_source:None, icon_rotation:0.0, icon_flip:False, icon_fixed_aspect_ratio:False)
## |-- |-- |-- |-- |-- Elements:
## |-- |-- |-- |-- |-- |-- FMU: circuit 'None'
## |-- |-- |-- |-- |-- |-- |-- path: resources/0004_circuit.fmu
## |-- |-- |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.0, y:0.5)
## |-- |-- |-- |-- |-- |-- |-- |-- (inductor.L, Causality.parameter, SignalType.Real, H, 'None')
## |-- |-- |-- |-- |-- |-- |-- |-- (resistor.R, Causality.parameter, SignalType.Real, Ohm, 'None')
## |-- |-- |-- |-- |-- |-- |-- |-- (resistor.T_ref, Causality.parameter, SignalType.Real, K, 'None')
## |-- |-- |-- |-- |-- |-- |-- |-- (resistor.alpha, Causality.parameter, SignalType.Real, 1/K, 'None')
## |-- |-- |-- |-- |-- |-- |-- |-- (resistor.T, Causality.calculatedParameter, SignalType.Real, K, 'None')
## |-- |-- |-- |-- |-- |-- |-- |-- (resistor.T_heatPort, Causality.calculatedParameter, SignalType.Real, K, 'None')
## |-- |-- |-- |-- |-- |-- |-- |-- (resistor.useHeatPort, Causality.calculatedParameter, SignalType.Boolean, None, 'None')
## |-- |-- |-- |-- |-- |-- |-- ElementGeometry:
## |-- |-- |-- |-- |-- |-- |-- |-- (x1:-17.96226, y1:-12.64151, x2:2.03774, y2:7.35849, rotation:0.0, icon_source:None, icon_rotation:0.0, icon_flip:False, icon_fixed_aspect_ratio:False)
## |-- |-- |-- |-- |-- Connections:
## |-- |-- |-- |-- |-- |-- .u -> circuit.u
## |-- |-- |-- |-- |-- |-- |-- ConnectionGeometry: (pointsX: [-37.5, -37.5], pointsY: [-2.0, -3.0])
## |-- |-- |-- |-- FMU: pulse 'None'
## |-- |-- |-- |-- |-- path: resources/0001_replacepulse.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.5)
## |-- |-- |-- |-- |-- |-- (amplitude, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (offset, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (period, Causality.parameter, SignalType.Real, s, 'None')
## |-- |-- |-- |-- |-- |-- (startTime, Causality.parameter, SignalType.Real, s, 'None')
## |-- |-- |-- |-- |-- |-- (width, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (nperiod, Causality.parameter, SignalType.Integer, None, 'None')
## |-- |-- |-- |-- |-- ElementGeometry:
## |-- |-- |-- |-- |-- |-- (x1:-61.3082, y1:-9.79874, x2:-41.3082, y2:10.20126, rotation:0.0, icon_source:None, icon_rotation:0.0, icon_flip:False, icon_fixed_aspect_ratio:False)
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real width, 50.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Real startTime, 0.0, s, 'None')
## |-- |-- |-- |-- |-- |-- (Real period, 0.02, s, 'None')
## |-- |-- |-- |-- |-- |-- (Real offset, 0.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Real amplitude, 220.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Integer nperiod, -1, None, 'None')
## |-- |-- |-- Connections:
## |-- |-- |-- |-- pulse.y -> SCSystem.u
## |-- |-- |-- |-- |-- ConnectionGeometry: (pointsX: [3.0], pointsY: [0.0])
## |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- ()
## |-- UnitDefinitions:
## |-- |-- Unit: s
## |-- |-- |-- BaseUnit: s: 1
## |-- DefaultExperiment
## |-- |-- startTime: 0.000000
## |-- |-- stopTime: 1.000000
##
## info:    maximum step size for 'model.root': 0.001000
## info:    Result file: pwm_res.mat (bufferSize=1)
## info:    Final Statistics for 'model.root':
##          NumSteps = 0 NumRhsEvals  = 0 NumLinSolvSetups = 0
##          NumNonlinSolvIters = 0 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## endResult
