-- status: correct
-- teardown_command: rm -rf Feedthrough3-lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: yes
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./Feedthrough3-lua/")
oms_setWorkingDirectory("./Feedthrough3-lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.Feedthrough", "../../../resources/Feedthrough3.fmu")

oms_setResultFile("model", "")
oms_setStopTime("model", 10.0)
oms_setTolerance("model.root", 1e-5)

oms_instantiate("model")

oms_setReal("model.root.Feedthrough.Float32_continuous_input", 3.1)
oms_setReal("model.root.Feedthrough.Float64_continuous_input", 3.3)
oms_setInteger("model.root.Feedthrough.Int8_input", 4)
oms_setInteger("model.root.Feedthrough.Int16_input", 5)
oms_setInteger("model.root.Feedthrough.Int32_input", 6)
oms_setInteger("model.root.Feedthrough.Int64_input", 7)
oms_setInteger("model.root.Feedthrough.UInt8_input", 8)
oms_setInteger("model.root.Feedthrough.UInt16_input", 9)
oms_setInteger("model.root.Feedthrough.UInt32_input", 10)
oms_setInteger("model.root.Feedthrough.UInt64_input", 11)
oms_setBoolean("model.root.Feedthrough.Boolean_input", true)
oms_setInteger("model.root.Feedthrough.Enumeration_input", 2)

-- need to implement fmi3causality table in fmi4c library if inital attribute is not present
--oms_setString("model.root.Feedthrough.String_parameter", "test me")

oms_initialize("model")
oms_simulate("model")

print("info:    Feedthrough.Float32_continuous_input  : " .. oms_getReal("model.root.Feedthrough.Float32_continuous_input"))
print("info:    Feedthrough.Float64_continuous_input  : " .. oms_getReal("model.root.Feedthrough.Float64_continuous_input"))
print("info:    Feedthrough.Int8_input                : " .. oms_getInteger("model.root.Feedthrough.Int8_input"))
print("info:    Feedthrough.Int16_input               : " .. oms_getInteger("model.root.Feedthrough.Int16_input"))
print("info:    Feedthrough.Int32_input               : " .. oms_getInteger("model.root.Feedthrough.Int32_input"))
print("info:    Feedthrough.Int64_input               : " .. oms_getInteger("model.root.Feedthrough.Int64_input"))
print("info:    Feedthrough.UInt8_input               : " .. oms_getInteger("model.root.Feedthrough.UInt8_input"))
print("info:    Feedthrough.UInt16_input              : " .. oms_getInteger("model.root.Feedthrough.UInt16_input"))
print("info:    Feedthrough.UInt32_input              : " .. oms_getInteger("model.root.Feedthrough.UInt32_input"))
print("info:    Feedthrough.UInt64_input              : " .. oms_getInteger("model.root.Feedthrough.UInt64_input"))

print("info:    Feedthrough.Boolean_input             : " .. oms_getBoolean("model.root.Feedthrough.Boolean_input"))
print("info:    Feedthrough.String_parameter          : " .. oms_getString("model.root.Feedthrough.String_parameter"))

print("info:    Feedthrough.Enumeration_input         : " .. oms_getInteger("model.root.Feedthrough.Enumeration_input"))

oms_terminate("model")
oms_delete("model")

-- Result:
-- warning: Unknown FMI3 base type for var : Binary_input
-- warning: Unknown FMI3 base type for var : Binary_output
-- Loading FMI version 3...
-- info:    No result file will be created
-- info:    Feedthrough.Float32_continuous_input  : 3.0999999046326
-- info:    Feedthrough.Float64_continuous_input  : 3.3
-- info:    Feedthrough.Int8_input                : 4
-- info:    Feedthrough.Int16_input               : 5
-- info:    Feedthrough.Int32_input               : 6
-- info:    Feedthrough.Int64_input               : 7
-- info:    Feedthrough.UInt8_input               : 8
-- info:    Feedthrough.UInt16_input              : 9
-- info:    Feedthrough.UInt32_input              : 10
-- info:    Feedthrough.UInt64_input              : 11
-- info:    Feedthrough.Boolean_input             : 1
-- info:    Feedthrough.String_parameter          : Set me!
-- info:    Feedthrough.Enumeration_input         : 2
-- info:    2 warnings
-- info:    0 errors
-- endResult
