-- status: correct
-- teardown_command: rm -rf tlmexternal.log tlmexternal-lua/ tlmexternal.csv tlmexternal.run tlmexternal_res.mat
-- linux: yes
-- mingw32: no
-- mingw64: no
-- mac: no

oms_setLogFile("tlmexternal.log")
oms_setTempDirectory("./tlmexternal-lua")
oms_newModel("model");
oms_setResultFile("model","tlmexternal_res.mat")
oms_addSystem("model.tlmexternal", oms_system_tlm);

-- adder
oms_addExternalModel("model.tlmexternal.adder", "../resources/tlm.adder.fmu", "./StartTLMFmiWrapper")
oms_addTLMBus("model.tlmexternal.adder.x1",  oms_tlm_domain_input, 1, oms_tlm_no_interpolation)
oms_addTLMBus("model.tlmexternal.adder.x2",  oms_tlm_domain_input, 1, oms_tlm_no_interpolation)
oms_addTLMBus("model.tlmexternal.adder.y",   oms_tlm_domain_output, 1, oms_tlm_no_interpolation)

-- source1
oms_addExternalModel("model.tlmexternal.source1", "../resources/tlm.source.fmu", "./StartTLMFmiWrapper")
oms_addTLMBus("model.tlmexternal.source1.y", oms_tlm_domain_output, 1, oms_tlm_no_interpolation)

-- source2
oms_addExternalModel("model.tlmexternal.source2", "../resources/tlm.source.fmu", "./StartTLMFmiWrapper")
oms_addTLMBus("model.tlmexternal.source2.y", oms_tlm_domain_output, 1, oms_tlm_no_interpolation)

-- gain
oms_addExternalModel("model.tlmexternal.gain", "../resources/tlm.gain.fmu", "./StartTLMFmiWrapper")
oms_addTLMBus("model.tlmexternal.gain.y",    oms_tlm_domain_input, 1, oms_tlm_no_interpolation)
oms_addTLMBus("model.tlmexternal.gain.u",    oms_tlm_domain_output, 1, oms_tlm_no_interpolation)

-- connections
oms_addTLMConnection("model.tlmexternal.adder.x1", "model.tlmexternal.source2.y", 0,0,0,0)
oms_addTLMConnection("model.tlmexternal.adder.x2", "model.tlmexternal.source1.y", 0,0,0,0)
oms_addTLMConnection("model.tlmexternal.gain.y", "model.tlmexternal.adder.y",     0,0,0,0)

-- simulation parameters
oms_setTLMSocketData("model.tlmexternal","127.0.1.1",13311,13411)
oms_setStartTime("model", 0.5)
oms_setStopTime("model", 4)

-- execute
oms_instantiate("model");
oms_initialize("model");
oms_simulate("model");
oms_terminate("model");

 vars = {"adder.y","source1.y","source2.y"}
 for _,var in ipairs(vars) do
   if 1 == oms_compareSimulationResults("tlmexternal.csv", "../references/tlmexternal.csv", var, 1e-2, 1e-4) then
     print(var .. " is equal")
   else
     print(var .. " is not equal")
   end
 end

-- Result:
-- Starting FMIWrapper. Debug output will be written to "TLMlogfile.log".
-- Starting FMIWrapper. Debug output will be written to "TLMlogfile.log".
-- Starting FMIWrapper. Debug output will be written to "TLMlogfile.log".
-- Starting FMIWrapper. Debug output will be written to "TLMlogfile.log".
-- Starting TLM simulation.
-- Monitoring thread finished.
-- Manager thread finished.
-- adder.y is equal
-- source1.y is equal
-- source2.y is equal
-- info:    Logging information has been saved to "tlmexternal.log"
-- endResult
