-- status: correct
-- teardown_command: rm -rf connections-lua/
-- linux: yes
-- mingw: yes
-- win: no
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")

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

status = oms_setTempDirectory("./connections-lua/")
printStatus(status, 0)

status = oms_newModel("model")
printStatus(status, 0)

status = oms_addSystem("model.wc", oms_system_wc)
printStatus(status, 0)

status = oms_addSystem("model.wc.sc1", oms_system_sc)
printStatus(status, 0)

status = oms_addSystem("model.wc.sc2", oms_system_sc)
printStatus(status, 0)

status = oms_addConnector("model.wc.sc1.u1", oms_causality_input, oms_signal_type_real)
status = oms_addConnector("model.wc.sc1.u2", oms_causality_input, oms_signal_type_real)
status = oms_addConnector("model.wc.sc1.y", oms_causality_output, oms_signal_type_real)
printStatus(status, 0)

status = oms_addConnector("model.wc.sc2.y1", oms_causality_output, oms_signal_type_real)
status = oms_addConnector("model.wc.sc2.y2", oms_causality_output, oms_signal_type_integer)
status = oms_addConnector("model.wc.sc2.y3", oms_causality_output, oms_signal_type_real)
printStatus(status, 0)

status = oms_addConnection("model.wc.sc1.u1", "model.wc.sc2.y1")
printStatus(status, 0)

--Connecting output to output (illegal)
status = oms_addConnection("model.wc.sc1.y", "model.wc.sc2.y3")
printStatus(status, 3)

--Connecting Real to Integer (illegal)
status = oms_addConnection("model.wc.sc1.u2", "model.wc.sc2.y2")
printStatus(status, 3)

--Connecting to already connected connector (illegal)
status = oms_addConnection("model.wc.sc1.u1", "model.wc.sc2.y3")
printStatus(status, 3)

src, status = oms_exportSnapshot("model.wc")
print(src)

status = oms_deleteConnection("model.wc.sc1.u1", "model.wc.sc2.y1")
printStatus(status, 0)

src, status = oms_exportSnapshot("model.wc")
print(src)

status = oms_delete("model")
printStatus(status, 0)

-- Result:
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- error:   [addConnection] Causality mismatch in connection: sc1.y -> sc2.y3
-- status:  [correct] error
-- error:   [addConnection] Type mismatch in connection: sc1.u2 -> sc2.y2
-- status:  [correct] error
-- error:   [addConnection] Connector sc1.u1 is already connected to sc2.y1
-- status:  [correct] error
-- error:   [exportSnapshot] "model.wc" is not a top level model
--
-- status:  [correct] ok
-- error:   [exportSnapshot] "model.wc" is not a top level model
--
-- status:  [correct] ok
-- info:    0 warnings
-- info:    5 errors
-- endResult
