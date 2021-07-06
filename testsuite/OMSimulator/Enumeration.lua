-- status: correct
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./Enumeration_lua/")

oms_newModel("test")
oms_addSystem("test.root", oms_system_sc)
oms_addSubModel("test.root.A", "../resources/Int1.fmu")
oms_addSubModel("test.root.B", "../resources/Enum1.fmu")

-- integer to enumeration
oms_addConnection("test.root.A.y", "test.root.B.u")

oms_setResultFile("test", "test_res.mat")

-- oms_exportDependencyGraphs("test.root", "test.root_init.dot", "test.root_sim.dot", "test.root_continuous.dot")
-- os.execute("gvpr -c \"N[$.degree==0]{delete(root, $)}\" test.root_init.dot | dot -Tpdf -o test.root_init.pdf")
-- os.execute("gvpr -c \"N[$.degree==0]{delete(root, $)}\" test.root_sim.dot | dot -Tpdf -o test.root_sim.pdf")
-- os.execute("gvpr -c \"N[$.degree==0]{delete(root, $)}\" test.root_continuous.dot | dot -Tpdf -o test.root_continuous.pdf")

oms_instantiate("test")
print("info:    Instantiation")
print("info:      test.root.A.y     : " .. oms_getInteger("test.root.A.y"))
print("info:      test.root.B.u     : " .. oms_getInteger("test.root.B.u"))

oms_initialize("test")
print("info:    Initialization")
print("info:      test.root.A.y     : " .. oms_getInteger("test.root.A.y"))
print("info:      test.root.B.u     : " .. oms_getInteger("test.root.B.u"))

oms_simulate("test")
print("info:    Simulation")
print("info:      test.root.A.y     : " .. oms_getInteger("test.root.A.y"))
print("info:      test.root.B.u     : " .. oms_getInteger("test.root.B.u"))

oms_terminate("test")
oms_delete("test")

-- Result:
-- info:    model doesn't contain any continuous state
-- info:    Instantiation
-- info:      test.root.A.y     : 2
-- info:      test.root.B.u     : 0
-- info:    Result file: test_res.mat (bufferSize=1)
-- info:    Initialization
-- info:      test.root.A.y     : 2
-- info:      test.root.B.u     : 2
-- info:    Simulation
-- info:      test.root.A.y     : 2
-- info:      test.root.B.u     : 2
-- endResult
