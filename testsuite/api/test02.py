## status: correct
## teardown_command: rm -rf test02-py/
## linux: yes
## mingw: yes
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./test02-py/")

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
  print "status:  [%s] %s" % (cmp, status)

def printType(t):
  if oms.system_tlm == t:
    print "type: system_tlm"
  elif oms.system_wc == t:
    print "type: system_wc"
  elif oms.system_sc == t:
    print "type: system_sc"
  else:
    print "Unknown type"

status = oms.newModel("test")
printStatus(status, 0)

status, type = oms.getSystemType("test")
printType(type)
printStatus(status, 3)

status = oms.addSystem("test.eoo", oms.system_tlm)
printStatus(status, 0)

status = oms.addSystem("test.eoo.foo", oms.system_wc)
printStatus(status, 0)

status = oms.addSystem("test.eoo.foo.goo", oms.system_sc)
printStatus(status, 0)

status, type = oms.getSystemType("test")
printType(type)
printStatus(status, 3)

status, type = oms.getSystemType("test.eoo")
printType(type)
printStatus(status, 0)

status, type = oms.getSystemType("test.eoo.foo")
printType(type)
printStatus(status, 0)

status, type = oms.getSystemType("test.eoo.foo.goo")
printType(type)
printStatus(status, 0)

status = oms.delete("test")
printStatus(status, 0)

## Result:
## status:  [correct] ok
## error:   [oms_getSystemType] Model "test" does not contain system ""
## Unknown type
## status:  [correct] error
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## type: system_tlm
## status:  [wrong] ok
## type: system_tlm
## status:  [correct] ok
## type: system_wc
## status:  [correct] ok
## type: system_sc
## status:  [correct] ok
## status:  [correct] ok
## info:    0 warnings
## info:    1 errors
## endResult
