-- status: correct
-- teardown_command: rm -rf tlm1d.log tlm1d-lua/ tlm1d.csv tlm1d.run tlm1d_res.mat
-- linux: yes
-- mingw: no
-- mac: no

oms_setLogFile("tlm1d.log")
oms_setTempDirectory("./tlm1d-lua")

oms_newModel("model");
oms_setResultFile("model", "tlm1d_res.mat")

oms_addSystem("model.tlm1d", oms_system_tlm);

oms_addSystem("model.tlm1d.wc1", oms_system_wc);
oms_setFixedStepSize("model.tlm1d.wc1", 0.001);
oms_addSubModel("model.tlm1d.wc1.mass", "../resources/tlm.mass.fmu");
oms_addConnector("model.tlm1d.wc1.x",output,oms_signal_type_real);
oms_addConnector("model.tlm1d.wc1.v",output,oms_signal_type_real);
oms_addConnector("model.tlm1d.wc1.f",input,oms_signal_type_real);
oms_addConnection("model.tlm1d.wc1.x","model.tlm1d.wc1.mass.x");
oms_addConnection("model.tlm1d.wc1.v","model.tlm1d.wc1.mass.v");
oms_addConnection("model.tlm1d.wc1.f","model.tlm1d.wc1.mass.f");
oms_addTLMBus("model.tlm1d.wc1.P",oms_tlm_domain_mechanical,1);
oms_addConnectorToTLMBus("model.tlm1d.wc1.P","model.tlm1d.wc1.x", "state");
oms_addConnectorToTLMBus("model.tlm1d.wc1.P","model.tlm1d.wc1.v", "flow");
oms_addConnectorToTLMBus("model.tlm1d.wc1.P","model.tlm1d.wc1.f", "effort");

oms_addSystem("model.tlm1d.wc2", oms_system_wc);
oms_setFixedStepSize("model.tlm1d.wc2", 0.001);
oms_addSubModel("model.tlm1d.wc2.mass", "../resources/tlm.massSpring.fmu");
oms_addConnector("model.tlm1d.wc2.x",output,oms_signal_type_real);
oms_addConnector("model.tlm1d.wc2.v",output,oms_signal_type_real);
oms_addConnector("model.tlm1d.wc2.f",input,oms_signal_type_real);
oms_addConnection("model.tlm1d.wc2.x","model.tlm1d.wc2.mass.x");
oms_addConnection("model.tlm1d.wc2.v","model.tlm1d.wc2.mass.v");
oms_addConnection("model.tlm1d.wc2.f","model.tlm1d.wc2.mass.f");
oms_addTLMBus("model.tlm1d.wc2.P",oms_tlm_domain_mechanical,1);
oms_addConnectorToTLMBus("model.tlm1d.wc2.P","model.tlm1d.wc2.x", "state");
oms_addConnectorToTLMBus("model.tlm1d.wc2.P","model.tlm1d.wc2.v", "flow");
oms_addConnectorToTLMBus("model.tlm1d.wc2.P","model.tlm1d.wc2.f", "effort");

oms_addTLMConnection("model.tlm1d.wc1.P","model.tlm1d.wc2.P", 0.001, 0.5, 10, 0);

oms_setTLMSocketData("model.tlm1d","127.0.1.1",11211,12211)

oms_setStartTime("model",0);
oms_setStopTime("model",2);

oms_instantiate("model");
oms_initialize("model");
oms_simulate("model");
oms_terminate("model");

vars = {"wc1.P.v [m/s]","wc2.P.v [m/s]","wc1.P.F [N]","wc2.P.F [N]"}
for _,var in ipairs(vars) do
  if 1 == oms_compareSimulationResults("tlm1d.csv", "../references/tlm1d.csv", var, 1e-2, 1e-4) then
    print(var .. " is equal")
  else
    print(var .. " is not equal")
  end
end

-- Result:
-- Starting TLM simulation.
-- Monitoring thread finished.
-- Manager thread finished.
-- wc1.P.v [m/s] is equal
-- wc2.P.v [m/s] is equal
-- wc1.P.F [N] is equal
-- wc2.P.F [N] is equal
-- info:    Logging information has been saved to "tlm1d.log"
-- endResult
