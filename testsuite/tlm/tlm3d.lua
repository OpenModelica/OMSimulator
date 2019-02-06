-- status: correct
-- teardown_command: rm -rf tlm3d.log tlm3d-lua/ tlm3d.csv tlm3d.run tlm3d_all.csv
-- linux: yes
-- mingw: no
-- mac: no

oms_setLogFile("tlm3d.log")
oms_setTempDirectory("./tlm3d-lua")

oms_newModel("model");
oms_setResultFile("model","tlm3d_all.csv")
oms_setLoggingInterval("model",0.002)

oms_addSystem("model.tlm3d", oms_system_tlm);

oms_addSystem("model.tlm3d.wc1", oms_system_wc);
oms_setFixedStepSize("model.tlm3d.wc1", 0.00001);
oms_addSubModel("model.tlm3d.wc1.pendulum", "../resources/tlm.UpperPendulum.fmu");
oms_addConnector("model.tlm3d.wc1.x1",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.x2",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.x3",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.A11",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.A12",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.A13",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.A21",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.A22",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.A23",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.A31",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.A32",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.A33",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.v1",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.v2",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.v3",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.w1",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.w2",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.w3",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.f1",input,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.f2",input,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.f3",input,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.t1",input,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.t2",input,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc1.t3",input,oms_signal_type_real);
oms_addConnection("model.tlm3d.wc1.x1","model.tlm3d.wc1.pendulum.r[1]");
oms_addConnection("model.tlm3d.wc1.x2","model.tlm3d.wc1.pendulum.r[2]");
oms_addConnection("model.tlm3d.wc1.x3","model.tlm3d.wc1.pendulum.r[3]");
oms_addConnection("model.tlm3d.wc1.A11","model.tlm3d.wc1.pendulum.A[1,1]");
oms_addConnection("model.tlm3d.wc1.A12","model.tlm3d.wc1.pendulum.A[1,2]");
oms_addConnection("model.tlm3d.wc1.A13","model.tlm3d.wc1.pendulum.A[1,3]");
oms_addConnection("model.tlm3d.wc1.A21","model.tlm3d.wc1.pendulum.A[2,1]");
oms_addConnection("model.tlm3d.wc1.A22","model.tlm3d.wc1.pendulum.A[2,2]");
oms_addConnection("model.tlm3d.wc1.A23","model.tlm3d.wc1.pendulum.A[2,3]");
oms_addConnection("model.tlm3d.wc1.A31","model.tlm3d.wc1.pendulum.A[3,1]");
oms_addConnection("model.tlm3d.wc1.A32","model.tlm3d.wc1.pendulum.A[3,2]");
oms_addConnection("model.tlm3d.wc1.A33","model.tlm3d.wc1.pendulum.A[3,3]");
oms_addConnection("model.tlm3d.wc1.v1","model.tlm3d.wc1.pendulum.v[1]");
oms_addConnection("model.tlm3d.wc1.v2","model.tlm3d.wc1.pendulum.v[2]");
oms_addConnection("model.tlm3d.wc1.v3","model.tlm3d.wc1.pendulum.v[3]");
oms_addConnection("model.tlm3d.wc1.w1","model.tlm3d.wc1.pendulum.w[1]");
oms_addConnection("model.tlm3d.wc1.w2","model.tlm3d.wc1.pendulum.w[2]");
oms_addConnection("model.tlm3d.wc1.w3","model.tlm3d.wc1.pendulum.w[3]");
oms_addConnection("model.tlm3d.wc1.f1","model.tlm3d.wc1.pendulum.f[1]");
oms_addConnection("model.tlm3d.wc1.f2","model.tlm3d.wc1.pendulum.f[2]");
oms_addConnection("model.tlm3d.wc1.f3","model.tlm3d.wc1.pendulum.f[3]");
oms_addConnection("model.tlm3d.wc1.t1","model.tlm3d.wc1.pendulum.t[1]");
oms_addConnection("model.tlm3d.wc1.t2","model.tlm3d.wc1.pendulum.t[2]");
oms_addConnection("model.tlm3d.wc1.t3","model.tlm3d.wc1.pendulum.t[3]");
oms_addTLMBus("model.tlm3d.wc1.P",oms_tlm_domain_mechanical,3);
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.x1", "state1");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.x2", "state2");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.x3", "state3");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.A11", "A11");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.A12", "A12");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.A13", "A13");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.A21", "A21");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.A22", "A22");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.A23", "A23");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.A31", "A31");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.A32", "A32");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.A33", "A33");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.v1", "flow1");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.v2", "flow2");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.v3", "flow3");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.w1", "flow4");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.w2", "flow5");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.w3", "flow6");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.f1", "effort1");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.f2", "effort2");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.f3", "effort3");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.t1", "effort4");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.t2", "effort5");
oms_addConnectorToTLMBus("model.tlm3d.wc1.P","model.tlm3d.wc1.t3", "effort6");


