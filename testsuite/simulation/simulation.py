## status: correct
## teardown_command: rm -rf simulation-py/ test_init1.dot test_sim1.dot test_event1.dot test.mat
## linux: yes
## mingw32: no
## mingw64: yes
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

status = oms.addSubModel("test.co_sim.A", "../resources/Modelica.Blocks.Sources.Sine.fmu")
printStatus(status, 0)

status = oms.addSubModel("test.co_sim.B", "../resources/Modelica.Blocks.Sources.Sine.fmu")
printStatus(status, 0)

oms.exportDependencyGraphs("test.co_sim", "test_init1.dot", "test_event1.dot", "test_sim1.dot")

oms.setResultFile("test", "test.mat")

status = oms.instantiate("test")
printStatus(status, 0)

v, status = oms.getReal("test.co_sim.A.amplitude")
printStatus(status, 0)
print("test.co_sim.A.amplitude.A: %g" % v, flush=True)

status = oms.setReal("test.co_sim.A.amplitude", v + 1.0)
printStatus(status, 0)

v, status = oms.getReal("test.co_sim.A.amplitude")
printStatus(status, 0)
print("test.co_sim.A.amplitude.A: %g" % v, flush=True)

status = oms.initialize("test")
printStatus(status, 0)

v, status = oms.getReal("test.co_sim.A.y")
printStatus(status, 0)
print("test.co_sim.A.amplitude.y: %g" % v, flush=True)

status = oms.simulate("test")
printStatus(status, 0)

v, status = oms.getReal("test.co_sim.A.y")
printStatus(status, 0)
print("test.co_sim.A.amplitude.y: %g" % v, flush=True)

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
## test.co_sim.A.amplitude.A: 1
## status:  [correct] ok
## status:  [correct] ok
## test.co_sim.A.amplitude.A: 2
## info:    Result file: test.mat (bufferSize=1)
## status:  [correct] ok
## status:  [correct] ok
## test.co_sim.A.amplitude.y: 0
## status:  [correct] ok
## status:  [correct] ok
## test.co_sim.A.amplitude.y: -4.89843e-16
## status:  [correct] ok
## status:  [correct] ok
## endResult
