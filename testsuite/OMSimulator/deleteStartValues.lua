-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./deleteStartValues_lua/")

oms_newModel("deleteStartValues")

oms_addSystem("deleteStartValues.Root", oms_system_wc)
oms_addConnector("deleteStartValues.Root.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteStartValues.Root.C2", oms_causality_output, oms_signal_type_real)
oms_addConnector("deleteStartValues.Root.C3", oms_causality_parameter, oms_signal_type_real)
oms_setReal("deleteStartValues.Root.C3", 10.0)

oms_addSystem("deleteStartValues.Root.System1", oms_system_sc)
oms_addConnector("deleteStartValues.Root.System1.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteStartValues.Root.System1.C2", oms_causality_input, oms_signal_type_real)

oms_setReal("deleteStartValues.Root.System1.C1", -10.0)
oms_setReal("deleteStartValues.Root.System1.C2", -30.0)

oms_addSystem("deleteStartValues.Root.System2", oms_system_sc)
oms_addConnector("deleteStartValues.Root.System2.C3", oms_causality_output, oms_signal_type_real)
oms_addConnector("deleteStartValues.Root.System2.C4", oms_causality_output, oms_signal_type_real)
oms_setReal("deleteStartValues.Root.System2.C3", 20.0)
oms_setReal("deleteStartValues.Root.System2.C4", 30.0)

oms_addSubModel("deleteStartValues.Root.System1.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")

--oms_setReal("deleteStartValues.Root.System1.Gain.k:start", 30.0) // this is allowed before initialization
--oms_setReal("deleteStartValues.Root.System1.Gain.k", 20.0) // allowed only if model is initialized, we are allowed to provide different value after initialization

src = oms_exportSnapshot("deleteStartValues")
print(src)

oms_delete("deleteStartValues.Root.C3:start")
oms_delete("deleteStartValues.Root.System1.C1:start")
oms_delete("deleteStartValues.Root.System1.C2:start")
oms_delete("deleteStartValues.Root.System2.C3:start")
oms_delete("deleteStartValues.Root.System2.C4:start")

-- delete unknown system startValue for warning
oms_delete("deleteStartValues.Root.System3.C4:start")

-- delete components startValue which was not set
oms_delete("deleteStartValues.Root.System1.Gain.k:start")

src = oms_exportSnapshot("deleteStartValues")
print(src)

-- Result:
-- error:   [exportSnapshot] Not implemented
--
-- warning: failed to delete object "deleteStartValues.Root.System3.C4:start" because the identifier couldn't be resolved to any connector, component, system, or model
-- warning: failed to delete start value "deleteStartValues.Root.System1.Gain.k:start" because the identifier couldn't be resolved to any component signal
-- error:   [exportSnapshot] Not implemented
--
-- info:    2 warnings
-- info:    2 errors
-- endResult
