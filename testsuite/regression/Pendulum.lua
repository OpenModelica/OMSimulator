-- status: correct
-- teardown_command: rm -rf Pendulum-lua/ pendulum_res.mat
-- linux: no
-- ucrt64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./Pendulum-lua/")


oms_newModel("model")

oms_addSystem("model.root", oms_system_sc)

oms_addSubModel("model.root.A", "../resources/Modelica.Mechanics.MultiBody.Examples.Elementary.Pendulum.fmu")

oms_setVariableStepSize("model", 1e-6, 1e-12, 0.01);
oms_setResultFile("model", "pendulum_res.mat")

oms_instantiate("model")

oms_initialize("model")

oms_simulate("model")

oms_terminate("model")

oms_delete("model")


-- Result:
-- info:    maximum step size for 'model.root': 0.010000
-- info:    Result file: pendulum_res.mat (bufferSize=1)
-- info:    Parameter model.root.A.world.label1 will not be stored in the result file, because the signal type is not supported
-- info:    Parameter model.root.A.world.label2 will not be stored in the result file, because the signal type is not supported
-- info:    Final Statistics for 'model.root':
--          NumSteps = 118 NumRhsEvals  = 134 NumLinSolvSetups = 17
--          NumNonlinSolvIters = 133 NumNonlinSolvConvFails = 0 NumErrTestFails = 2
-- endResult
