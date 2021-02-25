-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: yes

oms_setCommandLineOption("--suppressPath=true --exportParametersInline=false")
oms_setCommandLineOption("--suppressPath=true")

oms_setTempDirectory("./deleteStartValuesInSSV_lua/")

oms_newModel("deleteStartValuesInSSV")

oms_addSystem("deleteStartValuesInSSV.Root", oms_system_wc)

oms_addConnector("deleteStartValuesInSSV.Root.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteStartValuesInSSV.Root.C2", oms_causality_output, oms_signal_type_real)
oms_setReal("deleteStartValuesInSSV.Root.C1", 10.0)
oms_setReal("deleteStartValuesInSSV.Root.C2", 15.0)

-- add subSystem
oms_addSystem("deleteStartValuesInSSV.Root.System1", oms_system_sc)
oms_addConnector("deleteStartValuesInSSV.Root.System1.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteStartValuesInSSV.Root.System1.C2", oms_causality_input, oms_signal_type_real)

oms_setReal("deleteStartValuesInSSV.Root.System1.C1", 20.0)
oms_setReal("deleteStartValuesInSSV.Root.System1.C2", 30.0)

-- add submodel
oms_addSubModel("deleteStartValuesInSSV.Root.System1.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms_setReal("deleteStartValuesInSSV.Root.System1.Gain.k", 40.0)

src = oms_exportSnapshot("deleteStartValuesInSSV")
print(src)

-- delete top level system start value
oms_delete("deleteStartValuesInSSV.Root.C1:start")

-- delete subsystem start value
oms_delete("deleteStartValuesInSSV.Root.System1.C1:start")

-- delete unknown top level start value for warning
oms_delete("deleteStartValuesInSSV.Root.C3:start")

-- delete unknown subsystem start value for warning
oms_delete("deleteStartValuesInSSV.Root.System1.C4:start")

-- delete unknown component start values for warning
oms_delete("deleteStartValuesInSSV.Root.System1.Gain.j:start")

oms_export("deleteStartValuesInSSV", "deleteStartValuesInSSV.ssp");

oms_delete("deleteStartValuesInSSV")



-- Result:
-- error:   [exportSnapshot] Not implemented
--
-- warning: failed to delete start value "deleteStartValuesInSSV.Root.C3:start" because the identifier couldn't be resolved to any system signal
-- warning: failed to delete start value "deleteStartValuesInSSV.Root.System1.C4:start" because the identifier couldn't be resolved to any system signal
-- warning: failed to delete start value "deleteStartValuesInSSV.Root.System1.Gain.j:start" because the identifier couldn't be resolved to any component signal
-- info:    3 warnings
-- info:    1 errors
-- endResult
