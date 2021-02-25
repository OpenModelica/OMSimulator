-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./deleteConnector_lua/")

oms_newModel("deleteConnector")

oms_addSystem("deleteConnector.Root", oms_system_wc)
oms_addConnector("deleteConnector.Root.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteConnector.Root.C2", oms_causality_output, oms_signal_type_real)
oms_addConnection("deleteConnector.Root.C1", "deleteConnector.Root.C2")

oms_addSystem("deleteConnector.Root.System1", oms_system_sc)
oms_addConnector("deleteConnector.Root.System1.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteConnector.Root.System1.C2", oms_causality_input, oms_signal_type_real)

oms_addSystem("deleteConnector.Root.System2", oms_system_sc)
oms_addConnector("deleteConnector.Root.System2.C3", oms_causality_output, oms_signal_type_real)
oms_addConnector("deleteConnector.Root.System2.C4", oms_causality_output, oms_signal_type_real)
oms_setReal("deleteConnector.Root.System2.C3", 20.0)
oms_setReal("deleteConnector.Root.System2.C4", 30.0)

oms_addSubModel("deleteConnector.Root.System1.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms_setReal("deleteConnector.Root.System1.Gain.k", 30.0)

oms_addConnection("deleteConnector.Root.System2.C3", "deleteConnector.Root.System1.C1")
oms_addConnection("deleteConnector.Root.System2.C4", "deleteConnector.Root.System1.C2")

src = oms_exportSnapshot("deleteConnector")
print(src)

oms_delete("deleteConnector.Root.C1")

-- delete connector "deleteConnector.Root.System2.C3" and associated connection
oms_delete("deleteConnector.Root.System2.C3")

-- delete components connector and its startValue
oms_delete("deleteConnector.Root.System1.Gain.k")

-- delete unknown connector signal for warning
oms_delete("deleteConnector.Root.System2.C6")

-- delete unknown system for warning
oms_delete("deleteConnector.Root.System3")

-- after deletion
src = oms_exportSnapshot("deleteConnector")
print(src)


-- Result:
-- error:   [exportSnapshot] Not implemented
--
-- warning: failed to delete object "deleteConnector.Root.System2.C6" because the identifier couldn't be resolved to any connector, component, system, or model
-- warning: failed to delete object "deleteConnector.Root.System3" because the identifier couldn't be resolved to any connector, component, system, or model
-- error:   [exportSnapshot] Not implemented
--
-- info:    2 warnings
-- info:    2 errors
-- endResult
