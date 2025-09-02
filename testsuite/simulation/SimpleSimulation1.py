## status: correct
## teardown_command: rm -rf SimpleSimulaton1.ssp SimpleSimulation1_res.mat
## linux: no
## ucrt64: yes
## win: yes
## mac: no

from OMSimulator import SSP, Settings, CRef

Settings.suppressPath = True

model = SSP()
model.addResource('../resources/Modelica.Electrical.Analog.Examples.CauerLowPassAnalog.fmu', new_name='resources/CauerLowPassAnalog.fmu')

component1 = model.addComponent(CRef('default', 'CauerLowPassAnalog'), 'resources/CauerLowPassAnalog.fmu')
solver1 = {'name' : 'solver1',  'method': 'cvode', 'tolerance': 1e-4}
model.newSolver(solver1)

model.setSolver(CRef('default', 'CauerLowPassAnalog'), 'solver1')
#model.list()
model.export('SimpleSimulation1.ssp')

model2 = SSP('SimpleSimulation1.ssp')

model2.list()
print("", flush=True)
instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,
instantiated_model.setResultFile("SimpleSimulation1_res.mat")
instantiated_model.initialize()
instantiated_model.simulate()
instantiated_model.terminate()
instantiated_model.delete()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/CauerLowPassAnalog.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: CauerLowPassAnalog 'None'
## |-- |-- |-- |-- |-- path: resources/CauerLowPassAnalog.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (C1.C, Causality.calculatedParameter, SignalType.Real, F, 'Capacitance')
## |-- |-- |-- |-- |-- |-- (C2.C, Causality.calculatedParameter, SignalType.Real, F, 'Capacitance')
## |-- |-- |-- |-- |-- |-- (C3.C, Causality.calculatedParameter, SignalType.Real, F, 'Capacitance')
## |-- |-- |-- |-- |-- |-- (C4.C, Causality.calculatedParameter, SignalType.Real, F, 'Capacitance')
## |-- |-- |-- |-- |-- |-- (C5.C, Causality.calculatedParameter, SignalType.Real, F, 'Capacitance')
## |-- |-- |-- |-- |-- |-- (L1.L, Causality.calculatedParameter, SignalType.Real, H, 'Inductance')
## |-- |-- |-- |-- |-- |-- (L2.L, Causality.calculatedParameter, SignalType.Real, H, 'Inductance')
## |-- |-- |-- |-- |-- |-- (R1.R, Causality.parameter, SignalType.Real, Ohm, 'Resistance at temperature T_ref')
## |-- |-- |-- |-- |-- |-- (R1.T, Causality.calculatedParameter, SignalType.Real, K, 'Fixed device temperature if useHeatPort = false')
## |-- |-- |-- |-- |-- |-- (R1.T_heatPort, Causality.calculatedParameter, SignalType.Real, K, 'Temperature of heatPort')
## |-- |-- |-- |-- |-- |-- (R1.T_ref, Causality.parameter, SignalType.Real, K, 'Reference temperature')
## |-- |-- |-- |-- |-- |-- (R1.alpha, Causality.parameter, SignalType.Real, 1/K, 'Temperature coefficient of resistance (R_actual = R*(1 + alpha*(T_heatPort - T_ref))')
## |-- |-- |-- |-- |-- |-- (R2.R, Causality.parameter, SignalType.Real, Ohm, 'Resistance at temperature T_ref')
## |-- |-- |-- |-- |-- |-- (R2.T, Causality.calculatedParameter, SignalType.Real, K, 'Fixed device temperature if useHeatPort = false')
## |-- |-- |-- |-- |-- |-- (R2.T_heatPort, Causality.calculatedParameter, SignalType.Real, K, 'Temperature of heatPort')
## |-- |-- |-- |-- |-- |-- (R2.T_ref, Causality.parameter, SignalType.Real, K, 'Reference temperature')
## |-- |-- |-- |-- |-- |-- (R2.alpha, Causality.parameter, SignalType.Real, 1/K, 'Temperature coefficient of resistance (R_actual = R*(1 + alpha*(T_heatPort - T_ref))')
## |-- |-- |-- |-- |-- |-- (V.V, Causality.parameter, SignalType.Real, V, 'Height of step')
## |-- |-- |-- |-- |-- |-- (V.offset, Causality.parameter, SignalType.Real, V, 'Voltage offset')
## |-- |-- |-- |-- |-- |-- (V.signalSource.height, Causality.calculatedParameter, SignalType.Real, None, 'Height of step')
## |-- |-- |-- |-- |-- |-- (V.signalSource.offset, Causality.calculatedParameter, SignalType.Real, None, 'Offset of output signal y')
## |-- |-- |-- |-- |-- |-- (V.signalSource.startTime, Causality.calculatedParameter, SignalType.Real, s, 'Output y = offset for time < startTime')
## |-- |-- |-- |-- |-- |-- (V.startTime, Causality.parameter, SignalType.Real, s, 'Time offset')
## |-- |-- |-- |-- |-- |-- (c1, Causality.parameter, SignalType.Real, F, 'filter coefficient c1')
## |-- |-- |-- |-- |-- |-- (c2, Causality.calculatedParameter, SignalType.Real, F, 'filter coefficient c2')
## |-- |-- |-- |-- |-- |-- (c3, Causality.parameter, SignalType.Real, F, 'filter coefficient c3')
## |-- |-- |-- |-- |-- |-- (c4, Causality.calculatedParameter, SignalType.Real, F, 'filter coefficient c4')
## |-- |-- |-- |-- |-- |-- (c5, Causality.parameter, SignalType.Real, F, 'filter coefficient c5')
## |-- |-- |-- |-- |-- |-- (l1, Causality.parameter, SignalType.Real, H, 'filter coefficient I1')
## |-- |-- |-- |-- |-- |-- (l2, Causality.parameter, SignalType.Real, H, 'filter coefficient I2')
## |-- |-- |-- |-- |-- |-- (R1.useHeatPort, Causality.calculatedParameter, SignalType.Boolean, None, '=true, if heatPort is enabled')
## |-- |-- |-- |-- |-- |-- (R2.useHeatPort, Causality.calculatedParameter, SignalType.Boolean, None, '=true, if heatPort is enabled')
## |-- |-- |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- |-- |-- name: solver1
## |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- (name=solver1, method=cvode, tolerance=0.0001)
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
##
## info:    maximum step size for 'model.root.solver1': 0.001000
## info:    Result file: SimpleSimulation1_res.mat (bufferSize=1)
## info:    Final Statistics for 'model.root.solver1':
##          NumSteps = 1001 NumRhsEvals  = 1002 NumLinSolvSetups = 51
##          NumNonlinSolvIters = 1001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## endResult
