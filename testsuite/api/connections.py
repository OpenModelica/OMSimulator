## status: correct
## teardown_command: rm -rf connections-py/
## linux: yes
## mingw: yes
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")

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

status = oms.setTempDirectory("./connections-py/")
printStatus(status, 0)

status = oms.newModel("model")
printStatus(status, 0)

status = oms.addSystem("model.wc", oms.system_wc)
printStatus(status, 0)

status = oms.addSystem("model.wc.sc1", oms.system_sc)
printStatus(status, 0)

status = oms.addSystem("model.wc.sc2", oms.system_sc)
printStatus(status, 0)

status = oms.addConnector("model.wc.sc1.u1", oms.input, oms.signal_type_real)
status = oms.addConnector("model.wc.sc1.u2", oms.input, oms.signal_type_real)
status = oms.addConnector("model.wc.sc1.y", oms.output, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("model.wc.sc2.y1", oms.output, oms.signal_type_real)
status = oms.addConnector("model.wc.sc2.y2", oms.output, oms.signal_type_integer)
status = oms.addConnector("model.wc.sc2.y3", oms.output, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnection("model.wc.sc1.u1", "model.wc.sc2.y1")
printStatus(status, 0)

## Connecting input to input (illegal)
status = oms.addConnection("model.wc.sc1.y", "model.wc.sc2.y3")
printStatus(status, 3)

## Connecting Real to Integer (illegal)
status = oms.addConnection("model.wc.sc1.u2", "model.wc.sc2.y2")
printStatus(status, 3)

## Connecting to already connected connector (illegal)
status = oms.addConnection("model.wc.sc1.u1", "model.wc.sc2.y3")
printStatus(status, 3)

src, status = oms.exportSnapshot("model.wc")
print(src, flush=True)

status = oms.deleteConnection("model.wc.sc1.u1", "model.wc.sc2.y1")
printStatus(status, 0)

src, status = oms.exportSnapshot("model.wc")
print(src, flush=True)

status = oms.delete("model")
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
## error:   [addConnection] Causality mismatch in connection: sc1.y -> sc2.y3
## status:  [correct] error
## error:   [addConnection] Type mismatch in connection: sc1.u2 -> sc2.y2
## status:  [correct] error
## error:   [addConnection] Connector sc1.u1 is already connected to sc2.y1
## status:  [correct] error
## error:   [exportSnapshot] Not implemented
##
## status:  [correct] ok
## error:   [exportSnapshot] Not implemented
##
## status:  [correct] ok
## info:    0 warnings
## info:    5 errors
## endResult
