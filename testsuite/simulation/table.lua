-- status: correct
-- teardown_command: rm -rf table_lua/
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: no
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./table_lua/")
oms_setWorkingDirectory("./table_lua/")

systems = {oms_system_wc, oms_system_sc}
for _,system in ipairs(systems) do
  oms_newModel("table")
  oms_addSystem("table.root", system)

  -- instantiate table
  oms_addSubModel("table.root.A", "../../resources/table.csv")

  oms_instantiate("table")
  oms_initialize("table")
  oms_simulate("table")
  print("A.y: " .. oms_getReal("table.root.A.y"))
  oms_terminate("table")
  oms_delete("table")

  if 1 == oms_compareSimulationResults("../../resources/table.csv", "table_res.mat", "table.root.A.y", 1e-4, 1e-4) then
    print("system is equal")
  else
    print("system is not equal")
  end
end

-- Result:
-- info:    Result file: table_res.mat (bufferSize=10)
-- A.y: 0.0
-- system is equal
-- info:    model doesn't contain any continuous state
-- info:    Result file: table_res.mat (bufferSize=10)
-- A.y: 0.0
-- system is equal
-- endResult
