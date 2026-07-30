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
solver = {'name' : 'solver',  'method': 'cvode', 'tolerance': 1e-5}
model.newSolver(solver)
model.setSolver(CRef('default', 'Resource'), 'solver')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("Resource-me.mat")

instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

print(f"info:    Resource.y: {instantiated_model.getValue(CRef('default', 'Resource', 'y'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()

## Result:
## info:    model doesn't contain any continuous state
## info:    maximum step size for 'model.root': 0.001000
## info:    Result file: Resource-me.mat (bufferSize=10)
## info:    Resource.y: 97
## info:    Final Statistics for 'model.root':
##          NumSteps = 10001 NumRhsEvals  = 10002 NumLinSolvSetups = 501
##          NumNonlinSolvIters = 10001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## endResult
