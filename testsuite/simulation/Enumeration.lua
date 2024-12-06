-- status: correct
-- teardown_command: rm -rf Enumeration_lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./Enumeration_lua/")
oms_setWorkingDirectory("./Enumeration_lua/")

oms_newModel("Enumeration")
oms_addSystem("Enumeration.root", oms_system_sc)
oms_addSubModel("Enumeration.root.A", "../../resources/Int1.fmu")
oms_addSubModel("Enumeration.root.B", "../../resources/Enum1.fmu")

-- integer to enumeration
oms_addConnection("Enumeration.root.A.y", "Enumeration.root.B.u")

-- oms_exportDependencyGraphs("Enumeration.root", "Enumeration.root_init.dot", "Enumeration.root_sim.dot", "Enumeration.root_continuous.dot")
-- os.execute("gvpr -c \"N[$.degree==0]{delete(root, $)}\" Enumeration.root_init.dot | dot -Tpdf -o Enumeration.root_init.pdf")
-- os.execute("gvpr -c \"N[$.degree==0]{delete(root, $)}\" Enumeration.root_sim.dot | dot -Tpdf -o Enumeration.root_sim.pdf")
-- os.execute("gvpr -c \"N[$.degree==0]{delete(root, $)}\" Enumeration.root_continuous.dot | dot -Tpdf -o Enumeration.root_continuous.pdf")

oms_instantiate("Enumeration")
print("info:    Instantiation")
print("info:      Enumeration.root.A.y     : " .. oms_getInteger("Enumeration.root.A.y"))
print("info:      Enumeration.root.B.u     : " .. oms_getInteger("Enumeration.root.B.u"))

oms_initialize("Enumeration")
print("info:    Initialization")
print("info:      Enumeration.root.A.y     : " .. oms_getInteger("Enumeration.root.A.y"))
print("info:      Enumeration.root.B.u     : " .. oms_getInteger("Enumeration.root.B.u"))

oms_simulate("Enumeration")
print("info:    Simulation")
print("info:      Enumeration.root.A.y     : " .. oms_getInteger("Enumeration.root.A.y"))
print("info:      Enumeration.root.B.u     : " .. oms_getInteger("Enumeration.root.B.u"))

oms_terminate("Enumeration")
oms_delete("Enumeration")

-- Result:
-- info:    model doesn't contain any continuous state
-- info:    Instantiation
-- info:      Enumeration.root.A.y     : 2
-- info:      Enumeration.root.B.u     : 1
-- info:    maximum step size for 'Enumeration.root': 0.001000
-- info:    Result file: Enumeration_res.mat (bufferSize=10)
-- info:    Initialization
-- info:      Enumeration.root.A.y     : 2
-- info:      Enumeration.root.B.u     : 2
-- info:    Simulation
-- info:      Enumeration.root.A.y     : 2
-- info:      Enumeration.root.B.u     : 2
-- info:    Final Statistics for 'Enumeration.root':
--          NumSteps = 1001 NumRhsEvals  = 1002 NumLinSolvSetups = 51
--          NumNonlinSolvIters = 1001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
-- endResult
