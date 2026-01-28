## status: correct
## teardown_command: rm -rf Resource-cs.mat
## linux: yes
## ucrt64: yes
## win: yes
## asan: no


from OMSimulator import SSP, CRef, Settings
Settings.suppressPath = True

model = SSP()
model.addResource('../../resources/Resource3.fmu', new_name='resources/Resource3.fmu')

model.addComponent(CRef('default', 'Resource'), 'resources/Resource3.fmu')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("Resource-cs.mat")

instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

print(f"info:    Resource.y: {instantiated_model.getValue(CRef('default', 'Resource', 'y'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()

model2 = SSP()
model2.addResource('../../resources/Resource3.fmu', new_name='resources/Resource3.fmu')

model2.addComponent(CRef('default', 'Resource'), 'resources/Resource3.fmu')

solver2 = {'name' : 'solver2',  'method': 'cvode', 'tolerance': 1e-4}
model2.newSolver(solver2)
model2.setSolver(CRef('default', 'Resource'), 'solver2')

instantiated_model = model2.instantiate()
instantiated_model.setResultFile("Resource-me.mat")

instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

print(f"info:    Resource.y: {instantiated_model.getValue(CRef('default', 'Resource', 'y'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()

## Result:
## Loading FMI version 3...
## info:    Result file: Resource-cs.mat (bufferSize=1)
## info:    Resource.y: 97
## Loading FMI version 3...
## info:    model doesn't contain any continuous state
## info:    maximum step size for 'model.root': 0.001000
## info:    Result file: Resource-me.mat (bufferSize=1)
## info:    Resource.y: 97
## info:    Final Statistics for 'model.root':
##          NumSteps = 10001 NumRhsEvals  = 10002 NumLinSolvSetups = 501
##          NumNonlinSolvIters = 10001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## endResult
