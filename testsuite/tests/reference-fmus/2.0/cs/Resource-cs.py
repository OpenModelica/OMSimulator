## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## asan: no

from OMSimulator import SSP, CRef, Settings
Settings.suppressPath = True

model = SSP()
model.addResource('../../../resources/Resource.fmu', new_name='resources/Resource.fmu')

model.addComponent(CRef('default', 'Resource'), 'resources/Resource.fmu')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("Resource-cs.mat")

instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

print(f"info:    Resource.y: {instantiated_model.getValue(CRef('default', 'Resource', 'y'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()

## Result:
## info:    Result file: Resource-cs.mat (bufferSize=10)
## info:    Resource.y: 97
## endResult
