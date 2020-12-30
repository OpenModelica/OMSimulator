-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./import_parameter_mapping_inline_lua/")

oms_importFile("../resources/importParameterMapping.ssp");

oms_instantiate("import_parameter_mapping")

print("info:      Instantiation ")
print("info:      import_parameter_mapping.co_sim.Input_1              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_1"))
print("info:      import_parameter_mapping.co_sim.Input_2              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_2"))
print("info:      import_parameter_mapping.co_sim.Input_3              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_3"))
print("info:      import_parameter_mapping.co_sim.parameter_1          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_1"))
print("info:      import_parameter_mapping.co_sim.parameter_2          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System1.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_1"))
print("info:      import_parameter_mapping.co_sim.System1.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_2"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System2.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_1"))
print("info:      import_parameter_mapping.co_sim.System2.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_2"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_2"))


oms_initialize("import_parameter_mapping")
print("info:    Initialization")
print("info:      import_parameter_mapping.co_sim.Input_1              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_1"))
print("info:      import_parameter_mapping.co_sim.Input_2              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_2"))
print("info:      import_parameter_mapping.co_sim.Input_3              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_3"))
print("info:      import_parameter_mapping.co_sim.parameter_1          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_1"))
print("info:      import_parameter_mapping.co_sim.parameter_2          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System1.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_1"))
print("info:      import_parameter_mapping.co_sim.System1.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_2"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System2.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_1"))
print("info:      import_parameter_mapping.co_sim.System2.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_2"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_2"))


oms_simulate("import_parameter_mapping")
print("info:    Simulation")
print("info:      import_parameter_mapping.co_sim.Input_1              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_1"))
print("info:      import_parameter_mapping.co_sim.Input_2              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_2"))
print("info:      import_parameter_mapping.co_sim.Input_3              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_3"))
print("info:      import_parameter_mapping.co_sim.parameter_1          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_1"))
print("info:      import_parameter_mapping.co_sim.parameter_2          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System1.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_1"))
print("info:      import_parameter_mapping.co_sim.System1.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_2"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System2.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_1"))
print("info:      import_parameter_mapping.co_sim.System2.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_2"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_2"))

-- Result:
-- info:    model doesn't contain any continuous state
-- info:    model doesn't contain any continuous state
-- info:      Instantiation
-- info:      import_parameter_mapping.co_sim.Input_1              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_2              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_3              : 50.0
-- info:      import_parameter_mapping.co_sim.parameter_1          : -30.0
-- info:      import_parameter_mapping.co_sim.parameter_2          : -40.0
-- info:      import_parameter_mapping.co_sim.System1.Input_1      : 20.0
-- info:      import_parameter_mapping.co_sim.System1.Input_2      : 20.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_1  : -30.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_2  : 0.0
-- info:      import_parameter_mapping.co_sim.System2.Input_1      : 20.0
-- info:      import_parameter_mapping.co_sim.System2.Input_2      : 20.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_1  : -30.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_2  : 0.0
-- info:    Result file: import_parameter_mapping_res.mat (bufferSize=10)
-- info:    Initialization
-- info:      import_parameter_mapping.co_sim.Input_1              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_2              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_3              : 50.0
-- info:      import_parameter_mapping.co_sim.parameter_1          : -30.0
-- info:      import_parameter_mapping.co_sim.parameter_2          : -40.0
-- info:      import_parameter_mapping.co_sim.System1.Input_1      : 20.0
-- info:      import_parameter_mapping.co_sim.System1.Input_2      : 20.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_1  : -30.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_2  : 0.0
-- info:      import_parameter_mapping.co_sim.System2.Input_1      : 20.0
-- info:      import_parameter_mapping.co_sim.System2.Input_2      : 20.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_1  : -30.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_2  : 0.0
-- info:    Simulation
-- info:      import_parameter_mapping.co_sim.Input_1              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_2              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_3              : 50.0
-- info:      import_parameter_mapping.co_sim.parameter_1          : -30.0
-- info:      import_parameter_mapping.co_sim.parameter_2          : -40.0
-- info:      import_parameter_mapping.co_sim.System1.Input_1      : 20.0
-- info:      import_parameter_mapping.co_sim.System1.Input_2      : 20.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_1  : -30.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_2  : 0.0
-- info:      import_parameter_mapping.co_sim.System2.Input_1      : 20.0
-- info:      import_parameter_mapping.co_sim.System2.Input_2      : 20.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_1  : -30.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_2  : 0.0
-- endResult
