-- status: correct
-- teardown_command: rm -rf import_hierarchical_ssv_sources_lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./import_hierarchical_ssv_sources_lua/")
oms_setWorkingDirectory("./import_hierarchical_ssv_sources_lua/")

oms_importFile("../../resources/import_hierarchical_ssv_sources.ssp");

oms_instantiate("import_hierarchical_ssv_sources")

print("info:    Instantiation")
print("info:      import_hierarchical_ssv_sources.root.System1.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System1.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System1.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System1.parameter_1"))
print("info:      import_hierarchical_ssv_sources.root.System2.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System2.Input_2     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.Input_2"))
print("info:      import_hierarchical_ssv_sources.root.System2.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.parameter_1"))
print("info:      import_hierarchical_ssv_sources.root.System3.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System3.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System3.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System3.parameter_1"))

oms_initialize("import_hierarchical_ssv_sources")
print("info:    Initialization")
print("info:      import_hierarchical_ssv_sources.root.System1.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System1.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System1.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System1.parameter_1"))
print("info:      import_hierarchical_ssv_sources.root.System2.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System2.Input_2     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.Input_2"))
print("info:      import_hierarchical_ssv_sources.root.System2.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.parameter_1"))
print("info:      import_hierarchical_ssv_sources.root.System3.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System3.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System3.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System3.parameter_1"))

oms_simulate("import_hierarchical_ssv_sources")
print("info:    Simulate")
print("info:      import_hierarchical_ssv_sources.root.System1.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System1.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System1.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System1.parameter_1"))
print("info:      import_hierarchical_ssv_sources.root.System2.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System2.Input_2     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.Input_2"))
print("info:      import_hierarchical_ssv_sources.root.System2.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.parameter_1"))
print("info:      import_hierarchical_ssv_sources.root.System3.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System3.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System3.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System3.parameter_1"))

oms_terminate("import_hierarchical_ssv_sources")
oms_delete("import_hierarchical_ssv_sources")


-- Result:
-- info:    model doesn't contain any continuous state
-- info:    model doesn't contain any continuous state
-- info:    model doesn't contain any continuous state
-- info:    Instantiation
-- info:      import_hierarchical_ssv_sources.root.System1.Input_1     : -10.0
-- info:      import_hierarchical_ssv_sources.root.System1.parameter_1 : -20.0
-- info:      import_hierarchical_ssv_sources.root.System2.Input_1     : -30.0
-- info:      import_hierarchical_ssv_sources.root.System2.Input_2     : -100.0
-- info:      import_hierarchical_ssv_sources.root.System2.parameter_1 : -40.0
-- info:      import_hierarchical_ssv_sources.root.System3.Input_1     : -70.0
-- info:      import_hierarchical_ssv_sources.root.System3.parameter_1 : -70.0
-- info:    Result file: import_hierarchical_ssv_sources_res.mat (bufferSize=10)
-- info:    Initialization
-- info:      import_hierarchical_ssv_sources.root.System1.Input_1     : -10.0
-- info:      import_hierarchical_ssv_sources.root.System1.parameter_1 : -20.0
-- info:      import_hierarchical_ssv_sources.root.System2.Input_1     : -30.0
-- info:      import_hierarchical_ssv_sources.root.System2.Input_2     : -100.0
-- info:      import_hierarchical_ssv_sources.root.System2.parameter_1 : -40.0
-- info:      import_hierarchical_ssv_sources.root.System3.Input_1     : -70.0
-- info:      import_hierarchical_ssv_sources.root.System3.parameter_1 : -70.0
-- info:    Simulate
-- info:      import_hierarchical_ssv_sources.root.System1.Input_1     : -10.0
-- info:      import_hierarchical_ssv_sources.root.System1.parameter_1 : -20.0
-- info:      import_hierarchical_ssv_sources.root.System2.Input_1     : -30.0
-- info:      import_hierarchical_ssv_sources.root.System2.Input_2     : -100.0
-- info:      import_hierarchical_ssv_sources.root.System2.parameter_1 : -40.0
-- info:      import_hierarchical_ssv_sources.root.System3.Input_1     : -70.0
-- info:      import_hierarchical_ssv_sources.root.System3.parameter_1 : -70.0
-- endResult
