## status: correct
## teardown_command:
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes


from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# this example creates a composite model with an FMI2 and an FMI3 FMU and connects them together.
# It then simulates the model and prints the results.

model = SSP()

## FMI2.0
model.addResource("../resources/Modelica.Blocks.Math.Gain.fmu", new_name="resources/Gain.fmu")
model.addComponent(CRef("default", "Gain1"), "resources/Gain.fmu")

## FMI3.0
model.addResource("../resources/Feedthrough3.fmu", new_name="resources/Feedthrough3.fmu")
model.addComponent(CRef("default", "Feedthrough1"), "resources/Feedthrough3.fmu")

## connections (Gain.y (FMI2) --> Feedthrough.Float64_continuous_input (FMI3))
model.addConnection(CRef("default", "Gain1", "y"), CRef("default", "Feedthrough1", "Float64_continuous_input"))

instantiated_model = model.instantiate()  ## internally generate the json file and also set the model state like virgin,
instantiated_model.setResultFile("")
instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.setValue(CRef("default", "Gain1", "u"), 15)
instantiated_model.initialize()
instantiated_model.simulate()

print(f"info:   default.Gain1.u : {instantiated_model.getValue(CRef('default', 'Gain1', 'u'))}")
print(f"info:   default.Gain1.y : {instantiated_model.getValue(CRef('default', 'Gain1', 'y'))}")
print(f"info:   default.Feedthrough1.Float64_continuous_input  : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Float64_continuous_input'))}")
print(f"info:   default.Feedthrough1.Float64_continuous_output : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Float64_continuous_output'))}")

instantiated_model.terminate()
instantiated_model.delete()


## Result:
## warning: Unknown FMI3 base type for var : Binary_input
## warning: Unknown FMI3 base type for var : Binary_output
## Loading FMI version 3...
## info:    No result file will be created
## info:   default.Gain1.u : 15.0
## info:   default.Gain1.y : 15.0
## info:   default.Feedthrough1.Float64_continuous_input  : 15.0
## info:   default.Feedthrough1.Float64_continuous_output : 15.0
## info:    2 warnings
## info:    0 errors
## endResult
