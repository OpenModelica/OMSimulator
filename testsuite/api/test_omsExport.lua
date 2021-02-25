-- status: correct
-- teardown_command: rm -rf test_omsExport-lua/
-- linux: yes
-- linux32: yes
-- mingw: yes
-- win: no
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./test_omsExport-lua/")

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

status = oms_newModel("model")
printStatus(status, 0)

status = oms_addSystem("model.sc", oms_system_sc)
printStatus(status, 0)

status = oms_setSignalFilter("model", "[AB]")
printStatus(status, 0)

src, status = oms_exportSnapshot("model")
printStatus(status, 0)
print(src)

status = oms_removeSignalsFromResults("model", "A")
printStatus(status, 0)

status = oms_addSignalsToResults("model", "Y")
printStatus(status, 0)

src, status = oms_exportSnapshot("model")
printStatus(status, 0)
print(src)

status= oms_export("model", "model.ssp")
printStatus(status, 0)


-- Result:
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- error:   [exportSnapshot] Not implemented
-- status:  [wrong] error
--
-- warning: [removeSignalsFromResults] is deprecated and [setSignalFilter] is the recommended API
-- status:  [correct] ok
-- warning: [addSignalsToResults] is deprecated and [setSignalFilter] is the recommended API
-- status:  [correct] ok
-- error:   [exportSnapshot] Not implemented
-- status:  [wrong] error
--
-- status:  [correct] ok
-- info:    2 warnings
-- info:    2 errors
-- endResult
