## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## asan: yes

from OMSimulator import SSP, CRef, Settings, Capi
Settings.suppressPath = True

model = SSP()
model.addResource('../../../resources/Dahlquist3.fmu', new_name='resources/Dahlquist3.fmu')

model.addComponent(CRef('default', 'Dahlquist'), 'resources/Dahlquist3.fmu')

solver = {'name' : 'solver',  'method': 'cvode', 'tolerance': 1e-4}
model.newSolver(solver)
model.setSolver(CRef('default', 'Dahlquist'), 'solver')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("Dahlquist-me3.mat")

instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

instantiated_model.terminate()
instantiated_model.delete()

if 1 == Capi.compareSimulationResults("../../../references/Dahlquist-me.mat", "Dahlquist-me3.mat", "default.Dahlquist.x", "default.Dahlquist.x", 1e-4, 1e-4):
  print("signal x is equal", flush=True)
else:
  print("signal x is not equal", flush=True)

## Result:
## Loading FMI version 3...
## info:    maximum step size for 'model.root': 0.001000
## info:    Result file: Dahlquist-me3.mat (bufferSize=10)
## info:    Final Statistics for 'model.root':
##          NumSteps = 10001 NumRhsEvals  = 10002 NumLinSolvSetups = 501
##          NumNonlinSolvIters = 10001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## signal x is equal
## endResult
