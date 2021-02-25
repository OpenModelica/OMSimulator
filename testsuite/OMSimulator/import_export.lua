-- status: correct
-- teardown_command: rm -rf test.ssp import_export-lua/
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./import_export-lua/")

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

status = oms_addSystem("test.eoo", oms_system_tlm)
printStatus(status, 0)

status = oms_setTLMSocketData("test.eoo", "127.0.1.1", 11111, 11121)

status = oms_addSystem("test.eoo.foo", oms_system_wc)
printStatus(status, 0)

-- CS FMU
status = oms_addSubModel("test.eoo.foo.A", "../resources/tlm.source.fmu")
printStatus(status, 0)

-- table
status = oms_addSubModel("test.eoo.foo.T", "../resources/setpoint.csv")
printStatus(status, 0)

status = oms_addSystem("test.eoo.foo.goo", oms_system_sc)
printStatus(status, 0)

status = oms_addSystem("test.eoo.foo2", oms_system_wc)
printStatus(status, 0)

status = oms_addConnector("test.eoo.foo.f", oms_causality_input, oms_signal_type_real)
printStatus(status, 0)

status = oms_addConnector("test.eoo.foo.x", oms_causality_output, oms_signal_type_real)
printStatus(status, 0)

status = oms_addConnector("test.eoo.foo.v", oms_causality_output, oms_signal_type_real)
printStatus(status, 0)

status = oms_addConnector("test.eoo.foo.y1", oms_causality_output, oms_signal_type_real)
printStatus(status, 0)

status = oms_addConnector("test.eoo.foo.y2", oms_causality_output, oms_signal_type_real)
printStatus(status, 0)

status = oms_addTLMBus("test.eoo.foo.tlm", oms_tlm_domain_mechanical, 1, oms_tlm_no_interpolation)
printStatus(status, 0)

status = oms_addConnectorToTLMBus("test.eoo.foo.tlm", "test.eoo.foo.f", "effort")
printStatus(status, 0)

status = oms_addConnectorToTLMBus("test.eoo.foo.tlm", "test.eoo.foo.x", "state")
printStatus(status, 0)

status = oms_addConnectorToTLMBus("test.eoo.foo.tlm", "test.eoo.foo.v", "flow")
printStatus(status, 0)

status = oms_addBus("test.eoo.foo.bus")
printStatus(status, 0)

status = oms_addConnectorToBus("test.eoo.foo.bus", "test.eoo.foo.y1")
printStatus(status, 0)

status = oms_addConnectorToBus("test.eoo.foo.bus", "test.eoo.foo.y2")
printStatus(status, 0)

status = oms_addConnector("test.eoo.foo2.f", oms_causality_input, oms_signal_type_real)
printStatus(status, 0)

status = oms_addConnector("test.eoo.foo2.x", oms_causality_output, oms_signal_type_real)
printStatus(status, 0)

status = oms_addConnector("test.eoo.foo2.v", oms_causality_output, oms_signal_type_real)
printStatus(status, 0)

status = oms_addConnector("test.eoo.foo2.u1", oms_causality_input, oms_signal_type_real)
printStatus(status, 0)

status = oms_addConnector("test.eoo.foo2.u2", oms_causality_input, oms_signal_type_real)
printStatus(status, 0)

status = oms_addTLMBus("test.eoo.foo2.tlm", oms_tlm_domain_mechanical, 1, oms_tlm_no_interpolation)
printStatus(status, 0)

status = oms_addConnectorToTLMBus("test.eoo.foo2.tlm", "test.eoo.foo2.f", "effort")
printStatus(status, 0)

status = oms_addConnectorToTLMBus("test.eoo.foo2.tlm", "test.eoo.foo2.x", "state")
printStatus(status, 0)

status = oms_addConnectorToTLMBus("test.eoo.foo2.tlm", "test.eoo.foo2.v", "flow")
printStatus(status, 0)

status = oms_addBus("test.eoo.foo2.bus")
printStatus(status, 0)

status = oms_addConnectorToBus("test.eoo.foo2.bus", "test.eoo.foo2.u1")
printStatus(status, 0)

status = oms_addConnectorToBus("test.eoo.foo2.bus", "test.eoo.foo2.u2")
printStatus(status, 0)

status = oms_addTLMConnection("test.eoo.foo.tlm", "test.eoo.foo2.tlm", 0.001, 0.3, 100, 0)
printStatus(status, 0)

status = oms_addConnection("test.eoo.foo.y1", "test.eoo.foo2.u1")
printStatus(status, 0)

status = oms_addConnection("test.eoo.foo.y2", "test.eoo.foo2.u2")
printStatus(status, 0)

status = oms_addConnection("test.eoo.foo.bus", "test.eoo.foo2.bus")
printStatus(status, 0)

src, status = oms_exportSnapshot("test")
printStatus(status, 0)
print(src)

status = oms_export("test", "test.ssp");
printStatus(status, 0)

status = oms_delete("test")
printStatus(status, 0)

model, status = oms_importFile("test.ssp");
printStatus(status, 0)

src, status = oms_exportSnapshot(model)
printStatus(status, 0)
print(src)

status = oms_delete(model)
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
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- error:   [exportSnapshot] Not implemented
-- status:  [wrong] error
--
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- error:   [exportSnapshot] Not implemented
-- status:  [wrong] error
--
-- status:  [correct] ok
-- info:    0 warnings
-- info:    2 errors
-- endResult
