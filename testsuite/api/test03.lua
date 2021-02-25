-- status: correct
-- teardown_command: rm -rf test03-lua/ test_res.mat
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./test03-lua/")

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

oms_newModel("test")
oms_addSystem("test.eoo", oms_system_wc)
oms_addSubModel("test.eoo.source", "../resources/Modelica.Blocks.Sources.Sine.fmu")

-- save snapshot
src = oms_exportSnapshot("test")
print(src)

-- change model
oms_addSystem("test.eoo.goo", oms_system_sc)
oms_delete("test.eoo.source")
oms_addSubModel("test.eoo.source", "../resources/Modelica.Blocks.Sources.Constant.fmu")

-- restore model from snapshot
newCref, status = oms_importSnapshot("test", src)
printStatus(status, 0)

src = oms_exportSnapshot("test")
print(src)

oms_instantiate("test")
oms_initialize("test")
oms_simulate("test")
oms_terminate("test")

oms_delete("test")

-- Result:
-- error:   [exportSnapshot] Not implemented
--
-- error:   [importSnapshot] loading snapshot failed (No document element found)
-- status:  [wrong] error
-- error:   [exportSnapshot] Not implemented
--
-- info:    model doesn't contain any continuous state
-- info:    Result file: test_res.mat (bufferSize=10)
-- info:    0 warnings
-- info:    3 errors
-- endResult
