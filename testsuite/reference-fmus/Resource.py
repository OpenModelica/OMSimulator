## status: correct
## teardown_command: rm -rf Resource-cs.mat Resource-me.mat
## linux: yes
## ucrt64: yes
## win: yes
## asan: yes


from OMSimulator import SSP, CRef, Settings, Capi
Settings.suppressPath = True

model = SSP()
model.addResource('../resources/Resource.fmu', new_name='resources/Resource.fmu')

model.addComponent(CRef('default', 'Resource'), 'resources/Resource.fmu')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("Resource-cs.mat")

instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

print(f"info:    Resource.y: {instantiated_model.getValue(CRef('default', 'Resource', 'y'))}")

instantiated_model.terminate()
instantiated_model.delete()


model2 = SSP()
model2.addResource('../resources/Resource.fmu', new_name='resources/Resource.fmu')

model2.addComponent(CRef('default', 'Resource'), 'resources/Resource.fmu')
solver2 = {'name' : 'solver2',  'method': 'cvode', 'tolerance': 1e-5}
model2.newSolver(solver2)
model2.setSolver(CRef('default', 'Resource'), 'solver2')

print("",flush=True)
instantiated_model = model2.instantiate()
instantiated_model.setResultFile("Resource-me.mat")

instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

print(f"info:    Resource.y: {instantiated_model.getValue(CRef('default', 'Resource', 'y'))}")

instantiated_model.terminate()
instantiated_model.delete()

## Result:
## info:    Result file: Resource-cs.mat (bufferSize=1)
## info:    Resource.y: 97
##
## warning: Flag --suppressPath is set multiple times
## info:    model doesn't contain any continuous state
## info:    maximum step size for 'model.root': 0.001000
## info:    Result file: Resource-me.mat (bufferSize=1)
## info:    Final Statistics for 'model.root':
##          NumSteps = 10001 NumRhsEvals  = 10002 NumLinSolvSetups = 501
##          NumNonlinSolvIters = 10001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## info:    Resource.y: 97
## info:    1 warnings
## info:    0 errors
## endResult
