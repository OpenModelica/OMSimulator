## status: correct
## teardown_command: rm -rf Feedthrough-cs.mat Feedthrough-me.mat
## linux: yes
## ucrt64: yes
## win: yes
## asan: no


from OMSimulator import SSP, CRef, Settings
Settings.suppressPath = True

model = SSP()
model.addResource('../resources/Feedthrough.fmu', new_name='resources/Feedthrough.fmu')

model.addComponent(CRef('default', 'Feedthrough'), 'resources/Feedthrough.fmu')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("Feedthrough-cs.mat")

instantiated_model.setValue(CRef('default', 'Feedthrough', 'Boolean_input'), True)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Enumeration_input'), 2)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Float64_continuous_input'), 3.1)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Float64_discrete_input'), 4.2)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Int32_input'), 5)

instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()
print(f"info:    Feedthrough.Boolean_output: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Boolean_output'))}", flush=True)
print(f"info:    Feedthrough.Enumeration_output: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Enumeration_output'))}", flush=True)
print(f"info:    Feedthrough.Float64_continuous_output: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Float64_continuous_output'))}", flush=True)
print(f"info:    Feedthrough.Float64_discrete_output: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Float64_discrete_output'))}", flush=True)
print(f"info:    Feedthrough.Int32_output: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Int32_output'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()


model2 = SSP()
model2.addResource('../resources/Feedthrough.fmu', new_name='resources/Feedthrough.fmu')

model2.addComponent(CRef('default', 'Feedthrough'), 'resources/Feedthrough.fmu')

solver2 = {'name' : 'solver2',  'method': 'cvode', 'tolerance': 1e-5}
model2.newSolver(solver2)
model2.setSolver(CRef('default', 'Feedthrough'), 'solver2')
instantiated_model = model2.instantiate()
instantiated_model.setResultFile("Feedthrough-me.mat")

instantiated_model.setValue(CRef('default', 'Feedthrough', 'Boolean_input'), True)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Enumeration_input'), 2)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Float64_continuous_input'), 3.1)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Float64_discrete_input'), 4.2)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Int32_input'), 5)

instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

print(f"info:    Feedthrough.Boolean_output: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Boolean_output'))}", flush=True)
print(f"info:    Feedthrough.Enumeration_output: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Enumeration_output'))}", flush=True)
print(f"info:    Feedthrough.Float64_continuous_output: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Float64_continuous_output'))}", flush=True)
print(f"info:    Feedthrough.Float64_discrete_output: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Float64_discrete_output'))}", flush=True)
print(f"info:    Feedthrough.Int32_output: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Int32_output'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()


## Result:
## info:    Result file: Feedthrough-cs.mat (bufferSize=1)
## info:    Parameter default.Feedthrough.String_parameter will not be stored in the result file, because the signal type is not supported
## info:    Feedthrough.Boolean_output: True
## info:    Feedthrough.Enumeration_output: 2
## info:    Feedthrough.Float64_continuous_output: 3.1
## info:    Feedthrough.Float64_discrete_output: 4.2
## info:    Feedthrough.Int32_output: 5
## info:    model doesn't contain any continuous state
## info:    maximum step size for 'model.root': 0.001000
## info:    Result file: Feedthrough-me.mat (bufferSize=1)
## info:    Parameter default.Feedthrough.String_parameter will not be stored in the result file, because the signal type is not supported
## info:    Feedthrough.Boolean_output: True
## info:    Feedthrough.Enumeration_output: 2
## info:    Feedthrough.Float64_continuous_output: 3.1
## info:    Feedthrough.Float64_discrete_output: 4.2
## info:    Feedthrough.Int32_output: 5
## info:    Final Statistics for 'model.root':
##          NumSteps = 10001 NumRhsEvals  = 10002 NumLinSolvSetups = 501
##          NumNonlinSolvIters = 10001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## endResult
