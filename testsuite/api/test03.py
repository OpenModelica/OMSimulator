## status: correct
## teardown_command: rm -rf test03-py/ test_res.mat
## linux: yes
## mingw: yes
## win: no
## mac: no


from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./test03-py/")

def printStatus(status, expected):
  cmp = ""
  if status == expected:
    cmp = "correct"
  else:
    cmp = "wrong"

  if 0 == status:
    status = "ok"
  elif 1 == status:
    status = "warning"
  elif 3 == status:
    status = "error"
  print("status:  [%s] %s" % (cmp, status), flush=True)



oms.newModel("test")
oms.addSystem("test.eoo", oms.system_wc)
oms.addSubModel("test.eoo.source", "../resources/Modelica.Blocks.Sources.Sine.fmu")

## save snapshot
src, status = oms.exportSnapshot("test")
print(src, flush=True)

## change model
oms.addSystem("test.eoo.goo", oms.system_sc)
oms.delete("test.eoo.source")
oms.addSubModel("test.eoo.source", "../resources/Modelica.Blocks.Sources.Constant.fmu")

## restore model from snapshot
newCref, status = oms.importSnapshot("test", src)
printStatus(status, 0)

src, status = oms.exportSnapshot("test")
print(src, flush=True)

oms.instantiate("test")
oms.initialize("test")
oms.simulate("test")
oms.terminate("test")

oms.delete("test")


## Result:
## error:   [exportSnapshot] Not implemented
##
## error:   [importSnapshot] loading snapshot failed (No document element found)
## status:  [wrong] error
## error:   [exportSnapshot] Not implemented
##
## info:    model doesn't contain any continuous state
## info:    Result file: test_res.mat (bufferSize=10)
## info:    0 warnings
## info:    3 errors
## endResult
