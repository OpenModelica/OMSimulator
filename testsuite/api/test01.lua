-- status: correct
-- teardown_command: rm -rf test01-lua/
-- linux: yes
-- linux32: yes
-- mingw: yes
-- win: no
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./test01-lua/")

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

status = oms_newModel("test")
printStatus(status, 0)

status = oms_addSystem("test", oms_system_tlm)
printStatus(status, 3)

status = oms_addSystem("test.foo", oms_system_wc)
printStatus(status, 0)

status = oms_addSystem("test.foo.goo", oms_system_sc)
printStatus(status, 0)

status = oms_addSystem("test.foo.hoo", oms_system_wc)
printStatus(status, 3)

src, status = oms_exportSnapshot("test")
print(src)

src, status = oms_exportSnapshot("test.foo")
print(src)

src, status = oms_exportSnapshot("test.foo.goo")
print(src)

status = oms_newModel("test")
printStatus(status, 3)

status = oms_rename("test", "foo")
printStatus(status, 0)

status = oms_newModel("test")
printStatus(status, 0)

status = oms_delete("test")
printStatus(status, 0)

status = oms_delete("foo")
printStatus(status, 0)

status = oms_newModel("test")
printStatus(status, 0)

src, status = oms_exportSnapshot("test")
print(src)

status = oms_delete("test")
printStatus(status, 0)

status = oms_delete("foo")
printStatus(status, 3)

-- Result:
-- status:  [correct] ok
-- error:   [addSystem] Model "test" does not contain any system
-- status:  [correct] error
-- status:  [correct] ok
-- status:  [correct] ok
-- error:   [NewSystem] A WC system must be the root system or a subsystem of a TLM system.
-- status:  [correct] error
-- error:   [exportSnapshot] Not implemented
--
-- error:   [exportSnapshot] Not implemented
--
-- error:   [exportSnapshot] Not implemented
--
-- error:   [newModel] "test" already exists in the scope
-- status:  [correct] error
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- error:   [exportSnapshot] Not implemented
--
-- status:  [correct] ok
-- error:   [deleteModel] Model "foo" does not exist in the scope
-- status:  [correct] error
-- info:    0 warnings
-- info:    8 errors
-- endResult
