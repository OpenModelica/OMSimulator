-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true --exportParametersInline=false")
oms_setTempDirectory("./snapshot/")

oms_newModel("snapshot")
oms_addSystem("snapshot.root", oms_system_wc)

oms_addConnector("snapshot.root.C1", oms_causality_input, oms_signal_type_real)
oms_setReal("snapshot.root.C1", -10)

oms_addSubModel("snapshot.root.add", "../resources/Modelica.Blocks.Math.Add.fmu")
oms_setReal("snapshot.root.add.u1", 10)
oms_setReal("snapshot.root.add.k1", 30)

snapshot = oms_exportSnapshot("snapshot")
--print(snapshot)
oms_setReal("snapshot.root.add.u1", 3.5)

oms_importSnapshot("snapshot", snapshot)

snapshot = oms_exportSnapshot("snapshot")
print(snapshot)

oms_delete("snapshot")

-- Result:
-- error:   [exportSnapshot] Not implemented
-- error:   [importSnapshot] loading snapshot failed (No document element found)
-- error:   [exportSnapshot] Not implemented
--
-- info:    0 warnings
-- info:    3 errors
-- endResult
