-- status: correct
-- teardown_command: rm -rf test02-lua/
-- linux: yes
-- mingw: yes
-- win: yes
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./test02-lua/")

function printStatus(status, expected)
  cmp = ""
  if status == expected then
    cmp = "correct"
  else
    cmp = "wrong"
  end

  if 0 == status then
    status = "ok"
  elseif 1 == status then
    status = "warning"
  elseif 3 == status then
    status = "error"
  end
  print("status:  [" .. cmp .. "] " .. status)
end

function printType(t)
  if oms_system_tlm == t then
    print("type: oms_system_tlm")
  elseif oms_system_wc == t then
    print("type: oms_system_wc")
  elseif oms_system_sc == t then
    print("type: oms_system_sc")
  else
    print("Unknown type")
  end
end

status = oms_newModel("test")
printStatus(status, 0)

type, status = oms_getSystemType("test")
printType(type)
printStatus(status, 3)

status = oms_addSystem("test.eoo", oms_system_tlm)
printStatus(status, 0)

status = oms_addSystem("test.eoo.foo", oms_system_wc)
printStatus(status, 0)

status = oms_addSystem("test.eoo.foo.goo", oms_system_sc)
printStatus(status, 0)

type, status = oms_getSystemType("test")
printType(type)
printStatus(status, 3)

type, status = oms_getSystemType("test.eoo")
printType(type)
printStatus(status, 0)

type, status = oms_getSystemType("test.eoo.foo")
printType(type)
printStatus(status, 0)

type, status = oms_getSystemType("test.eoo.foo.goo")
printType(type)
printStatus(status, 0)

status = oms_delete("test")
printStatus(status, 0)

-- Result:
-- status:  [correct] ok
-- error:   [oms_getSystemType] Model "test" does not contain system ""
-- Unknown type
-- status:  [correct] error
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- type: oms_system_tlm
-- status:  [wrong] ok
-- type: oms_system_tlm
-- status:  [correct] ok
-- type: oms_system_wc
-- status:  [correct] ok
-- type: oms_system_sc
-- status:  [correct] ok
-- status:  [correct] ok
-- info:    0 warnings
-- info:    1 errors
-- endResult
