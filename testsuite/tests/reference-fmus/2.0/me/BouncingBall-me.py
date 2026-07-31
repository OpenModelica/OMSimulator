## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## asan: yes

from OMSimulator import SSP, CRef, Settings, Capi
Settings.suppressPath = True

model = SSP()
model.addResource('../../../resources/BouncingBall.fmu', new_name='resources/BouncingBall.fmu')
model.addComponent(CRef('default', 'BouncingBall'), 'resources/BouncingBall.fmu')

solver = {'name' : 'solver',  'method': 'cvode', 'relativeTolerance': 1e-5}
model.newSolver(solver)
model.setSolver(CRef('default', 'BouncingBall'), 'solver')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("BouncingBall-me.mat")

instantiated_model.setStopTime(3.0)

instantiated_model.initialize()
instantiated_model.simulate()

instantiated_model.terminate()
instantiated_model.delete()

if 1 == Capi.compareSimulationResults("../../../references/BouncingBall-me.mat", "BouncingBall-me.mat", "default.BouncingBall.h", "default.BouncingBall.h", 1e-4, 1e-4):
  print("signal h is equal", flush=True)
else:
  print("signal h is not equal", flush=True)

if 1 == Capi.compareSimulationResults("../../../references/BouncingBall-me.mat", "BouncingBall-me.mat", "default.BouncingBall.v", "default.BouncingBall.v", 1e-4, 1e-4):
  print("signal v is equal", flush=True)
else:
  print("signal v is not equal", flush=True)

## Result:
## info:    maximum step size for 'model.root': 0.001000
## info:    Result file: BouncingBall-me.mat (bufferSize=10)
## info:    Final Statistics for 'model.root':
##          NumSteps = 503 NumRhsEvals  = 504 NumLinSolvSetups = 27
##          NumNonlinSolvIters = 503 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## signal h is equal
## signal v is equal
## endResult
