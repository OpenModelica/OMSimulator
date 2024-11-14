-- status: correct
-- teardown_command: rm -rf embrace-lua/
-- linux: yes
-- mingw32: no
-- mingw64: no
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true --wallTime=true --ignoreInitialUnknowns=false")
oms_setTempDirectory("./embrace-lua/")
oms_setWorkingDirectory("./embrace-lua/")

oms_importFile("../../resources/embrace.ssp")
oms_addSubModel("embrace.root.BC", "../../resources/CONOPS.csv")

------------------- Connections to BC files -----------------
--BC -> Consumer
oms_addConnection("embrace.root.Consumer.Alt", "embrace.root.BC.Alt")
oms_addConnection("embrace.root.Consumer.Mach", "embrace.root.BC.Mach")
oms_addConnection("embrace.root.Consumer.heatLoad", "embrace.root.BC.heatLoad")

--BC -> ECS_HW
oms_addConnection("embrace.root.ECS_HW.Alt", "embrace.root.BC.Alt")
oms_addConnection("embrace.root.ECS_HW.Mach", "embrace.root.BC.Mach")

--BC -> ECS_SW
oms_addConnection("embrace.root.BC.Aircraft_state", "embrace.root.AdaptionUnit.Aircraft_State")

--BC -> Atmos
oms_addConnection("embrace.root.Atmos.Alt", "embrace.root.BC.Alt")
oms_addConnection("embrace.root.Atmos.Ma", "embrace.root.BC.Mach")
-------------------------------------------------------------

--set params
oms_setReal("embrace.root.ECS_HW.eCS.MaxCoolPower.k", 5)
oms_setInteger("embrace.root.ECS_HW.coolinPackAir.looptype", 2)
oms_setInteger("embrace.root.ECS_HW.phSrc.looptype", 2)
oms_setInteger("embrace.root.ECS_HW.pipeB.looptype", 2)
oms_setInteger("embrace.root.ECS_HW.pipeA.looptype", 2)

--simulation settings
oms_setResultFile("embrace", "sim_results.mat")
oms_setStopTime("embrace", 12)
oms_setFixedStepSize("embrace", 1e-3)
oms_setLoggingInterval("embrace", 1)
oms_exportDependencyGraphs("embrace.root", "init.dot", "event.dot", "simulation.dot")

oms_instantiate("embrace")
oms_initialize("embrace")
print("info:    Initialize:")
print("info:      embrace.root.ECS_HW.coolinPackAir.looptype: " .. oms_getInteger("embrace.root.ECS_HW.coolinPackAir.looptype"))
print("info:      embrace.root.ECS_HW.eCS.MaxCoolPower.k    : " .. oms_getReal("embrace.root.ECS_HW.eCS.MaxCoolPower.k"))
print("info:      embrace.root.ECS_HW.pipeC.L               : " .. oms_getReal("embrace.root.ECS_HW.pipeC.L"))
print("info:      embrace.root.ECS_HW.pipeB.L               : " .. oms_getReal("embrace.root.ECS_HW.pipeB.L"))
print("info:      embrace.root.ECS_HW.pipeA.L               : " .. oms_getReal("embrace.root.ECS_HW.pipeA.L"))

oms_simulate("embrace")
oms_terminate("embrace")
oms_delete("embrace")

-- Result:
-- warning: invalid "SystemStructureDescription" detected in file "../../resources/embrace.ssp" at line: 17 column: 26, missing required attribute 'name'
-- warning: "SystemStructureDescription" does not conform to the SSP standard schema
-- warning: invalid "SystemStructureParameterValues" detected in file "resources/RAPID_Systems_2021-03-29_Test_1.ssv" at line: 3 column: 10, element 'Units' must be qualified
-- warning: "SystemStructureParameterValues" does not conform to the SSP standard schema
-- info:    Result file: sim_results.mat (bufferSize=1)
-- info:    Initialize:
-- info:      embrace.root.ECS_HW.coolinPackAir.looptype:  2
-- info:      embrace.root.ECS_HW.eCS.MaxCoolPower.k    :  5.0
-- info:      embrace.root.ECS_HW.pipeC.L               :  1.0
-- info:      embrace.root.ECS_HW.pipeB.L               :  0.976535328081166
-- info:      embrace.root.ECS_HW.pipeA.L               :  0.976535328081166
-- info:    4 warnings
-- info:    0 errors
-- endResult
