## status: correct
## teardown_command:
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes


from poplib import CR
from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# this example creates a composite model with FMI3 FMU and connects them together.
# It then simulates the model and prints the results.

model = SSP()
model.addResource("../resources/Feedthrough3.fmu", new_name="resources/Feedthrough3.fmu")
model.addComponent(CRef("default", "Feedthrough1"), "resources/Feedthrough3.fmu")
model.addComponent(CRef("default", "Feedthrough2"), "resources/Feedthrough3.fmu")


## connections
model.addConnection(CRef("default", "Feedthrough1", "Float64_continuous_output"), CRef("default", "Feedthrough2", "Float64_continuous_input"))
model.addConnection(CRef("default", "Feedthrough1", "Int64_output"), CRef("default", "Feedthrough2", "Int64_input"))

instantiated_model = model.instantiate()  ## internally generate the json file and also set the model state like virgin,
instantiated_model.setResultFile("")
instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.setValue(CRef("default", "Feedthrough1", "Float64_continuous_input"), 3.5)
instantiated_model.setValue(CRef("default", "Feedthrough1", "Int64_input"), 7)

instantiated_model.initialize()
instantiated_model.simulate()
print(f"info:    Feedthrough1.Float64_continuous_input  : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Float64_continuous_input'))}")
print(f"info:    Feedthrough1.Float64_continuous_output : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Float64_continuous_output'))}")
print(f"info:    Feedthrough2.Float64_continuous_input  : {instantiated_model.getValue(CRef('default', 'Feedthrough2', 'Float64_continuous_input'))}")
print(f"info:    Feedthrough2.Float64_continuous_output : {instantiated_model.getValue(CRef('default', 'Feedthrough2', 'Float64_continuous_output'))}")
print(f"info:    Feedthrough1.Int64_input               : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Int64_input'))}")
print(f"info:    Feedthrough1.Int64_output              : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Int64_output'))}")
print(f"info:    Feedthrough2.Int64_input               : {instantiated_model.getValue(CRef('default', 'Feedthrough2', 'Int64_input'))}")
print(f"info:    Feedthrough2.Int64_output              : {instantiated_model.getValue(CRef('default', 'Feedthrough2', 'Int64_output'))}")

instantiated_model.terminate()
instantiated_model.delete()

## Result:
## warning: Unknown FMI3 base type for var : Binary_input
## warning: Unknown FMI3 base type for var : Binary_output
## warning: Unknown FMI3 base type for var : Binary_input
## warning: Unknown FMI3 base type for var : Binary_output
## Loading FMI version 3...
## Loading FMI version 3...
## info:    No result file will be created
## info:    Feedthrough1.Float64_continuous_input  : 3.5
## info:    Feedthrough1.Float64_continuous_output : 3.5
## info:    Feedthrough2.Float64_continuous_input  : 3.5
## info:    Feedthrough2.Float64_continuous_output : 3.5
## info:    Feedthrough1.Int64_input               : 7
## info:    Feedthrough1.Int64_output              : 7
## info:    Feedthrough2.Int64_input               : 7
## info:    Feedthrough2.Int64_output              : 7
## info:    4 warnings
## info:    0 errors
## endResult
