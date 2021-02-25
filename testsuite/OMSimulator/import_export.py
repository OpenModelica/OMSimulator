## status: correct
## teardown_command: rm -rf import_export-py/ test-py.ssp
## linux: yes
## mingw: yes
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./import_export-py/")

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

status = oms.addSystem("test.eoo", oms.system_tlm)
printStatus(status, 0)

status = oms.setTLMSocketData("test.eoo", "127.0.1.1", 11111, 11121)

status = oms.addSystem("test.eoo.foo", oms.system_wc)
printStatus(status, 0)

## CS FMU
status = oms.addSubModel("test.eoo.foo.A", "../resources/tlm.source.fmu")
printStatus(status, 0)

## table
status = oms.addSubModel("test.eoo.foo.T", "../resources/setpoint.csv")
printStatus(status, 0)

status = oms.addSystem("test.eoo.foo.goo", oms.system_sc)
printStatus(status, 0)

status = oms.addSystem("test.eoo.foo2", oms.system_wc)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo.f", oms.input, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo.x", oms.output, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo.v", oms.output, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo.y1", oms.output, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo.y2", oms.output, oms.signal_type_real)
printStatus(status, 0)

status = oms.addTLMBus("test.eoo.foo.tlm", oms.tlm_domain_mechanical, 1, oms.default)
printStatus(status, 0)

status = oms.addConnectorToTLMBus("test.eoo.foo.tlm", "test.eoo.foo.f", "effort")
printStatus(status, 0)

status = oms.addConnectorToTLMBus("test.eoo.foo.tlm", "test.eoo.foo.x", "state")
printStatus(status, 0)

status = oms.addConnectorToTLMBus("test.eoo.foo.tlm", "test.eoo.foo.v", "flow")
printStatus(status, 0)

status = oms.addBus("test.eoo.foo.bus")
printStatus(status, 0)

status = oms.addConnectorToBus("test.eoo.foo.bus", "test.eoo.foo.y1")
printStatus(status, 0)

status = oms.addConnectorToBus("test.eoo.foo.bus", "test.eoo.foo.y2")
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo2.f", oms.input, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo2.x", oms.output, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo2.v", oms.output, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo2.u1", oms.input, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo2.u2", oms.input, oms.signal_type_real)
printStatus(status, 0)

status = oms.addTLMBus("test.eoo.foo2.tlm", oms.tlm_domain_mechanical, 1, oms.default)
printStatus(status, 0)

status = oms.addConnectorToTLMBus("test.eoo.foo2.tlm", "test.eoo.foo2.f", "effort")
printStatus(status, 0)

status = oms.addConnectorToTLMBus("test.eoo.foo2.tlm", "test.eoo.foo2.x", "state")
printStatus(status, 0)

status = oms.addConnectorToTLMBus("test.eoo.foo2.tlm", "test.eoo.foo2.v", "flow")
printStatus(status, 0)

status = oms.addBus("test.eoo.foo2.bus")
printStatus(status, 0)

status = oms.addConnectorToBus("test.eoo.foo2.bus", "test.eoo.foo2.u1")
printStatus(status, 0)

status = oms.addConnectorToBus("test.eoo.foo2.bus", "test.eoo.foo2.u2")
printStatus(status, 0)

status = oms.addTLMConnection("test.eoo.foo.tlm", "test.eoo.foo2.tlm", 0.001, 0.3, 100, 0)
printStatus(status, 0)

status = oms.addConnection("test.eoo.foo.y1", "test.eoo.foo2.u1")
printStatus(status, 0)

status = oms.addConnection("test.eoo.foo.y2", "test.eoo.foo2.u2")
printStatus(status, 0)

status = oms.addConnection("test.eoo.foo.bus", "test.eoo.foo2.bus")
printStatus(status, 0)

src, status = oms.exportSnapshot("test")
printStatus(status, 0)
print(src)

status = oms.export("test", "test-py.ssp")
printStatus(status, 0)

status = oms.delete("test")
printStatus(status, 0)

model, status = oms.importFile("test-py.ssp")
printStatus(status, 0)

src, status = oms.exportSnapshot(model)
printStatus(status, 0)
print(src)

status = oms.delete(model)
printStatus(status, 0)

## Result:
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## error:   [exportSnapshot] Not implemented
## status:  [wrong] error
##
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## error:   [exportSnapshot] Not implemented
## status:  [wrong] error
##
## status:  [correct] ok
## info:    0 warnings
## info:    2 errors
## endResult
