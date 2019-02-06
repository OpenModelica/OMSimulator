-- status: correct
-- teardown_command: rm -f HelloWorld_cs_Fit.log HelloWorld_cs_Fit_res.mat

-- Uncomment below if script shall be executed by a standard Lua interpreter
-- require("package")
-- OMSimulatorLua = package.loadlib("../../install/linux/lib/libOMSimulatorLua.so", "luaopen_OMSimulatorLua")
-- OMSimulatorLua()

oms_setLogFile("HelloWorld_cs_Fit.log")
status = oms_setTempDirectory("./HelloWorld_cs_Fit/")
status = oms_newModel("HelloWorld_cs_Fit")
status = oms_addSystem("HelloWorld_cs_Fit.root", oms_system_wc)
--status = oms_setFixedStepSize("HelloWorld_cs_Fit.root", 1e-1)
--oms_setTolerance("HelloWorld_cs_Fit.root", 1e-5);
--oms_setResultFile("HelloWorld_cs_Fit", "");

-- add FMU
status = oms_addSubModel("HelloWorld_cs_Fit.root.HelloWorld", "../resources/HelloWorld.fmu")

-- create system identification model for model
simodel = omsi_newSysIdentModel("HelloWorld_cs_Fit");
-- omsi_describe(simodel)

-- Data generated from simulating HelloWorld.mo for 1.0s with Euler and 0.1s step size
kNumSeries = 1;
kNumObservations = 11;
data_time = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1};
inputvars = {};
measurementvars = {"HelloWorld_cs_Fit.root.HelloWorld.x"};
data_x = {1, 0.9, 0.8100000000000001, 0.7290000000000001, 0.6561, 0.5904900000000001, 0.5314410000000001, 0.4782969000000001, 0.43046721, 0.387420489, 0.3486784401};

omsi_initialize(simodel, kNumSeries, data_time, inputvars, measurementvars)
-- omsi_describe(simodel)

omsi_addParameter(simodel, "HelloWorld_cs_Fit.root.HelloWorld.x_start", 0.5);
omsi_addParameter(simodel, "HelloWorld_cs_Fit.root.HelloWorld.a", -0.5);
omsi_addMeasurement(simodel, 0, "HelloWorld_cs_Fit.root.HelloWorld.x", data_x);
-- omsi_describe(simodel)

omsi_setOptions_max_num_iterations(simodel, 25)
omsi_solve(simodel, "")

status, simodelstate = omsi_getState(simodel);
-- print(status, simodelstate)

status, startvalue1, estimatedvalue1 = omsi_getParameter(simodel, "HelloWorld_cs_Fit.root.HelloWorld.a")
status, startvalue2, estimatedvalue2 = omsi_getParameter(simodel, "HelloWorld_cs_Fit.root.HelloWorld.x_start")
-- print("HelloWorld.a startvalue=" .. startvalue1 .. ", estimatedvalue=" .. estimatedvalue1)
-- print("HelloWorld.x_start startvalue=" .. startvalue2 .. ", estimatedvalue=" .. estimatedvalue2)
is_OK1 = estimatedvalue1 > -1.1 and estimatedvalue1 < -0.9
is_OK2 = estimatedvalue2 > 0.9 and estimatedvalue2 < 1.1
print("HelloWorld.a estimation is OK: " .. tostring(is_OK1))
print("HelloWorld.x_start estimation is OK: " .. tostring(is_OK2))

omsi_freeSysIdentModel(simodel)

oms_terminate("HelloWorld_cs_Fit")
oms_delete("HelloWorld_cs_Fit")

-- Result:
-- HelloWorld.a estimation is OK: true
-- HelloWorld.x_start estimation is OK: true
-- info:    Logging information has been saved to "HelloWorld_cs_Fit.log"
-- endResult
