## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## asan: yes

from OMSimulator import SSP, CRef, Settings, Capi
Settings.suppressPath = True
Capi.setCommandLineOption("--stripRoot=true")

model = SSP()
model.addResource('../../../resources/Stair.fmu', new_name='resources/Stair.fmu')

model.addComponent(CRef('default', 'Stair'), 'resources/Stair.fmu')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("Stair-cs.mat")

instantiated_model.setFixedStepSize(0.2)
instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

instantiated_model.terminate()
instantiated_model.delete()

if 1 == Capi.compareSimulationResults("../../../references/Stair-cs.mat", "Stair-cs.mat", "default.Stair.counter", "default.Stair.counter", 1e-4, 1e-4):
  print("signal counter is equal", flush=True)
else:
  print("signal counter is not equal", flush=True)

## Result:
## info:    Result file: Stair-cs.mat (bufferSize=10)
## info:    fmi2_doStep discarded for FMU "model.root.Stair"
## signal counter is equal
## endResult
