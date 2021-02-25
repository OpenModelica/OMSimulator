## status: correct
## teardown_command: rm -rf test01-py/
## linux: yes
## mingw: yes
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./test01-py/")

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

status = oms.addSystem("test", oms.system_tlm)
printStatus(status, 3)

status = oms.addSystem("test.foo", oms.system_wc)
printStatus(status, 0)

status = oms.addSystem("test.foo.goo", oms.system_sc)
printStatus(status, 0)

status = oms.addSystem("test.foo.hoo", oms.system_wc)
printStatus(status, 3)

src, status = oms.exportSnapshot("test")
print(src, flush=True)

src, status = oms.exportSnapshot("test.foo")
print(src, flush=True)

src, status = oms.exportSnapshot("test.foo.goo")
print(src, flush=True)

status = oms.newModel("test")
printStatus(status, 3)

status = oms.rename("test", "foo")
printStatus(status, 0)

status = oms.newModel("test")
printStatus(status, 0)

status = oms.delete("test")
printStatus(status, 0)

status = oms.delete("foo")
printStatus(status, 0)

status = oms.newModel("test")
printStatus(status, 0)

src, status = oms.exportSnapshot("test")
print(src, flush=True)

status = oms.delete("test")
printStatus(status, 0)

status = oms.delete("foo")
printStatus(status, 3)

## Result:
## status:  [correct] ok
## error:   [addSystem] Model "test" does not contain any system
## status:  [correct] error
## status:  [correct] ok
## status:  [correct] ok
## error:   [NewSystem] A WC system must be the root system or a subsystem of a TLM system.
## status:  [correct] error
## error:   [exportSnapshot] Not implemented
##
## error:   [exportSnapshot] Not implemented
##
## error:   [exportSnapshot] Not implemented
##
## error:   [newModel] "test" already exists in the scope
## status:  [correct] error
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## error:   [exportSnapshot] Not implemented
##
## status:  [correct] ok
## error:   [deleteModel] Model "foo" does not exist in the scope
## status:  [correct] error
## info:    0 warnings
## info:    8 errors
## endResult
