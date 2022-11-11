oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./fmi4c_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)

-- oms_addSubModel("model.root.A", "C:/OPENMODELICAGIT/OpenModelica/testsuite/openmodelica/fmi/ModelExchange/2.0/fmi_attributes_06.fmu")
-- oms_addSubModel("model.root.A", "C:/OPENMODELICAGIT/OpenModelica/testsuite/openmodelica/fmi/ModelExchange/2.0/fmi_attributes_04.fmu")
oms_addSubModel("model.root.A", "C:/OPENMODELICAGIT/fmi4c/sampletest/Modelica.Blocks.Math.Add.fmu")

oms_instantiate("model")

oms_initialize("model")

oms_simulate("model")

oms_terminate("model")

oms_delete("model")
