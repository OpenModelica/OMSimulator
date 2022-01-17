-- status: correct
-- teardown_command: rm -rf fmi_attribute_02_ME_lua/
-- linux: no
-- mingw32: no
-- mingw64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./fmi_attribute_02_ME_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_sc)

oms_addSubModel("model.root.fmi_attribute_02", "../resources/fmi_attributes_01_current_master.fmu")

oms_setResultFile("model", "fmi_attribute_02_ME.mat", 10)

oms_instantiate("model")

oms_initialize("model")

oms_simulate("model")

oms_terminate("model")
oms_delete("model")


-- Result:
-- endResult
