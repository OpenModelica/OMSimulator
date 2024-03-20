-- status: correct
-- teardown_command: rm -rf deleteResources2_lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./deleteReferences2_lua/")
oms_setWorkingDirectory("./deleteReferences2_lua/")

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

-- add resources to subsystem
oms_newResources("deleteResources.root.system1:system1.ssv")
oms_setReal("deleteResources.root.system1.C1", -10)

oms_addSubModel("deleteResources.root.Gain", "../../resources/Modelica.Blocks.Math.Gain.fmu")

-- add resources to submodule
oms_newResources("deleteResources.root.Gain:gain.ssv")

oms_setReal("deleteResources.root.Gain.k", 27)

oms_setResultFile("deleteResources", "deleteResources2.mat", 10)

oms_export("deleteResources", "deleteResources2.ssp")

oms_terminate("deleteResources")
oms_delete("deleteResources")

oms_importFile("deleteResources2.ssp")

-- report error for reference file not provided
oms_deleteResources("deleteResources.root")
-- report error for wrong extension
oms_deleteResources("deleteResources.root:root.ss")
-- report error for reference file not found
oms_deleteResources("deleteResources.root:root1.ssv")

-- report error for reference file associated with subsystem not found
oms_deleteResources("deleteResources.root.system2:system1.ssv")

-- report error for reference file associated with component not found
oms_deleteResources("deleteResources.root.Gain:gain1.ssv")

oms_terminate("deleteResources")
oms_delete("deleteResources")

-- Result:
-- error:   [deleteReferencesInSSD] reference file not provided for "deleteResources.root", hence deleting reference file cannot be done. Provide a valid reference file eg: "model.root:test1.ssv"
-- error:   [deleteReferencesInSSD] filename extension for "deleteResources.root:root.ss" must be ".ssv" or ".ssm", no other formats are supported
-- error:   [deleteReferencesInSSD] failed to delete references in ssd, as the reference file "deleteResources.root:root1.ssv" could not be resolved to a system or subsystem or component
-- error:   [deleteReferencesInSSD] failed to delete references in ssd, as the reference file "deleteResources.root.system2:system1.ssv" could not be resolved to a system or subsystem or component
-- error:   [deleteReferencesInSSD] failed to delete references in ssd, as the reference file "deleteResources.root.Gain:gain1.ssv" could not be resolved to a system or subsystem or component
-- info:    0 warnings
-- info:    5 errors
-- endResult
