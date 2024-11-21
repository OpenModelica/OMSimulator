-- status: correct
-- teardown_command: rm -rf str_hello_world_lua/
-- linux: no
-- ucrt64: yes
-- win: yes
-- mac: no


oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./str_hello_world_lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)

oms_addSubModel('model.root.fmu', '../resources/str_hello_world.fmu')

oms_setResultFile("model", "")

oms_instantiate("model")

print("info: a: " .. oms_getString("model.root.fmu.a"))
print("info: b: " .. oms_getString("model.root.fmu.b"))
print("info: c: " .. oms_getString("model.root.fmu.c"))

oms_setString("model.root.fmu.b", "OMSimulator")
print("info: c: " .. oms_getString("model.root.fmu.c"))

oms_initialize("model")
print("info: a: " .. oms_getString("model.root.fmu.a"))
print("info: b: " .. oms_getString("model.root.fmu.b"))
print("info: c: " .. oms_getString("model.root.fmu.c"))

oms_terminate("model")
oms_delete("model")

-- Result:
-- info: a: Hello
-- info: b: World
-- info: c: Hello World!
-- info: c: Hello OMSimulator!
-- info:    No result file will be created
-- info: a: Hello
-- info: b: OMSimulator
-- info: c: Hello OMSimulator!
-- endResult
