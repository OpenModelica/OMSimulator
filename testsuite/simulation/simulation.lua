-- status: correct
-- teardown_command: rm -rf simulation-lua/ simulation_init.dot simulation_event.dot simulation_sim.dot simulation_res.mat
-- linux: yes
-- mingw32: no
-- mingw64: no
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./simulation-lua/")

function printStatus(status, expected)
  cmp = ""
  if status == expected then
    cmp = "correct"
  else
    cmp = "wrong"
  end

  if 0 == status then
    status = "ok"
  elseif 1 == status then
    status = "warning"
  elseif 3 == status then
    status = "error"
  end
  print("status:  [" .. cmp .. "] " .. status)
end

status = oms_newModel("simulation")
printStatus(status, 0)

status = oms_addSystem("simulation.co_sim", oms_system_wc)
printStatus(status, 0)

status = oms_addSubModel("simulation.co_sim.A", "../resources/tlm.source.fmu")
printStatus(status, 0)

status = oms_addSubModel("simulation.co_sim.B", "../resources/tlm.source.fmu")
printStatus(status, 0)

oms_exportDependencyGraphs("simulation.co_sim", "simulation_init.dot", "simulation_event.dot", "simulation_sim.dot")

status = oms_instantiate("simulation")
printStatus(status, 0)

v, status = oms_getReal("simulation.co_sim.A.A")
printStatus(status, 0)
print("simulation.co_sim.A.A: " .. v)

status = oms_setReal("simulation.co_sim.A.A", v + 1.0)
printStatus(status, 0)

v, status = oms_getReal("simulation.co_sim.A.A")
printStatus(status, 0)
print("simulation.co_sim.A.A: " .. v)

status = oms_initialize("simulation")
printStatus(status, 0)

v, status = oms_getReal("simulation.co_sim.A.y")
printStatus(status, 0)
print("simulation.co_sim.A.y: " .. v)

status = oms_simulate("simulation")
printStatus(status, 0)

v, status = oms_getReal("simulation.co_sim.A.y")
printStatus(status, 0)
print("simulation.co_sim.A.y: " .. v)

status = oms_terminate("simulation")
printStatus(status, 0)

status = oms_delete("simulation")
printStatus(status, 0)

-- Result:
-- status:  [correct] ok
-- status:  [correct] ok
-- warning: [A: resources/0001_A.fmu] The FMU lists 0 initial unknowns and exposes 1 initial unknowns.
--            * 2: y is missing
-- info:    [A: resources/0001_A.fmu] The FMU contains bad initial unknowns. This might cause problems, e.g. wrong simulation results.
-- status:  [correct] ok
-- warning: [B: resources/0002_B.fmu] The FMU lists 0 initial unknowns and exposes 1 initial unknowns.
--            * 2: y is missing
-- info:    [B: resources/0002_B.fmu] The FMU contains bad initial unknowns. This might cause problems, e.g. wrong simulation results.
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- simulation.co_sim.A.A: 1.0
-- status:  [correct] ok
-- status:  [correct] ok
-- simulation.co_sim.A.A: 2.0
-- info:    Result file: simulation_res.mat (bufferSize=10)
-- status:  [correct] ok
-- status:  [correct] ok
-- simulation.co_sim.A.y: 0.0
-- status:  [correct] ok
-- status:  [correct] ok
-- simulation.co_sim.A.y: 1.6829419696158
-- status:  [correct] ok
-- status:  [correct] ok
-- info:    2 warnings
-- info:    0 errors
-- endResult
