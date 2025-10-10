## status: correct
## teardown_command: rm -rf VanDerPol-cs.mat VanDerPol-me.mat
## linux: yes
## ucrt64: yes
## win: yes
## asan: yes


from OMSimulator import SSP, CRef, Settings, Capi
Settings.suppressPath = True

model = SSP()
model.addResource('../resources/VanDerPol.fmu', new_name='resources/VanDerPol.fmu')

model.addComponent(CRef('default', 'VanDerPol'), 'resources/VanDerPol.fmu')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("VanDerPol-cs.mat")

instantiated_model.setStopTime(20.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

instantiated_model.terminate()
instantiated_model.delete()

if 1 == Capi.compareSimulationResults("../references/VanDerPol-cs.mat", "VanDerPol-cs.mat", "model.root.VanDerPol.x0", "default.VanDerPol.x0", 1e-4, 1e-4):
  print("signal x0 is equal")
else:
  print("signal x0 is not equal")


if 1 == Capi.compareSimulationResults("../references/VanDerPol-cs.mat", "VanDerPol-cs.mat", "model.root.VanDerPol.x1", "default.VanDerPol.x1", 1e-4, 1e-4):
  print("signal x1 is equal")
else:
  print("signal x1 is not equal")


model2 = SSP()
model2.addResource('../resources/VanDerPol.fmu', new_name='resources/VanDerPol.fmu')

model2.addComponent(CRef('default', 'VanDerPol'), 'resources/VanDerPol.fmu')

solver2 = {'name' : 'solver2',  'method': 'cvode', 'tolerance': 1e-5}
model2.newSolver(solver2)
model2.setSolver(CRef('default', 'VanDerPol'), 'solver2')
print("",flush=True)

instantiated_model = model2.instantiate()
instantiated_model.setResultFile("VanDerPol-me.mat")

instantiated_model.setStopTime(20.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

instantiated_model.terminate()
instantiated_model.delete()

if 1 == Capi.compareSimulationResults("../references/VanDerPol-me.mat", "VanDerPol-me.mat", "model.root.VanDerPol.x0", "default.VanDerPol.x0", 1e-4, 1e-4):
  print("signal x0 is equal")
else:
  print("signal x0 is not equal")

if 1 == Capi.compareSimulationResults("../references/VanDerPol-me.mat", "VanDerPol-me.mat", "model.root.VanDerPol.x1", "default.VanDerPol.x1", 1e-4, 1e-4):
  print("signal x1 is equal")
else:
  print("signal x1 is not equal")


## Result:
## info:    Result file: VanDerPol-cs.mat (bufferSize=1)
## signal x0 is equal
## signal x1 is equal
##
## warning: Flag --suppressPath is set multiple times
## info:    maximum step size for 'model.root': 0.001000
## info:    Result file: VanDerPol-me.mat (bufferSize=1)
## info:    Final Statistics for 'model.root':
##          NumSteps = 20002 NumRhsEvals  = 20004 NumLinSolvSetups = 1002
##          NumNonlinSolvIters = 20003 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## signal x0 is equal
## signal x1 is equal
## info:    1 warnings
## info:    0 errors
## endResult
