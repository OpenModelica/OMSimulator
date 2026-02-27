-- status: correct
-- teardown_command: rm -rf Pendulum-lua/ pendulum_res.mat
-- linux: no
-- ucrt64: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./Pendulum-lua/")


oms_newModel("model")

oms_addSystem("model.root", oms_system_sc)

oms_addSubModel("model.root.A", "../resources/Modelica.Mechanics.MultiBody.Examples.Elementary.Pendulum.fmu")

initialStepSize = 0.01
minimumStepSize = 0.01
maximumStepSize = 0.01

oms_setVariableStepSize("model", initialStepSize, minimumStepSize, maximumStepSize);
oms_setResultFile("model", "pendulum_res.mat")

oms_instantiate("model")

oms_initialize("model")

oms_simulate("model")

oms_terminate("model")

oms_delete("model")
