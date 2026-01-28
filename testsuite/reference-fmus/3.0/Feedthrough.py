## status: correct
## teardown_command:
## linux: yes
## ucrt64: yes
## win: yes
## asan: no


from OMSimulator import SSP, CRef, Settings
Settings.suppressPath = True

model = SSP()
model.addResource('../../resources/Feedthrough3.fmu', new_name='resources/Feedthrough3.fmu')

model.addComponent(CRef('default', 'Feedthrough'), 'resources/Feedthrough3.fmu')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("")
instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.setValue(CRef('default', 'Feedthrough', 'Float32_continuous_input'), 3.1)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Float64_continuous_input'), 3.3)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Int8_input'), 4)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Int16_input'), 5)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Int32_input'), 6)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Int64_input'), 7)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'UInt8_input'), 8)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'UInt16_input'), 9)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'UInt32_input'), 10)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'UInt64_input'), 11)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Boolean_input'), True)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Enumeration_input'), 2)

## need to implement fmi3causality table in fmi4c library if inital attribute is not present
## instantiated_model.setValue(CRef('default', 'Feedthrough', 'String_parameter'), "Set me!")

instantiated_model.initialize()
instantiated_model.simulate()

# --- Print all outputs and values that were set ---
print(f"info:    Feedthrough.Float32_continuous_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Float32_continuous_input'))}", flush=True)
print(f"info:    Feedthrough.Float64_continuous_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Float64_continuous_input'))}", flush=True)
print(f"info:    Feedthrough.Int8_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Int8_input'))}", flush=True)
print(f"info:    Feedthrough.Int16_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Int16_input'))}", flush=True)
print(f"info:    Feedthrough.Int32_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Int32_input'))}", flush=True)
print(f"info:    Feedthrough.Int64_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Int64_input'))}", flush=True)
print(f"info:    Feedthrough.UInt8_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'UInt8_input'))}", flush=True)
print(f"info:    Feedthrough.UInt16_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'UInt16_input'))}", flush=True)
print(f"info:    Feedthrough.UInt32_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'UInt32_input'))}", flush=True)
print(f"info:    Feedthrough.UInt64_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'UInt64_input'))}", flush=True)
print(f"info:    Feedthrough.Boolean_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Boolean_input'))}", flush=True)
print(f"info:    Feedthrough.Enumeration_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Enumeration_input'))}", flush=True)
print(f"info:    Feedthrough.String_parameter: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'String_parameter'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()


model2 = SSP()
model2.addResource('../../resources/Feedthrough3.fmu', new_name='resources/Feedthrough3.fmu')

model2.addComponent(CRef('default', 'Feedthrough'), 'resources/Feedthrough3.fmu')

solver2 = {'name' : 'solver2',  'method': 'cvode', 'tolerance': 1e-4}
model2.newSolver(solver2)
model2.setSolver(CRef('default', 'Feedthrough'), 'solver2')

instantiated_model = model2.instantiate()
instantiated_model.setResultFile("")
instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.setValue(CRef('default', 'Feedthrough', 'Float32_continuous_input'), 3.1)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Float64_continuous_input'), 3.3)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Int8_input'), 4)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Int16_input'), 5)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Int32_input'), 6)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Int64_input'), 7)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'UInt8_input'), 8)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'UInt16_input'), 9)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'UInt32_input'), 10)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'UInt64_input'), 11)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Boolean_input'), True)
instantiated_model.setValue(CRef('default', 'Feedthrough', 'Enumeration_input'), 2)

## need to implement fmi3causality table in fmi4c library if inital attribute is not present
## instantiated_model.setValue(CRef('default', 'Feedthrough', 'String_parameter'), "Set me!")

instantiated_model.initialize()
instantiated_model.simulate()

# --- Print all outputs and values that were set ---
print(f"info:    Feedthrough.Float32_continuous_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Float32_continuous_input'))}", flush=True)
print(f"info:    Feedthrough.Float64_continuous_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Float64_continuous_input'))}", flush=True)
print(f"info:    Feedthrough.Int8_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Int8_input'))}", flush=True)
print(f"info:    Feedthrough.Int16_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Int16_input'))}", flush=True)
print(f"info:    Feedthrough.Int32_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Int32_input'))}", flush=True)
print(f"info:    Feedthrough.Int64_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Int64_input'))}", flush=True)
print(f"info:    Feedthrough.UInt8_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'UInt8_input'))}", flush=True)
print(f"info:    Feedthrough.UInt16_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'UInt16_input'))}", flush=True)
print(f"info:    Feedthrough.UInt32_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'UInt32_input'))}", flush=True)
print(f"info:    Feedthrough.UInt64_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'UInt64_input'))}", flush=True)
print(f"info:    Feedthrough.Boolean_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Boolean_input'))}", flush=True)
print(f"info:    Feedthrough.Enumeration_input: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'Enumeration_input'))}", flush=True)
print(f"info:    Feedthrough.String_parameter: {instantiated_model.getValue(CRef('default', 'Feedthrough', 'String_parameter'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()


## Result:
## warning: Unknown FMI3 base type for var : Binary_input
## warning: Unknown FMI3 base type for var : Binary_output
## Loading FMI version 3...
## info:    No result file will be created
## info:    Feedthrough.Float32_continuous_input: 3.0999999046325684
## info:    Feedthrough.Float64_continuous_input: 3.3
## info:    Feedthrough.Int8_input: 4
## info:    Feedthrough.Int16_input: 5
## info:    Feedthrough.Int32_input: 6
## info:    Feedthrough.Int64_input: 7
## info:    Feedthrough.UInt8_input: 8
## info:    Feedthrough.UInt16_input: 9
## info:    Feedthrough.UInt32_input: 10
## info:    Feedthrough.UInt64_input: 11
## info:    Feedthrough.Boolean_input: True
## info:    Feedthrough.Enumeration_input: 2
## info:    Feedthrough.String_parameter: Set me!
## warning: Unknown FMI3 base type for var : Binary_input
## warning: Unknown FMI3 base type for var : Binary_output
## Loading FMI version 3...
## info:    model doesn't contain any continuous state
## info:    maximum step size for 'model.root': 0.001000
## info:    No result file will be created
## info:    Feedthrough.Float32_continuous_input: 3.0999999046325684
## info:    Feedthrough.Float64_continuous_input: 3.3
## info:    Feedthrough.Int8_input: 4
## info:    Feedthrough.Int16_input: 5
## info:    Feedthrough.Int32_input: 6
## info:    Feedthrough.Int64_input: 7
## info:    Feedthrough.UInt8_input: 8
## info:    Feedthrough.UInt16_input: 9
## info:    Feedthrough.UInt32_input: 10
## info:    Feedthrough.UInt64_input: 11
## info:    Feedthrough.Boolean_input: True
## info:    Feedthrough.Enumeration_input: 2
## info:    Feedthrough.String_parameter: Set me!
## info:    Final Statistics for 'model.root':
##          NumSteps = 10001 NumRhsEvals  = 10002 NumLinSolvSetups = 501
##          NumNonlinSolvIters = 10001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## info:    4 warnings
## info:    0 errors
## endResult
