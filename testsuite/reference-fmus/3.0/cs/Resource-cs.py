## status: correct
## teardown_command: rm -rf Resource-cs3.mat
## linux: yes
## ucrt64: yes
## win: yes
## asan: no

from OMSimulator import SSP, CRef, Settings
Settings.suppressPath = True

model = SSP()
model.addResource('../../../resources/Resource3.fmu', new_name='resources/Resource3.fmu')

model.addComponent(CRef('default', 'Resource'), 'resources/Resource3.fmu')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("Resource-cs3.mat")

instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

print(f"info:    Resource.y: {instantiated_model.getValue(CRef('default', 'Resource', 'y'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()

## Result:
## Loading FMI version 3...
## info:    Result file: Resource-cs3.mat (bufferSize=10)
## info:    Resource.y: 97
## endResult
