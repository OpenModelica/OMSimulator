## status: correct
## teardown_command: rm -rf FeedthroughsetValue2.ssp feed_through_parameters1.ssv
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes


from numpy import uint64
from OMSimulator import SSP, SSV, CRef, Settings, Float64, Float32, Int8, Int16, Int32, Int64, UInt8, UInt16, UInt32, UInt64

Settings.suppressPath = True

# this example creates a composite model with FMI3 FMU and connects them together.
# It then simulates the model and prints the results.

model = SSP()
model.addResource("../resources/Feedthrough3.fmu", new_name="resources/Feedthrough3.fmu")
model.addComponent(CRef("default", "Feedthrough1"), "resources/Feedthrough3.fmu")

ssv = SSV()

ssv.setValue("Feedthrough1.Float64_continuous_input", Float64(3.5))
ssv.setValue("Feedthrough1.Int64_input", Int64(7))
ssv.setValue("Feedthrough1.Float32_continuous_input", Float32(1.5))
ssv.setValue("Feedthrough1.Int8_input", Int8(3))
ssv.setValue("Feedthrough1.Boolean_input", True)
ssv.setValue("Feedthrough1.Float32_discrete_input", Float32(2.5))
ssv.setValue("Feedthrough1.Enumeration_input", 2)
ssv.setValue("Feedthrough1.Int16_input", Int16(1000))
ssv.setValue("Feedthrough1.UInt16_input", UInt16(2000))
ssv.setValue("Feedthrough1.UInt32_input", UInt32(3000))
ssv.setValue("Feedthrough1.UInt64_input", UInt64(4000))
ssv.export("feed_through_parameters1.ssv")

## add ssv to model resources
model.addResource("feed_through_parameters1.ssv", "resources/feed_through_parameters1.ssv")
## reference ssv to Feedthrough1
model.addSSVReference(CRef("default", "Feedthrough1"), "resources/feed_through_parameters1.ssv")

model.export("FeedthroughsetValue2.ssp")

model2 = SSP("FeedthroughsetValue2.ssp")
model2.list()
print("", flush=True)

instantiated_model = model2.instantiate()  ## internally generate the json file and also set the model state like virgin,
instantiated_model.setResultFile("")
instantiated_model.setStopTime(10.0)
instantiated_model.setTolerance(1e-5)

# print(instantiated_model.dumpApiCalls(), flush=True)
instantiated_model.initialize()
instantiated_model.simulate()

print(f"info:    Feedthrough1.Float64_continuous_input : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Float64_continuous_input'))}", flush=True)
print(f"info:    Feedthrough1.Int64_input              : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Int64_input'))}", flush=True)
print(f"info:    Feedthrough1.Float32_continuous_input : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Float32_continuous_input'))}", flush=True)
print(f"info:    Feedthrough1.Int8_input               : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Int8_input'))}", flush=True)
print(f"info:    Feedthrough1.Boolean_input            : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Boolean_input'))}", flush=True)
print(f"info:    Feedthrough1.Float32_discrete_input   : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Float32_discrete_input'))}", flush=True)
print(f"info:    Feedthrough1.Enumeration_input        : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Enumeration_input'))}", flush=True)
print(f"info:    Feedthrough1.Int16_input              : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'Int16_input'))}", flush=True)
print(f"info:    Feedthrough1.UInt16_input             : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'UInt16_input'))}", flush=True)
print(f"info:    Feedthrough1.UInt32_input             : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'UInt32_input'))}", flush=True)
print(f"info:    Feedthrough1.UInt64_input             : {instantiated_model.getValue(CRef('default', 'Feedthrough1', 'UInt64_input'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Feedthrough3.fmu
## |--   resources/feed_through_parameters1.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Float64 Feedthrough1.Float64_continuous_input, 3.5, None, 'None')
## |--   |-- |-- (Int64 Feedthrough1.Int64_input, 7, None, 'None')
## |--   |-- |-- (Float32 Feedthrough1.Float32_continuous_input, 1.5, None, 'None')
## |--   |-- |-- (Int8 Feedthrough1.Int8_input, 3, None, 'None')
## |--   |-- |-- (Boolean Feedthrough1.Boolean_input, True, None, 'None')
## |--   |-- |-- (Float32 Feedthrough1.Float32_discrete_input, 2.5, None, 'None')
## |--   |-- |-- (Integer Feedthrough1.Enumeration_input, 2, None, 'None')
## |--   |-- |-- (Int16 Feedthrough1.Int16_input, 1000, None, 'None')
## |--   |-- |-- (UInt16 Feedthrough1.UInt16_input, 2000, None, 'None')
## |--   |-- |-- (UInt32 Feedthrough1.UInt32_input, 3000, None, 'None')
## |--   |-- |-- (UInt64 Feedthrough1.UInt64_input, 4000, None, 'None')
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
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
## |-- |-- |-- |-- |-- Parameter Bindings: resources/feed_through_parameters1.ssv
## |-- EnumerationDefinitions:
## |-- |-- EnumName: Option
## |-- |-- |-- EnumItems: Option 1: 1, Option 2: 2
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
##
## warning: Unknown FMI3 base type for var : Binary_input
## warning: Unknown FMI3 base type for var : Binary_output
## Loading FMI version 3...
## info:    No result file will be created
## info:    Feedthrough1.Float64_continuous_input : 3.5
## info:    Feedthrough1.Int64_input              : 7
## info:    Feedthrough1.Float32_continuous_input : 1.5
## info:    Feedthrough1.Int8_input               : 3
## info:    Feedthrough1.Boolean_input            : True
## info:    Feedthrough1.Float32_discrete_input   : 2.5
## info:    Feedthrough1.Enumeration_input        : 2
## info:    Feedthrough1.Int16_input              : 1000
## info:    Feedthrough1.UInt16_input             : 2000
## info:    Feedthrough1.UInt32_input             : 3000
## info:    Feedthrough1.UInt64_input             : 4000
## info:    2 warnings
## info:    0 errors
## endResult
