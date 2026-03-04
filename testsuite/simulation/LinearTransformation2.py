## status: correct
## teardown_command: rm -rf LinearTransformation2_res.mat
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True


# This example tests the support for LinearTransformation in provided in <ssd:connection>.
model = SSP('../resources/LinearTransformation.ssp')
solver2 = {'name' : 'solver2',  'method': 'cvode', 'tolerance': 1e-4}
model.newSolver(solver2)
model.setSolver(CRef('default', 'Add1'), 'solver2')
model.setSolver(CRef('default', 'Gain1'), 'solver2')
model.list()
print("", flush=True)
instantiated_model = model.instantiate()
instantiated_model.setResultFile("LinearTransformation2_res.mat")
instantiated_model.initialize()
instantiated_model.simulate()

print(f"info: After simulation:")
print(f"info:    default.Gain1.y: {instantiated_model.getValue(CRef('default', 'Gain1', 'y'))}", flush=True)
print(f"info:    default.Add1.u1: {instantiated_model.getValue(CRef('default', 'Add1', 'u1'))}", flush=True)

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
## |-- |-- |-- |-- |-- |-- name: solver2
## |-- |-- |-- |-- FMU: Gain1 'None'
## |-- |-- |-- |-- |-- path: resources/Gain.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- |-- |-- name: solver2
## |-- |-- |-- Connections:
## |-- |-- |-- |-- Gain1.y -> Add1.u1
## |-- |-- |-- |-- |-- LinearTransformation: (factor: 2.0, offset: 3.0)
## |-- |-- |-- Solver Settings:
## |-- |-- |-- |-- (name=solver2, method=cvode, tolerance=0.0001)
## |-- UnitDefinitions:
## |-- |-- Unit: 1
## |-- |-- |-- BaseUnit:
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
##
## info:    model doesn't contain any continuous state
## info:    maximum step size for 'model.root': 0.001000
## info:    Result file: LinearTransformation2_res.mat (bufferSize=1)
## info: After simulation:
## info:    default.Gain1.y: 0.0
## info:    default.Add1.u1: 3.0
## info:    Final Statistics for 'model.root':
##          NumSteps = 1001 NumRhsEvals  = 1002 NumLinSolvSetups = 51
##          NumNonlinSolvIters = 1001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## endResult
