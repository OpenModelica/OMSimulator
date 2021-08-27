-- status: correct
-- teardown_command: rm -rf addExternalResources1_lua/
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./addExternalResources1_lua/")
oms_setWorkingDirectory("./addExternalResources1_lua/")

oms_newModel("addExternalResources")

oms_addSystem("addExternalResources.root", oms_system_wc)
oms_addConnector("addExternalResources.root.Input1", oms_causality_input, oms_signal_type_real)
oms_addConnector("addExternalResources.root.Input2", oms_causality_input, oms_signal_type_real)

-- add Top level resources
oms_newResources("addExternalResources.root:root.ssv")

oms_setReal("addExternalResources.root.Input1", 10)
oms_setReal("addExternalResources.root.Input2", 50)

oms_addSystem("addExternalResources.root.system1", oms_system_sc)
oms_addConnector("addExternalResources.root.system1.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("addExternalResources.root.system1.C2", oms_causality_input, oms_signal_type_real)

-- add resources to subsystem
oms_newResources("addExternalResources.root.system1:system1.ssv")
oms_setReal("addExternalResources.root.system1.C1", -10)

oms_addSubModel("addExternalResources.root.Gain", "../../resources/Modelica.Blocks.Math.Gain.fmu")

-- add resources to submodule
oms_newResources("addExternalResources.root.Gain:gain.ssv")

oms_setReal("addExternalResources.root.Gain.k", 27)

oms_setResultFile("addExternalResources", "addExternalResources1.mat", 10)

oms_export("addExternalResources", "addExternalResources1.ssp")

oms_terminate("addExternalResources")
oms_delete("addExternalResources")

oms_importFile("addExternalResources1.ssp")

-- add list of external resources from filesystem to ssp
oms_addResources("addExternalResources", "../../resources/externalRoot.ssv")
oms_addResources("addExternalResources:externalSystem", "../../resources/externalSystem1.ssv")
oms_addResources("addExternalResources", "../../resources/externalGain.ssv")

-- switch with new resouces
-- oms_referenceResources("addExternalResources.root:root.ssv")

oms_export("addExternalResources", "addExternalResources1.ssp")

oms_terminate("addExternalResources")
oms_delete("addExternalResources")



-- Result:
-- endResult
