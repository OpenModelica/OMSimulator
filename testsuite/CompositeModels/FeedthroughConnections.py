## status: correct
## teardown_command: rm -rf FeedthroughConnections.ssp
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes


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


model.export("FeedthroughConnections.ssp")

model2 = SSP("FeedthroughConnections.ssp")
model2.list()
print("", flush=True)

instantiated_model = model.instantiate()  ## internally generate the json file and also set the model state like virgin,
instantiated_model.setResultFile("")
instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.setValue(CRef("default", "Feedthrough1", "Float64_continuous_input"), 3.5)
instantiated_model.setValue(CRef("default", "Feedthrough1", "Int64_input"), 7)

instantiated_model.initialize()
instantiated_model.simulate()
print(f"info:    Feedthrough1.Float64_continuous_input  : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Float64_continuous_input'))}", flush=True)
print(f"info:    Feedthrough1.Float64_continuous_output : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Float64_continuous_output'))}", flush=True)
print(f"info:    Feedthrough2.Float64_continuous_input  : {instantiated_model.getValue(CRef('default', 'Feedthrough2', 'Float64_continuous_input'))}", flush=True)
print(f"info:    Feedthrough2.Float64_continuous_output : {instantiated_model.getValue(CRef('default', 'Feedthrough2', 'Float64_continuous_output'))}", flush=True)
print(f"info:    Feedthrough1.Int64_input               : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Int64_input'))}", flush=True)
print(f"info:    Feedthrough1.Int64_output              : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Int64_output'))}", flush=True)
print(f"info:    Feedthrough2.Int64_input               : {instantiated_model.getValue(CRef('default', 'Feedthrough2', 'Int64_input'))}", flush=True)
print(f"info:    Feedthrough2.Int64_output              : {instantiated_model.getValue(CRef('default', 'Feedthrough2', 'Int64_output'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Feedthrough3.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": None
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: Feedthrough1 'None'
## |-- |-- |-- |-- |-- path: resources/Feedthrough3.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (Float32_continuous_input, Causality.input, SignalType.Float32, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float32_continuous_output, Causality.output, SignalType.Float32, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float32_discrete_input, Causality.input, SignalType.Float32, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float32_discrete_output, Causality.output, SignalType.Float32, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float64_fixed_parameter, Causality.parameter, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float64_tunable_parameter, Causality.parameter, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float64_continuous_input, Causality.input, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float64_continuous_output, Causality.output, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float64_discrete_input, Causality.input, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float64_discrete_output, Causality.output, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int8_input, Causality.input, SignalType.Int8, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int8_output, Causality.output, SignalType.Int8, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt8_input, Causality.input, SignalType.UInt8, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt8_output, Causality.output, SignalType.UInt8, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int16_input, Causality.input, SignalType.Int16, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int16_output, Causality.output, SignalType.Int16, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt16_input, Causality.input, SignalType.UInt16, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt16_output, Causality.output, SignalType.UInt16, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int32_input, Causality.input, SignalType.Int32, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int32_output, Causality.output, SignalType.Int32, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt32_input, Causality.input, SignalType.UInt32, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt32_output, Causality.output, SignalType.UInt32, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int64_input, Causality.input, SignalType.Int64, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int64_output, Causality.output, SignalType.Int64, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt64_input, Causality.input, SignalType.UInt64, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt64_output, Causality.output, SignalType.UInt64, None, 'None')
## |-- |-- |-- |-- |-- |-- (Boolean_input, Causality.input, SignalType.Boolean, None, 'None')
## |-- |-- |-- |-- |-- |-- (Boolean_output, Causality.output, SignalType.Boolean, None, 'None')
## |-- |-- |-- |-- |-- |-- (String_parameter, Causality.parameter, SignalType.String, None, 'None')
## |-- |-- |-- |-- |-- |-- (Binary_input, Causality.input, SignalType.Binary, None, 'None')
## |-- |-- |-- |-- |-- |-- (Binary_output, Causality.output, SignalType.Binary, None, 'None')
## |-- |-- |-- |-- |-- |-- (Enumeration_input, Causality.input, SignalType.Enumeration, None, 'None')
## |-- |-- |-- |-- |-- |-- (Enumeration_output, Causality.output, SignalType.Enumeration, None, 'None')
## |-- |-- |-- |-- FMU: Feedthrough2 'None'
## |-- |-- |-- |-- |-- path: resources/Feedthrough3.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (Float32_continuous_input, Causality.input, SignalType.Float32, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float32_continuous_output, Causality.output, SignalType.Float32, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float32_discrete_input, Causality.input, SignalType.Float32, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float32_discrete_output, Causality.output, SignalType.Float32, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float64_fixed_parameter, Causality.parameter, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float64_tunable_parameter, Causality.parameter, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float64_continuous_input, Causality.input, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float64_continuous_output, Causality.output, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float64_discrete_input, Causality.input, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float64_discrete_output, Causality.output, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int8_input, Causality.input, SignalType.Int8, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int8_output, Causality.output, SignalType.Int8, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt8_input, Causality.input, SignalType.UInt8, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt8_output, Causality.output, SignalType.UInt8, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int16_input, Causality.input, SignalType.Int16, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int16_output, Causality.output, SignalType.Int16, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt16_input, Causality.input, SignalType.UInt16, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt16_output, Causality.output, SignalType.UInt16, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int32_input, Causality.input, SignalType.Int32, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int32_output, Causality.output, SignalType.Int32, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt32_input, Causality.input, SignalType.UInt32, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt32_output, Causality.output, SignalType.UInt32, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int64_input, Causality.input, SignalType.Int64, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int64_output, Causality.output, SignalType.Int64, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt64_input, Causality.input, SignalType.UInt64, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt64_output, Causality.output, SignalType.UInt64, None, 'None')
## |-- |-- |-- |-- |-- |-- (Boolean_input, Causality.input, SignalType.Boolean, None, 'None')
## |-- |-- |-- |-- |-- |-- (Boolean_output, Causality.output, SignalType.Boolean, None, 'None')
## |-- |-- |-- |-- |-- |-- (String_parameter, Causality.parameter, SignalType.String, None, 'None')
## |-- |-- |-- |-- |-- |-- (Binary_input, Causality.input, SignalType.Binary, None, 'None')
## |-- |-- |-- |-- |-- |-- (Binary_output, Causality.output, SignalType.Binary, None, 'None')
## |-- |-- |-- |-- |-- |-- (Enumeration_input, Causality.input, SignalType.Enumeration, None, 'None')
## |-- |-- |-- |-- |-- |-- (Enumeration_output, Causality.output, SignalType.Enumeration, None, 'None')
## |-- |-- |-- Connections:
## |-- |-- |-- |-- Feedthrough1.Float64_continuous_output -> Feedthrough2.Float64_continuous_input
## |-- |-- |-- |-- Feedthrough1.Int64_output -> Feedthrough2.Int64_input
## |-- EnumerationDefinitions:
## |-- |-- EnumName: Option
## |-- |-- |-- EnumItems: Option 1: 1, Option 2: 2
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
##
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
