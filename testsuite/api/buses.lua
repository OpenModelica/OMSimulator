-- status: correct
-- teardown_command: rm -rf buses-lua/
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

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

status = oms_setTempDirectory("./buses-lua/")
printStatus(status, 0)

status = oms_newModel("model")
status = oms_addSystem("model.tlm", oms_system_tlm)
status = oms_addSystem("model.tlm.wc1", oms_system_wc)
status = oms_addSystem("model.tlm.wc2", oms_system_wc)
status = oms_addConnector("model.tlm.wc1.u1", oms_causality_input, oms_signal_type_real)
status = oms_addConnector("model.tlm.wc1.u2", oms_causality_input, oms_signal_type_real)
status = oms_addConnector("model.tlm.wc1.y", oms_causality_output, oms_signal_type_real)
status = oms_addConnector("model.tlm.wc2.y1", oms_causality_output, oms_signal_type_real)
status = oms_addConnector("model.tlm.wc2.y2", oms_causality_output, oms_signal_type_real)
status = oms_addConnector("model.tlm.wc2.y3", oms_causality_output, oms_signal_type_real)
status = oms_addBus("model.tlm.wc1.bus1")
printStatus(status, 0)

status = oms_addConnectorToBus("model.tlm.wc1.bus1","model.tlm.wc1.u1")
printStatus(status, 0)

status = oms_addConnectorToBus("model.tlm.wc1.bus1","model.tlm.wc1.u2")
printStatus(status, 0)

status = oms_addConnectorToBus("model.tlm.wc1.bus1","model.tlm.wc2.y1")
printStatus(status, 3)

status = oms_addConnectorToBus("model.tlm.wc1.bus1","model.tlm.wc1.y")
printStatus(status, 0)

status = oms_addBus("model.tlm.wc2.bus2")
printStatus(status, 0)

status = oms_addConnectorToBus("model.tlm.wc2.bus2","model.tlm.wc2.y1")
printStatus(status, 0)

status = oms_addConnectorToBus("model.tlm.wc2.bus2","model.tlm.wc2.y2")
printStatus(status, 0)

status = oms_addConnection("model.tlm.wc1.u1","model.tlm.wc2.y1")
printStatus(status, 0)

status = oms_addConnection("model.tlm.wc1.bus1","model.tlm.wc2.bus2")
printStatus(status, 0)

status = oms_addConnection("model.tlm.wc1.u2","model.tlm.wc2.y2")
printStatus(status, 0)

status = oms_addConnection("model.tlm.wc2.bus2","model.tlm.wc1.bus1")
printStatus(status, 3)

src, status = oms_exportSnapshot("model.tlm")
print(src)

status = oms_deleteConnectorFromBus("model.tlm.wc1.bus1","model.tlm.wc1.y")
printStatus(status, 0)

src, status = oms_exportSnapshot("model.tlm")
print(src)

status = oms_delete("model")
printStatus(status, 0)


-- Result:
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- error:   [addConnectorToBus] Bus "wc1.bus1" and connector "wc2.y1" do not belong to same system
-- status:  [correct] error
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- error:   [addConnection] Connection <"wc2.bus2", "wc1.bus1"> exists already in system "model.tlm"
-- status:  [correct] error
-- error:   [exportSnapshot] Not implemented
--
-- status:  [correct] ok
-- error:   [exportSnapshot] Not implemented
--
-- status:  [correct] ok
-- info:    0 warnings
-- info:    4 errors
-- endResult
