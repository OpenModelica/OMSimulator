-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./rename_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)
oms_addConnector("model.root.input1", oms_causality_input, oms_signal_type_real)
oms_addConnector("model.root.output", oms_causality_output, oms_signal_type_real)

oms_setReal("model.root.input1", 10)

oms_addSystem("model.root.System1", oms_system_sc)
oms_addConnector("model.root.System1.input1", oms_causality_input, oms_signal_type_real)
oms_addConnector("model.root.System1.output", oms_causality_output, oms_signal_type_real)
-- oms_setReal("model.root.System1.input1", 20)

oms_addSubModel("model.root.add", "../resources/Modelica.Blocks.Math.Add.fmu")

oms_addConnection("model.root.input1", "model.root.System1.input1")
oms_addConnection("model.root.input1", "model.root.add.u1")
-- oms_addConnection("model.root.input1", "model.root.add.u2")
oms_addConnection("model.root.System1.output", "model.root.add.u2")
oms_addConnection("model.root.add.y", "model.root.output")


src = oms_exportSnapshot("model")
print(src)

status = oms_rename("model.root", "root_1")
oms_addConnection("model.root_1.add.y", "model.root.output")

-- error as model.root is changed to root_1
status = oms_rename("model.root", "root_2")

-- error as model.root is changed to root_1
status = oms_rename("model.root.System1", "System_1")
-- TODO allow new connections after rename wth new connectors
oms_addConnection("model.root_1.System_1.output", "model.root_1.add.u2")

-- correct
status = oms_rename("model.root_1.System1", "System_1")

-- error as model.root_1.System1 is changed to System_1
status = oms_rename("model.root_1.System1", "System_2")

-- correct
status = oms_rename("model.root_1.System_1", "System_2")

-- correct
status = oms_rename("model.root_1.add", "add_1")

-- error as model_root_1.add is changed to add_1
status = oms_rename("model.root_1.add", "add_2")

oms_instantiate("model")

oms_initialize("model")

src = oms_exportSnapshot("model")
print(src)


print("info:      Parameter settings After Rename")
print("info:      model.root_1.input1             : " .. oms_getReal("model.root_1.input1"))
print("info:      model.root_1.System_2.input1    : " .. oms_getReal("model.root_1.System_2.input1"))
print("info:      model.root_1.add_1.u1           : " .. oms_getReal("model.root_1.add_1.u1"))
print("info:      model.root_1.add_1.u2           : " .. oms_getReal("model.root_1.add_1.u2"))


-- Result:
-- error:   [exportSnapshot] Not implemented
--
-- error:   [addConnection] Connection <"add.y", "output"> exists already in system "model.root_1"
-- error:   [rename] Model "model" does not contain system "root"
-- error:   [rename] Model "model" does not contain system "root"
-- error:   [getTLMBusConnector] "System_1.output" is not a valid ident
-- error:   [addConnection] Connector "System_1.output" not found in system "model.root_1"
-- error:   [rename] failed for "model.root_1.System1" as the identifier could not be resolved to a system or subsystem or component
-- error:   [rename] failed for "model.root_1.add" as the identifier could not be resolved to a system or subsystem or component
-- info:    model doesn't contain any continuous state
-- info:    Result file: model_res.mat (bufferSize=10)
-- error:   [exportSnapshot] Not implemented
--
-- info:      Parameter settings After Rename
-- info:      model.root_1.input1             : 10.0
-- info:      model.root_1.System_2.input1    : 10.0
-- info:      model.root_1.add_1.u1           : 10.0
-- info:      model.root_1.add_1.u2           : 0.0
-- info:    0 warnings
-- info:    9 errors
-- endResult