oms_addSystem("model.tlm3d.wc2", oms_system_wc);
oms_setFixedStepSize("model.tlm3d.wc2", 0.00001);
oms_addSubModel("model.tlm3d.wc2.pendulum", "../resources/tlm.LowerPendulum.fmu");
oms_addConnector("model.tlm3d.wc2.x1",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.x2",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.x3",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.A11",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.A12",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.A13",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.A21",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.A22",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.A23",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.A31",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.A32",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.A33",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.v1",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.v2",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.v3",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.w1",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.w2",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.w3",output,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.f1",input,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.f2",input,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.f3",input,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.t1",input,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.t2",input,oms_signal_type_real);
oms_addConnector("model.tlm3d.wc2.t3",input,oms_signal_type_real);
oms_addConnection("model.tlm3d.wc2.x1","model.tlm3d.wc2.pendulum.r[1]");
oms_addConnection("model.tlm3d.wc2.x2","model.tlm3d.wc2.pendulum.r[2]");
oms_addConnection("model.tlm3d.wc2.x3","model.tlm3d.wc2.pendulum.r[3]");
oms_addConnection("model.tlm3d.wc2.A11","model.tlm3d.wc2.pendulum.A[1,1]");
oms_addConnection("model.tlm3d.wc2.A12","model.tlm3d.wc2.pendulum.A[1,2]");
oms_addConnection("model.tlm3d.wc2.A13","model.tlm3d.wc2.pendulum.A[1,3]");
oms_addConnection("model.tlm3d.wc2.A21","model.tlm3d.wc2.pendulum.A[2,1]");
oms_addConnection("model.tlm3d.wc2.A22","model.tlm3d.wc2.pendulum.A[2,2]");
oms_addConnection("model.tlm3d.wc2.A23","model.tlm3d.wc2.pendulum.A[2,3]");
oms_addConnection("model.tlm3d.wc2.A31","model.tlm3d.wc2.pendulum.A[3,1]");
oms_addConnection("model.tlm3d.wc2.A32","model.tlm3d.wc2.pendulum.A[3,2]");
oms_addConnection("model.tlm3d.wc2.A33","model.tlm3d.wc2.pendulum.A[3,3]");
oms_addConnection("model.tlm3d.wc2.v1","model.tlm3d.wc2.pendulum.v[1]");
oms_addConnection("model.tlm3d.wc2.v2","model.tlm3d.wc2.pendulum.v[2]");
oms_addConnection("model.tlm3d.wc2.v3","model.tlm3d.wc2.pendulum.v[3]");
oms_addConnection("model.tlm3d.wc2.w1","model.tlm3d.wc2.pendulum.w[1]");
oms_addConnection("model.tlm3d.wc2.w2","model.tlm3d.wc2.pendulum.w[2]");
oms_addConnection("model.tlm3d.wc2.w3","model.tlm3d.wc2.pendulum.w[3]");
oms_addConnection("model.tlm3d.wc2.f1","model.tlm3d.wc2.pendulum.f[1]");
oms_addConnection("model.tlm3d.wc2.f2","model.tlm3d.wc2.pendulum.f[2]");
oms_addConnection("model.tlm3d.wc2.f3","model.tlm3d.wc2.pendulum.f[3]");
oms_addConnection("model.tlm3d.wc2.t1","model.tlm3d.wc2.pendulum.t[1]");
oms_addConnection("model.tlm3d.wc2.t2","model.tlm3d.wc2.pendulum.t[2]");
oms_addConnection("model.tlm3d.wc2.t3","model.tlm3d.wc2.pendulum.t[3]");
oms_addTLMBus("model.tlm3d.wc2.P",oms_tlm_domain_mechanical,3);
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.x1", "state1");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.x2", "state2");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.x3", "state3");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.A11", "A11");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.A12", "A12");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.A13", "A13");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.A21", "A21");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.A22", "A22");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.A23", "A23");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.A31", "A31");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.A32", "A32");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.A33", "A33");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.v1", "flow1");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.v2", "flow2");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.v3", "flow3");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.w1", "flow4");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.w2", "flow5");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.w3", "flow6");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.f1", "effort1");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.f2", "effort2");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.f3", "effort3");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.t1", "effort4");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.t2", "effort5");
oms_addConnectorToTLMBus("model.tlm3d.wc2.P","model.tlm3d.wc2.t3", "effort6");

oms_addTLMConnection("model.tlm3d.wc1.P","model.tlm3d.wc2.P", 0.00001, 0.9, 10000, 100);

oms_setTLMSocketData("model.tlm3d","127.0.1.1",13511,13611)

oms_setStartTime("model",0);
oms_setStopTime("model",0.1);

oms_instantiate("model");
oms_initialize("model");

oms_setTLMPositionAndOrientation("model.tlm3d.wc1",0,0,0,0,0,-1,0,1,0,1,0,0)
oms_setTLMPositionAndOrientation("model.tlm3d.wc2",0.5,0,0,1,0,0,0,1,0,0,0,1)

oms_simulate("model");
oms_terminate("model");

vars = {"model.tlm3d.wc1.P.x[1]","model.tlm3d.wc1.P.x[2]","model.tlm3d.wc1.P.x[3]","model.tlm3d.wc2.P.x[1]","model.tlm3d.wc2.P.x[2]","model.tlm3d.wc2.P.x[3]"}
for _,var in ipairs(vars) do
  if 1 == oms_compareSimulationResults("tlm3d_all.csv", "../references/tlm3d_all.csv", var, 1e-2, 1e-4) then
    print(var .. " is equal")
  else
    print(var .. " is not equal")
  end
end


-- Result:
-- Starting TLM simulation.
-- Monitoring thread finished.
-- Manager thread finished.
-- model.tlm3d.wc1.P.x[1] is equal
-- model.tlm3d.wc1.P.x[2] is equal
-- model.tlm3d.wc1.P.x[3] is equal
-- model.tlm3d.wc2.P.x[1] is equal
-- model.tlm3d.wc2.P.x[2] is equal
-- model.tlm3d.wc2.P.x[3] is equal
-- info:    Logging information has been saved to "tlm3d.log"
-- endResult
