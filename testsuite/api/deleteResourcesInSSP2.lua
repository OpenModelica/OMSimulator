-- status: correct
-- teardown_command: rm -rf deleteResources1_lua/
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./deleteResources2_lua/")
oms_setWorkingDirectory("./deleteResources2_lua/")

oms_newModel("deleteResources")

oms_addSystem("deleteResources.root", oms_system_wc)
oms_addConnector("deleteResources.root.Input1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteResources.root.Input2", oms_causality_input, oms_signal_type_real)

-- add Top level resources
oms_newResources("deleteResources.root:root.ssv")

oms_setReal("deleteResources.root.Input1", 10)
oms_setReal("deleteResources.root.Input2", 50)

oms_addSystem("deleteResources.root.system1", oms_system_sc)
oms_addConnector("deleteResources.root.system1.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteResources.root.system1.C2", oms_causality_input, oms_signal_type_real)

-- report error for reference file not provided when adding new resource
oms_newResources("deleteResources.root.system1")
-- report error for reference file with wrong extension
oms_newResources("deleteResources.root.system1:system")

-- add resources to subsystem
oms_newResources("deleteResources.root.system1:system1.ssv")
oms_setReal("deleteResources.root.system1.C1", -10)

oms_addSubModel("deleteResources.root.Gain", "../../resources/Modelica.Blocks.Math.Gain.fmu")

-- add resources to submodule
oms_newResources("deleteResources.root.Gain:gain.ssv")

oms_setReal("deleteResources.root.Gain.k", 27)

oms_setResultFile("deleteResources", "deleteResources4.mat", 10)

oms_export("deleteResources", "deleteResources4.ssp")

oms_terminate("deleteResources")
oms_delete("deleteResources")

oms_importFile("deleteResources4.ssp")

-- report error for reference file not provided
oms_deleteResources("deleteResources")
-- report error for wrong extension
oms_deleteResources("deleteResources:root.ss")
-- report error for reference file not found
oms_deleteResources("deleteResources:root1.ssv")

-- report error for reference file associated with subsystem not found
oms_deleteResources("deleteResources:system2.ssv")

-- report error for reference file associated with component not found
oms_deleteResources("deleteResources:gain1.ssv")

oms_terminate("deleteResources")
oms_delete("deleteResources")

-- Result:
-- error:   [newResources] resource file not provided for "deleteResources.root.system1", Provide a valid reference file eg: "model.root:test1.ssv"
-- error:   [newResources] filename extension for "deleteResources.root.system1:system" must be ".ssv", no other formats are supported
-- error:   [deleteResourcesInSSP] reference file not provided for "deleteResources", hence deleting resource file cannot be done. Provide a valid reference file eg: "model:test1.ssv"
-- error:   [deleteResourcesInSSP] filename extension for "deleteResources:root.ss" must be ".ssv" or ".ssm", no other formats are supported
-- error:   [deleteResourcesInSSP] failed to delete resources in ssp, as the reference file "deleteResources:root1.ssv" could not be resolved to a system or subsystem or component
-- error:   [deleteResourcesInSSP] failed to delete resources in ssp, as the reference file "deleteResources:system2.ssv" could not be resolved to a system or subsystem or component
-- error:   [deleteResourcesInSSP] failed to delete resources in ssp, as the reference file "deleteResources:gain1.ssv" could not be resolved to a system or subsystem or component
-- info:    0 warnings
-- info:    7 errors
-- endResult
