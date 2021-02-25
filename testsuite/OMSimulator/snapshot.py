## status: correct
## linux: yes
## mingw: yes
## win: no
## mac: no


from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true --exportParametersInline=false")
oms.setTempDirectory("./snapshot-py/")

oms.newModel("snapshot")
oms.addSystem("snapshot.root", oms.system_wc)

oms.addConnector("snapshot.root.C1", oms.input, oms.signal_type_real)
oms.setReal("snapshot.root.C1", -10)

oms.addSubModel("snapshot.root.add", "../resources/Modelica.Blocks.Math.Add.fmu")
oms.setReal("snapshot.root.add.u1", 10)
oms.setReal("snapshot.root.add.k1", 30)

snapshot, status = oms.exportSnapshot("snapshot")
##print(snapshot, flush=True)
oms.setReal("snapshot.root.add.u1", 3.5)

oms.importSnapshot("snapshot", snapshot)

snapshot, status = oms.exportSnapshot("snapshot")
print(snapshot, flush=True)

oms.delete("snapshot")

## Result:
## error:   [exportSnapshot] Not implemented
## error:   [importSnapshot] loading snapshot failed (No document element found)
## error:   [exportSnapshot] Not implemented
##
## info:    0 warnings
## info:    3 errors
## endResult
