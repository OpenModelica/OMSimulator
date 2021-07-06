-- status: correct
-- teardown_command: rm -rf tlmsignals.log tlmsignals-lua/ tlmsignals.run tlmsignals.csv tlmsignals_res.mat
-- linux: yes
-- mingw32: no
-- mingw64: no
-- mac: no

oms_setLogFile("tlmsignals.log")
oms_setTempDirectory("./tlmsignals-lua/")

oms_newModel("model");
oms_setResultFile("model","tlmsignals_res.mat")

oms_addSystem("model.tlmsignals", oms_system_tlm);

oms_addSystem("model.tlmsignals.wc1", oms_system_wc);
oms_setFixedStepSize("model.tlmsignals.wc1", 0.005);
oms_addSubModel("model.tlmsignals.wc1.adder", "../resources/Modelica.Blocks.Math.Add.fmu");
oms_addSubModel("model.tlmsignals.wc1.sine1", "../resources/Modelica.Blocks.Sources.Sine.fmu");
oms_addSubModel("model.tlmsignals.wc1.sine2", "../resources/Modelica.Blocks.Sources.Sine.fmu");
oms_addSubModel("model.tlmsignals.wc1.gain", "../resources/Modelica.Blocks.Math.Gain.fmu");
oms_addConnection("model.tlmsignals.wc1.sine1.y", "model.tlmsignals.wc1.adder.u1");
oms_addConnection("model.tlmsignals.wc1.sine2.y", "model.tlmsignals.wc1.adder.u2");
oms_addTLMBus("model.tlmsignals.wc1.out",oms_tlm_domain_output,1);
oms_addTLMBus("model.tlmsignals.wc1.in",oms_tlm_domain_input,1);
oms_addConnector("model.tlmsignals.wc1.y",oms_causality_output,oms_signal_type_real);
oms_addConnector("model.tlmsignals.wc1.u",oms_causality_input,oms_signal_type_real);
oms_addConnection("model.tlmsignals.wc1.adder.y","model.tlmsignals.wc1.y");
oms_addConnection("model.tlmsignals.wc1.u","model.tlmsignals.wc1.gain.u");
oms_addConnectorToTLMBus("model.tlmsignals.wc1.out","model.tlmsignals.wc1.y", "value");
oms_addConnectorToTLMBus("model.tlmsignals.wc1.in","model.tlmsignals.wc1.u", "value");

oms_addSystem("model.tlmsignals.wc2", oms_system_wc);
oms_setFixedStepSize("model.tlmsignals.wc2", 0.005);
oms_addSubModel("model.tlmsignals.wc2.gain", "../resources/Modelica.Blocks.Math.Gain.fmu");
oms_addTLMBus("model.tlmsignals.wc2.out",oms_tlm_domain_output,1);
oms_addTLMBus("model.tlmsignals.wc2.in",oms_tlm_domain_input,1);
oms_addConnector("model.tlmsignals.wc2.y",oms_causality_output,oms_signal_type_real);
oms_addConnector("model.tlmsignals.wc2.u",oms_causality_input,oms_signal_type_real);
oms_addConnection("model.tlmsignals.wc2.gain.y","model.tlmsignals.wc2.y");
oms_addConnection("model.tlmsignals.wc2.u","model.tlmsignals.wc2.gain.u");
oms_addConnectorToTLMBus("model.tlmsignals.wc2.out","model.tlmsignals.wc2.y", "value");
oms_addConnectorToTLMBus("model.tlmsignals.wc2.in","model.tlmsignals.wc2.u", "value");

oms_addTLMConnection("model.tlmsignals.wc1.out","model.tlmsignals.wc2.in", 0.01, 0, 0, 0);
oms_addTLMConnection("model.tlmsignals.wc2.out","model.tlmsignals.wc1.in", 0.01, 0, 0, 0);

oms_setTLMSocketData("model.tlmsignals","127.0.1.1",11111,12111)

oms_setStartTime("model",0);
oms_setStopTime("model",1);

oms_instantiate("model");
oms_setReal("model.tlmsignals.wc1.sine1.amplitude",1);
oms_setReal("model.tlmsignals.wc1.sine1.freqHz",8);
oms_setReal("model.tlmsignals.wc1.sine2.amplitude",0.3);
oms_setReal("model.tlmsignals.wc1.sine2.freqHz",20);
oms_setReal("model.tlmsignals.wc2.gain.k",2);

oms_initialize("model");
oms_simulate("model");
oms_terminate("model");

vars = {"wc1.out","wc2.out"}
for _,var in ipairs(vars) do
  if 1 == oms_compareSimulationResults("tlmsignals.csv", "../references/tlmsignals.csv", var, 1e-2, 1e-4) then
    print(var .. " is equal")
  else
    print(var .. " is not equal")
  end
end

-- Result:
-- Starting TLM simulation.
-- Monitoring thread finished.
-- Manager thread finished.
-- wc1.out is equal
-- wc2.out is equal
-- info:    Logging information has been saved to "tlmsignals.log"
-- endResult
