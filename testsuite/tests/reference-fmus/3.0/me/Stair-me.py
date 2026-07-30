## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## asan: no

from OMSimulator import SSP, CRef, Settings, Capi
Settings.suppressPath = True

model = SSP()
model.addResource('../../../resources/Stair3.fmu', new_name='resources/Stair3.fmu')

model.addComponent(CRef('default', 'Stair'), 'resources/Stair3.fmu')

solver = {'name' : 'solver',  'method': 'cvode', 'tolerance': 1e-4}
model.newSolver(solver)
model.setSolver(CRef('default', 'Stair'), 'solver')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("Stair-me3.mat")

instantiated_model.setFixedStepSize(0.2)
instantiated_model.setStopTime(9.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

instantiated_model.terminate()
instantiated_model.delete()

if 1 == Capi.compareSimulationResults("../../../references/Stair-me.mat", "Stair-me3.mat", "default.Stair.counter", "default.Stair.counter", 1e-4, 1e-4):
  print("signal counter is equal", flush=True)
else:
  print("signal counter is not equal", flush=True)

## Result:
## Loading FMI version 3...
## info:    model doesn't contain any continuous state
## info:    maximum step size for 'model.root': 0.200000
## info:    Result file: Stair-me3.mat (bufferSize=10)
## info:    Simulation terminated by FMU model.root.Stair at time 9.000000
## info:    Final Statistics for 'model.root':
##          NumSteps = 0 NumRhsEvals  = 0 NumLinSolvSetups = 0
##          NumNonlinSolvIters = 0 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## signal counter is equal
## endResult
