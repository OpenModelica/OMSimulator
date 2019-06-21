-- status: correct
-- teardown_command: rm -rf HelloWorldWithInput_cs_Fit/ HelloWorldWithInput_cs_Fit.log HelloWorldWithInput_cs_Fit_res.csv
-- linux: yes
-- mingw: no
-- win: no
-- mac: no


oms_setLogFile("HelloWorldWithInput_cs_Fit.log")
status = oms_setTempDirectory("./HelloWorldWithInput_cs_Fit/")
status = oms_newModel("HelloWorldWithInput_cs_Fit")
status = oms_addSystem("HelloWorldWithInput_cs_Fit.root", oms_system_wc)

oms_setFixedStepSize("HelloWorldWithInput_cs_Fit.root", 1e-1)
oms_setTolerance("HelloWorldWithInput_cs_Fit.root", 1e-5);
oms_setResultFile("HelloWorldWithInput_cs_Fit", "");

-- add FMU
status = oms_addSubModel("HelloWorldWithInput_cs_Fit.root.HelloWorldWithInput", "../resources/HelloWorldWithInput.fmu")

-- create system identification model for model
simodel = omsi_newSysIdentModel("HelloWorldWithInput_cs_Fit");
-- omsi_describe(simodel)

-- Data generated from simulating HelloWorldWithInput.mo for 1.0s with Euler and 0.1s step size
kNumSeries = 1;
kNumObservations = 11;
data_time = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1};
inputvars = {"HelloWorldWithInput_cs_Fit.root.HelloWorldWithInput.u"};
data_u =    {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1}; -- input u=time
measurementvars = {"HelloWorldWithInput_cs_Fit.root.HelloWorldWithInput.x"};
data_x = {1, 0.91, 0.839, 0.7851, 0.74659, 0.721931, 0.7097379, 0.70876411, 0.717887699, 0.7360989291, 0.76248903619}

omsi_initialize(simodel, kNumSeries, data_time, inputvars, measurementvars)
-- omsi_describe(simodel)

omsi_addParameter(simodel, "HelloWorldWithInput_cs_Fit.root.HelloWorldWithInput.x_start", 0.5);
omsi_addParameter(simodel, "HelloWorldWithInput_cs_Fit.root.HelloWorldWithInput.a", -0.5);
omsi_addInput(simodel, "HelloWorldWithInput_cs_Fit.root.HelloWorldWithInput.u", data_u);
omsi_addMeasurement(simodel, 0, "HelloWorldWithInput_cs_Fit.root.HelloWorldWithInput.x", data_x);
-- omsi_describe(simodel)

omsi_setOptions_max_num_iterations(simodel, 25)
omsi_solve(simodel, "")

status, simodelstate = omsi_getState(simodel);
-- print(status, simodelstate)

status, startvalue1, estimatedvalue1 = omsi_getParameter(simodel, "HelloWorldWithInput_cs_Fit.root.HelloWorldWithInput.a")
status, startvalue2, estimatedvalue2 = omsi_getParameter(simodel, "HelloWorldWithInput_cs_Fit.root.HelloWorldWithInput.x_start")
-- print("HelloWorldWithInput.a startvalue=" .. startvalue1 .. ", estimatedvalue=" .. estimatedvalue1)
-- print("HelloWorldWithInput.x_start startvalue=" .. startvalue2 .. ", estimatedvalue=" .. estimatedvalue2)
is_OK1 = estimatedvalue1 > -1.1 and estimatedvalue1 < -0.9
is_OK2 = estimatedvalue2 > 0.9 and estimatedvalue2 < 1.1
print("HelloWorldWithInput.a estimation is OK: " .. tostring(is_OK1))
print("HelloWorldWithInput.x_start estimation is OK: " .. tostring(is_OK2))

omsi_freeSysIdentModel(simodel)

oms_terminate("HelloWorldWithInput_cs_Fit")
oms_delete("HelloWorldWithInput_cs_Fit")

-- Result:
-- HelloWorldWithInput.a estimation is OK: true
-- HelloWorldWithInput.x_start estimation is OK: true
-- info:    Logging information has been saved to "HelloWorldWithInput_cs_Fit.log"
-- endResult
