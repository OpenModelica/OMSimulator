oms_setTempDirectory("./tempOMS/")
oms_newModel("testOM")
oms_addSystem("testOM.root", oms_system_sc)

-- Instantiate FMUs
oms_addSubModel("testOM.root.engine", "fmus/VehProp.SimulationModel.Vehicle.Engine.fmu")
oms_addSubModel("testOM.root.chassi", "fmus/VehProp.SimulationModel.Vehicle.Chassi.fmu")
oms_addSubModel("testOM.root.driver_2_1", "fmus/VehProp.SimulationModel.Driver_2.fmu")
--oms_addSubModel("testOM.root.operatingCycle", "fmus/VehProp.SimulationModel.OperatingCycle.fmu")
--oms_addSubModel("testOM.root.shift_logic_1_1", "fmus/VehProp.SimulationModel.Vehicle.Shift_logic_1.fmu")
--oms_addSubModel("testOM.root.transmission", "fmus/VehProp.SimulationModel.Vehicle.Transmission.fmu")

-- Add connections
--oms_addConnection("testOM.root.chassi.v_is", "testOM.root.driver_2_1.v_is")
--oms_addConnection("testOM.root.driver_2_1.Accelerator", "testOM.root.engine.T_req")
--oms_addConnection("testOM.root.driver_2_1.Brake", "testOM.root.chassi.Brake")
--oms_addConnection("testOM.root.operatingCycle.v_set[1]", "testOM.root.driver_2_1.v_set")
--oms_addConnection("testOM.root.chassi.dist", "testOM.root.operatingCycle.dist[1]")
--oms_addConnection("testOM.root.chassi.v_is", "testOM.root.shift_logic_1_1.v_is[1]")
--oms_addConnection("testOM.root.chassi.alpha", "testOM.root.operatingCycle.alpha[1]")
--oms_addConnection("testOM.root.engine.T_e", "testOM.root.transmission.T_e")
--oms_addConnection("testOM.root.shift_logic_1_1.y1[1]", "testOM.root.transmission.gear_ratio")
--oms_addConnection("testOM.root.chassi.v_is", "testOM.root.transmission.v_is")
--oms_addConnection("testOM.root.transmission.T_w", "testOM.root.chassi.T_w")
--oms_addConnection("testOM.root.transmission.w_e", "testOM.root.engine.w_e")

-- Simulation settings
oms_setResultFile("testOM", "results-oms.mat")
oms_setStartTime("testOM", 0)
oms_setStopTime("testOM", 1)
oms_setTolerance("testOM.root", 1e-6, 1e-6);
oms_setFixedStepSize("testOM.root", 1e-6)

-- oms_export("testOM", "testOM.ssp")
oms_export_fmu("testOM", "testOM.fmu")

-- Set start values
--oms_instantiate("testOM")
--oms_setReal("testOM.root.transmission.v_is", 24.4)

--oms_initialize("testOM")
--oms_simulate("testOM")
--oms_terminate("testOM")
--oms_delete("testOM")

