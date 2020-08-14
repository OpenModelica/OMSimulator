## status: correct
## teardown_command: rm -rf simulation-py/ test_init.dot test_sim.dot test.mat
## linux: yes
## mingw: no
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./simulation-py/")

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

status = oms.newModel("test")
printStatus(status, 0)

status = oms.addSystem("test.co_sim", oms.system_wc)
printStatus(status, 0)

status = oms.addSubModel("test.co_sim.A", "../resources/tlm.source.fmu")
printStatus(status, 0)

status = oms.addSubModel("test.co_sim.B", "../resources/tlm.source.fmu")
printStatus(status, 0)

oms.exportDependencyGraphs("test.co_sim", "test_init.dot", "test_sim.dot")

oms.setResultFile("test", "test.mat")

status = oms.instantiate("test")
printStatus(status, 0)

v, status = oms.getReal("test.co_sim.A.A")
printStatus(status, 0)
print("test.co_sim.A.A: %g" % v, flush=True)

status = oms.setReal("test.co_sim.A.A", v + 1.0)
printStatus(status, 0)

v, status = oms.getReal("test.co_sim.A.A")
printStatus(status, 0)
print("test.co_sim.A.A: %g" % v, flush=True)

status = oms.initialize("test")
printStatus(status, 0)

v, status = oms.getReal("test.co_sim.A.y")
printStatus(status, 0)
print("test.co_sim.A.y: %g" % v, flush=True)

status = oms.simulate("test")
printStatus(status, 0)

v, status = oms.getReal("test.co_sim.A.y")
printStatus(status, 0)
print("test.co_sim.A.y: %g" % v, flush=True)

status = oms.terminate("test")
printStatus(status, 0)

status = oms.delete("test")
printStatus(status, 0)

## Result:
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## test.co_sim.A.A: 1
## status:  [correct] ok
## status:  [correct] ok
## test.co_sim.A.A: 2
## info:    Result file: test.mat (bufferSize=1)
## status:  [correct] ok
## status:  [correct] ok
## test.co_sim.A.y: 0
## status:  [correct] ok
## status:  [correct] ok
## test.co_sim.A.y: 1.68294
## status:  [correct] ok
## status:  [correct] ok
## endResult
