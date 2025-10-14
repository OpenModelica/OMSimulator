## status: correct
## teardown_command: rm -rf VanDerPol-cs.mat
## linux: yes
## ucrt64: yes
## win: yes
## asan: no


from OMSimulator import SSP, CRef, Settings, Capi
Settings.suppressPath = True

model = SSP()
model.addResource('../../resources/VanDerPol3.fmu', new_name='resources/VanDerPol3.fmu')

model.addComponent(CRef('default', 'VanDerPol'), 'resources/VanDerPol3.fmu')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("VanDerPol-cs.mat")

instantiated_model.setStopTime(20.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

instantiated_model.terminate()
instantiated_model.delete()

if 1 == Capi.compareSimulationResults("../../references/VanDerPol-cs.mat", "VanDerPol-cs.mat", "model.root.VanDerPol.x0", "default.VanDerPol.x0", 1e-4, 1e-4):
  print("signal x0 is equal", flush=True)
else:
  print("signal x0 is not equal", flush=True)


if 1 == Capi.compareSimulationResults("../../references/VanDerPol-cs.mat", "VanDerPol-cs.mat", "model.root.VanDerPol.x1", "default.VanDerPol.x1", 1e-4, 1e-4):
  print("signal x1 is equal", flush=True)
else:
  print("signal x1 is not equal", flush=True)

## Result:
## Loading FMI version 3...
## info:    Result file: VanDerPol-cs.mat (bufferSize=1)
## signal x0 is equal
## signal x1 is equal
## endResult
