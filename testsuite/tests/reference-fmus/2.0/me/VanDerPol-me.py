## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## asan: yes

from OMSimulator import SSP, CRef, Settings, Capi
Settings.suppressPath = True

model = SSP()
model.addResource('../../../resources/VanDerPol.fmu', new_name='resources/VanDerPol.fmu')

model.addComponent(CRef('default', 'VanDerPol'), 'resources/VanDerPol.fmu')

solver = {'name' : 'solver',  'method': 'cvode', 'tolerance': 1e-5}
model.newSolver(solver)
model.setSolver(CRef('default', 'VanDerPol'), 'solver')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("VanDerPol-me.mat")

instantiated_model.setStopTime(20.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

instantiated_model.terminate()
instantiated_model.delete()

if 1 == Capi.compareSimulationResults("../../../references/VanDerPol-me.mat", "VanDerPol-me.mat", "default.VanDerPol.x0", "default.VanDerPol.x0", 1e-4, 1e-4):
  print("signal x0 is equal", flush=True)
else:
  print("signal x0 is not equal", flush=True)

if 1 == Capi.compareSimulationResults("../../../references/VanDerPol-me.mat", "VanDerPol-me.mat", "default.VanDerPol.x1", "default.VanDerPol.x1", 1e-4, 1e-4):
  print("signal x1 is equal", flush=True)
else:
  print("signal x1 is not equal", flush=True)

## Result:
## info:    maximum step size for 'model.root': 0.001000
## info:    Result file: VanDerPol-me.mat (bufferSize=10)
## info:    Final Statistics for 'model.root':
##          NumSteps = 20002 NumRhsEvals  = 20004 NumLinSolvSetups = 1002
##          NumNonlinSolvIters = 20003 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## signal x0 is equal
## signal x1 is equal
## endResult
