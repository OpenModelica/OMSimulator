## status: correct
## teardown_command: rm -rf Dahlquist-cs.mat Dahlquist-me.mat
## linux: yes
## ucrt64: yes
## win: yes
## asan: yes

from OMSimulator import SSP, CRef, Settings, Capi
Settings.suppressPath = True

model = SSP()
model.addResource('../resources/Dahlquist.fmu', new_name='resources/Dahlquist.fmu')

model.addComponent(CRef('default', 'Dahlquist'), 'resources/Dahlquist.fmu')

print("",flush=True)
instantiated_model = model.instantiate()
instantiated_model.setResultFile("Dahlquist-cs.mat")

instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

instantiated_model.terminate()
instantiated_model.delete()


if 1 == Capi.compareSimulationResults("../references/Dahlquist-cs.mat", "Dahlquist-cs.mat", "model.root.Dahlquist.x", "default.Dahlquist.x", 1e-4, 1e-4):
  print("signal x is equal")
else:
  print("signal x is not equal")


model2 = SSP()
model2.addResource('../resources/Dahlquist.fmu', new_name='resources/Dahlquist.fmu')
model2.addComponent(CRef('default', 'Dahlquist'), 'resources/Dahlquist.fmu')

solver2 = {'name' : 'solver2',  'method': 'cvode', 'tolerance': 1e-5}
model2.newSolver(solver2)
model2.setSolver(CRef('default', 'Dahlquist'), 'solver2')

print("",flush=True)
instantiated_model = model.instantiate()

instantiated_model.setResultFile("Dahlquist-me.mat")
instantiated_model.setStopTime(10.0)
#instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

instantiated_model.terminate()
instantiated_model.delete()

if 1 == Capi.compareSimulationResults("../references/Dahlquist-me.mat", "Dahlquist-me.mat", "model.root.Dahlquist.x", "default.Dahlquist.x", 1e-4, 1e-4):
  print("signal x is equal")
else:
  print("signal x is not equal")


## Result:
##
## info:    Result file: Dahlquist-cs.mat (bufferSize=1)
## signal x is equal
##
## warning: Flag --suppressPath is set multiple times
## info:    Result file: Dahlquist-me.mat (bufferSize=1)
## error:   [compareSeries] ResultReader::compareSeries: different values at time 0.001000
##          valueA: 0.999001, valueB: 1.000000
## signal x is not equal
## info:    1 warnings
## info:    1 errors
## endResult
