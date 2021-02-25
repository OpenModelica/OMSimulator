## status: correct
## teardown_command: rm -rf test_omsExport-py/
## linux: yes
## linux32: yes
## mingw: yes
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./test_omsExport-py/")

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

status = oms.newModel("model")
printStatus(status, 0)

status = oms.addSystem("model.sc", oms.system_sc)
printStatus(status, 0)

status = oms.setSignalFilter("model", "[AB]")
printStatus(status, 0)

(src, status) = oms.exportSnapshot("model")
printStatus(status, 0)
print(src, flush=True)

status = oms.removeSignalsFromResults("model", "A")
printStatus(status, 0)

status = oms.addSignalsToResults("model", "Y")
printStatus(status, 0)

(src, status) = oms.exportSnapshot("model")
printStatus(status, 0)
print(src, flush=True)

status= oms.export("model", "model.ssp")
printStatus(status, 0)


## Result:
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## error:   [exportSnapshot] Not implemented
## status:  [wrong] error
##
## warning: [removeSignalsFromResults] is deprecated and [setSignalFilter] is the recommended API
## status:  [correct] ok
## warning: [addSignalsToResults] is deprecated and [setSignalFilter] is the recommended API
## status:  [correct] ok
## error:   [exportSnapshot] Not implemented
## status:  [wrong] error
##
## status:  [correct] ok
## info:    2 warnings
## info:    2 errors
## endResult
