## status: correct
## teardown_command: rm -rf FeedthroughsetValue1.ssp
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes


from numpy import uint64
from OMSimulator import SSP, CRef, Settings, Float64, Float32, Int8, Int16, Int32, Int64, UInt8, UInt16, UInt32, UInt64

Settings.suppressPath = True

# this example creates a composite model with FMI3 FMU and connects them together.
# It then simulates the model and prints the results.

model = SSP()
model.addResource("../resources/Feedthrough3.fmu", new_name="resources/Feedthrough3.fmu")
model.addComponent(CRef("default", "Feedthrough1"), "resources/Feedthrough3.fmu")

model.setValue(CRef("default", "Feedthrough1", "Float64_continuous_input"), Float64(3.5))
model.setValue(CRef("default", "Feedthrough1", "Int64_input"), Int64(7))
model.setValue(CRef("default", "Feedthrough1", "Float32_continuous_input"), Float32(1.5))
model.setValue(CRef("default", "Feedthrough1", "Int8_input"), Int8(3))
model.setValue(CRef("default", "Feedthrough1", "Boolean_input"), True)

model.setValue(CRef("default", "Feedthrough1", "Float32_discrete_input"), Float32(2.5))
model.setValue(CRef("default", "Feedthrough1", "Enumeration_input"), 2)

model.setValue(CRef("default", "Feedthrough1", "Int16_input"), Int16(1000))
model.setValue(CRef("default", "Feedthrough1", "UInt16_input"), UInt16(2000))
model.setValue(CRef("default", "Feedthrough1", "UInt32_input"), UInt32(3000))
model.setValue(CRef("default", "Feedthrough1", "UInt64_input"), UInt64(4000))

#model.list()
model.export("FeedthroughsetValue1.ssp")

model2 = SSP("FeedthroughsetValue1.ssp")
model2.list()
# print("", flush=True)

# instantiated_model = model2.instantiate()  ## internally generate the json file and also set the model state like virgin,
# instantiated_model.setResultFile("")
# instantiated_model.setStopTime(10.0)
# instantiated_model.setTolerance(1e-5)

# # instantiated_model.setValue(CRef("default", "Feedthrough1", "Float64_continuous_input"), 3.5)
# # instantiated_model.setValue(CRef("default", "Feedthrough1", "Int64_input"), 7)

# instantiated_model.initialize()
# instantiated_model.simulate()
# print(f"info:    Feedthrough1.Float64_continuous_input  : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Float64_continuous_input'))}", flush=True)
# print(f"info:    Feedthrough1.Float64_continuous_output : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Float64_continuous_output'))}", flush=True)
# print(f"info:    Feedthrough2.Float64_continuous_input  : {instantiated_model.getValue(CRef('default', 'Feedthrough2', 'Float64_continuous_input'))}", flush=True)
# print(f"info:    Feedthrough2.Float64_continuous_output : {instantiated_model.getValue(CRef('default', 'Feedthrough2', 'Float64_continuous_output'))}", flush=True)
# print(f"info:    Feedthrough1.Int64_input               : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Int64_input'))}", flush=True)
# print(f"info:    Feedthrough1.Int64_output              : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Int64_output'))}", flush=True)
# print(f"info:    Feedthrough2.Int64_input               : {instantiated_model.getValue(CRef('default', 'Feedthrough2', 'Int64_input'))}", flush=True)
# print(f"info:    Feedthrough2.Int64_output              : {instantiated_model.getValue(CRef('default', 'Feedthrough2', 'Int64_output'))}", flush=True)

# instantiated_model.terminate()
# instantiated_model.delete()

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
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Float64 Float64_continuous_input, 3.5, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int64 Int64_input, 7, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float32 Float32_continuous_input, 1.5, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int8 Int8_input, 3, None, 'None')
## |-- |-- |-- |-- |-- |-- (Boolean Boolean_input, True, None, 'None')
## |-- |-- |-- |-- |-- |-- (Float32 Float32_discrete_input, 2.5, None, 'None')
## |-- |-- |-- |-- |-- |-- (Integer Enumeration_input, 2, None, 'None')
## |-- |-- |-- |-- |-- |-- (Int16 Int16_input, 1000, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt16 UInt16_input, 2000, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt32 UInt32_input, 3000, None, 'None')
## |-- |-- |-- |-- |-- |-- (UInt64 UInt64_input, 4000, None, 'None')
## |-- EnumerationDefinitions:
## |-- |-- EnumName: Option
## |-- |-- |-- EnumItems: Option 1: 1, Option 2: 2
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
