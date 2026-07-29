## status: correct
## teardown_command: rm -rf BouncingBall-me3.mat
## linux: yes
## ucrt64: yes
## win: yes
## asan: no

from OMSimulator import SSP, CRef, Settings, Capi
Settings.suppressPath = True

model = SSP()
model.addResource('../../../resources/BouncingBall3.fmu', new_name='resources/BouncingBall3.fmu')
model.addComponent(CRef('default', 'BouncingBall'), 'resources/BouncingBall3.fmu')

solver = {'name' : 'solver',  'method': 'cvode', 'tolerance': 1e-4}
model.newSolver(solver)
model.setSolver(CRef('default', 'BouncingBall'), 'solver')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("BouncingBall-me3.mat")
instantiated_model.setStopTime(3.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

instantiated_model.terminate()
instantiated_model.delete()

if 1 == Capi.compareSimulationResults("../../../references/BouncingBall-me.mat", "BouncingBall-me3.mat", "default.BouncingBall.h", "default.BouncingBall.h",1e-4, 1e-4):
  print("signal h is equal")
else:
  print("signal h is not equal")

if 1 == Capi.compareSimulationResults("../../../references/BouncingBall-me.mat", "BouncingBall-me3.mat", "default.BouncingBall.v", "default.BouncingBall.v",1e-4, 1e-4):
  print("signal v is equal")
else:
  print("signal v is not equal")

## Result:
## Loading FMI version 3...
## info:    maximum step size for 'model.root': 0.001000
## info:    Result file: BouncingBall-me3.mat (bufferSize=10)
## info:    Final Statistics for 'model.root':
##          NumSteps = 503 NumRhsEvals  = 504 NumLinSolvSetups = 27
##          NumNonlinSolvIters = 503 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## signal h is equal
## signal v is equal
## endResult
