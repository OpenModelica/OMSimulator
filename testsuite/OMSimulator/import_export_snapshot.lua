-- status: correct
-- teardown_command: rm -rf import_export_snapshot_lua/
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no


oms_setCommandLineOption("--suppressPath=true --exportParametersInline=false")
status = oms_setTempDirectory("./import_export_snapshot_lua/")

oms_newModel("import_export_snapshot")
oms_addSystem("import_export_snapshot.root", oms_system_wc)
oms_addConnector("import_export_snapshot.root.C1", oms_causality_input, oms_signal_type_real)
oms_setReal("import_export_snapshot.root.C1", -10)

oms_addSubModel("import_export_snapshot.root.add", "../resources/Modelica.Blocks.Math.Add.fmu")

oms_setReal("import_export_snapshot.root.add.u1", 10)
oms_setReal("import_export_snapshot.root.add.k1", 30)

-- src1 = oms_exportSnapshot("import_export_snapshot")
-- print(src1)

oms_export("import_export_snapshot", "import_export_snapshot.ssp");
oms_delete("import_export_snapshot")

oms_importFile("import_export_snapshot.ssp");

src1 = oms_exportSnapshot("import_export_snapshot")
print(src1)

src2 = oms_exportSnapshot("import_export_snapshot")
print(src2)

oms_importSnapshot("import_export_snapshot", src2)

-- check of error msg
oms_exportSnapshot("import_export_snapshot.root.add")

oms_setStopTime("import_export_snapshot", 2)

oms_instantiate("import_export_snapshot")

oms_initialize("import_export_snapshot")
oms_simulate("import_export_snapshot")
oms_terminate("import_export_snapshot")
oms_delete("import_export_snapshot")


-- Result:
-- error:   [exportSnapshot] Not implemented
--
-- error:   [exportSnapshot] Not implemented
--
-- error:   [importSnapshot] loading snapshot failed (No document element found)
-- error:   [exportSnapshot] Not implemented
-- info:    Result file: import_export_snapshot_res.mat (bufferSize=10)
-- info:    0 warnings
-- info:    4 errors
-- endResult
