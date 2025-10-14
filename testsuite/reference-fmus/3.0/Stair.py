## status: correct
## teardown_command: rm -rf Stair-cs.mat
## linux: yes
## ucrt64: yes
## win: yes
## asan: no

from OMSimulator import SSP, CRef, Settings, Capi
Settings.suppressPath = True

model = SSP()
model.addResource('../../resources/Stair3.fmu', new_name='resources/Stair3.fmu')

model.addComponent(CRef('default', 'Stair'), 'resources/Stair3.fmu')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("Stair-cs.mat")

instantiated_model.setFixedStepSize(0.2)
instantiated_model.setStopTime(9.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

instantiated_model.terminate()
instantiated_model.delete()



if 1 == Capi.compareSimulationResults("../../references/Stair-cs.mat", "Stair-cs.mat", "model.root.Stair.counter", "default.Stair.counter", 1e-4, 1e-4):
  print("signal counter is equal", flush=True)
else:
  print("signal counter is not equal", flush=True)

## Result:
## Loading FMI version 3...
## info:    Result file: Stair-cs.mat (bufferSize=1)
## signal counter is equal
## endResult
