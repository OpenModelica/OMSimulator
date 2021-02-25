-- status: correct
-- teardown_command: rm -rf import_export_parameters_lua/
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true --exportParametersInline=false")
oms_setTempDirectory("./import_export_parameters_lua/")

oms_newModel("import_export_parameters")

oms_addSystem("import_export_parameters.co_sim", oms_system_wc)
oms_addConnector("import_export_parameters.co_sim.Input_cref", oms_causality_input, oms_signal_type_real)
oms_addConnector("import_export_parameters.co_sim.Output_cref", oms_causality_output, oms_signal_type_real)
oms_addConnector("import_export_parameters.co_sim.T_cref", oms_causality_parameter, oms_signal_type_real)
oms_addConnector("import_export_parameters.co_sim.k_cref", oms_causality_parameter, oms_signal_type_real)

oms_setReal("import_export_parameters.co_sim.Input_cref", -20.0)
oms_setReal("import_export_parameters.co_sim.T_cref", 20.0)
oms_setReal("import_export_parameters.co_sim.k_cref", 30.0)


oms_addSystem("import_export_parameters.co_sim.foo", oms_system_sc)
oms_addConnector("import_export_parameters.co_sim.foo.F_cref", oms_causality_parameter, oms_signal_type_real)
oms_setReal("import_export_parameters.co_sim.foo.F_cref", 40.0)

-- instantiate FMUs
oms_addSubModel("import_export_parameters.co_sim.addP", "../resources/Modelica.Blocks.Math.Add.fmu")
oms_addSubModel("import_export_parameters.co_sim.P", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms_addSubModel("import_export_parameters.co_sim.addI", "../resources/Modelica.Blocks.Math.Add3.fmu")

-- TODO check all allowed connections
-- System.Parameter to Element.Parameter
oms_addConnection("import_export_parameters.co_sim.T_cref", "import_export_parameters.co_sim.addI.k2")
oms_addConnection("import_export_parameters.co_sim.k_cref", "import_export_parameters.co_sim.addP.k1")

-- System.Input to Element.Input
oms_addConnection("import_export_parameters.co_sim.Input_cref", "import_export_parameters.co_sim.addP.u1")

-- Element.Output to System.Output
oms_addConnection("import_export_parameters.co_sim.addP.y", "import_export_parameters.co_sim.Output_cref")

-- invalid connection check Element parameter to  Element parameter
oms_addConnection("import_export_parameters.co_sim.foo.F_cref", "import_export_parameters.co_sim.addP.k1")

-- setParameters
oms_setReal("import_export_parameters.co_sim.addP.k1", 10.0)
oms_setReal("import_export_parameters.co_sim.addP.k2", -1.0)
oms_setReal("import_export_parameters.co_sim.addI.k2",  2.0)

-- simulation settings
oms_setStartTime("import_export_parameters", 0.0)
oms_setStopTime("import_export_parameters", 4.0)
oms_setFixedStepSize("import_export_parameters.co_sim", 1e-3)
oms_setResultFile("import_export_parameters", "import_export_parameters.mat", 100)

-- oms_exportDependencyGraphs("import_export_parameters.co_sim", "import_export_parameters_init.dot", "import_export_parameters_sim.dot")
-- os.execute("gvpr -c \"N[$.degree==0]{delete(root, $)}\" import_export_parameters_init.dot | dot -Tpdf -o import_export_parameters_init.pdf")
-- os.execute("gvpr -c \"N[$.degree==0]{delete(root, $)}\" import_export_parameters_sim.dot | dot -Tpdf -o import_export_parameters_sim.pdf")

src, status = oms_exportSnapshot("import_export_parameters")
print(src)

oms_export("import_export_parameters", "import_export_parameters.ssp");
oms_delete("import_export_parameters")

oms_importFile("import_export_parameters.ssp");

oms_instantiate("import_export_parameters")

print("info:      Parameter settings")
print("info:      import_export_parameters.co_sim.addP.k1     : " .. oms_getReal("import_export_parameters.co_sim.addP.k1"))
print("info:      import_export_parameters.co_sim.addP.k2     : " .. oms_getReal("import_export_parameters.co_sim.addP.k2"))
print("info:      import_export_parameters.co_sim.addP.u1     : " .. oms_getReal("import_export_parameters.co_sim.addP.u1"))
print("info:      import_export_parameters.co_sim.addP.y      : " .. oms_getReal("import_export_parameters.co_sim.addP.y"))
print("info:      import_export_parameters.co_sim.addI.k2     : " .. oms_getReal("import_export_parameters.co_sim.addI.k2"))
-- Top level cref's
print("info:      Top-System-Level-Vars")
print("info:      import_export_parameters.co_sim.Input_cref  : " .. oms_getReal("import_export_parameters.co_sim.Input_cref"))
print("info:      import_export_parameters.co_sim.T_cref      : " .. oms_getReal("import_export_parameters.co_sim.T_cref"))
print("info:      import_export_parameters.co_sim.k_cref      : " .. oms_getReal("import_export_parameters.co_sim.k_cref"))
print("info:      import_export_parameters.co_sim.Output_cref : " .. oms_getReal("import_export_parameters.co_sim.Output_cref"))
print("info:      import_export_parameters.co_sim.foo.F_cref  : " .. oms_getReal("import_export_parameters.co_sim.foo.F_cref"))

oms_initialize("import_export_parameters")
print("info:    Initialization")
print("info:      import_export_parameters.co_sim.addP.k1     : " .. oms_getReal("import_export_parameters.co_sim.addP.k1"))
print("info:      import_export_parameters.co_sim.addP.k2     : " .. oms_getReal("import_export_parameters.co_sim.addP.k2"))
print("info:      import_export_parameters.co_sim.addP.u1     : " .. oms_getReal("import_export_parameters.co_sim.addP.u1"))
print("info:      import_export_parameters.co_sim.addP.y      : " .. oms_getReal("import_export_parameters.co_sim.addP.y"))
print("info:      import_export_parameters.co_sim.addI.k2     : " .. oms_getReal("import_export_parameters.co_sim.addI.k2"))
-- Top level cref's
print("info:      Top-System-Level-Vars")
print("info:      import_export_parameters.co_sim.Input_cref  : " .. oms_getReal("import_export_parameters.co_sim.Input_cref"))
print("info:      import_export_parameters.co_sim.T_cref      : " .. oms_getReal("import_export_parameters.co_sim.T_cref"))
print("info:      import_export_parameters.co_sim.k_cref      : " .. oms_getReal("import_export_parameters.co_sim.k_cref"))
print("info:      import_export_parameters.co_sim.Output_cref : " .. oms_getReal("import_export_parameters.co_sim.Output_cref"))
print("info:      import_export_parameters.co_sim.foo.F_cref  : " .. oms_getReal("import_export_parameters.co_sim.foo.F_cref"))

oms_simulate("import_export_parameters")
print("info:    Simulation")
print("info:      import_export_parameters.co_sim.addP.k1     : " .. oms_getReal("import_export_parameters.co_sim.addP.k1"))
print("info:      import_export_parameters.co_sim.addP.k2     : " .. oms_getReal("import_export_parameters.co_sim.addP.k2"))
print("info:      import_export_parameters.co_sim.addP.u1     : " .. oms_getReal("import_export_parameters.co_sim.addP.u1"))
print("info:      import_export_parameters.co_sim.addP.y      : " .. oms_getReal("import_export_parameters.co_sim.addP.y"))
print("info:      import_export_parameters.co_sim.addI.k2     : " .. oms_getReal("import_export_parameters.co_sim.addI.k2"))

-- Top level cref's
print("info:      Top-System-Level-Vars")
print("info:      import_export_parameters.co_sim.Input_cref  : " .. oms_getReal("import_export_parameters.co_sim.Input_cref"))
print("info:      import_export_parameters.co_sim.T_cref      : " .. oms_getReal("import_export_parameters.co_sim.T_cref"))
print("info:      import_export_parameters.co_sim.k_cref      : " .. oms_getReal("import_export_parameters.co_sim.k_cref"))
print("info:      import_export_parameters.co_sim.Output_cref : " .. oms_getReal("import_export_parameters.co_sim.Output_cref"))
print("info:      import_export_parameters.co_sim.foo.F_cref  : " .. oms_getReal("import_export_parameters.co_sim.foo.F_cref"))

oms_terminate("import_export_parameters")
oms_delete("import_export_parameters")

-- Result:
-- error:   [addConnection] Connector addP.k1 is already connected to k_cref
-- error:   [exportSnapshot] Not implemented
--
-- info:    model doesn't contain any continuous state
-- info:      Parameter settings
-- info:      import_export_parameters.co_sim.addP.k1     : 10.0
-- info:      import_export_parameters.co_sim.addP.k2     : -1.0
-- info:      import_export_parameters.co_sim.addP.u1     : 0.0
-- info:      import_export_parameters.co_sim.addP.y      : 0.0
-- info:      import_export_parameters.co_sim.addI.k2     : 2.0
-- info:      Top-System-Level-Vars
-- info:      import_export_parameters.co_sim.Input_cref  : -20.0
-- info:      import_export_parameters.co_sim.T_cref      : 20.0
-- info:      import_export_parameters.co_sim.k_cref      : 30.0
-- info:      import_export_parameters.co_sim.Output_cref : 0.0
-- info:      import_export_parameters.co_sim.foo.F_cref  : 40.0
-- info:    Result file: import_export_parameters.mat (bufferSize=100)
-- info:    Initialization
-- info:      import_export_parameters.co_sim.addP.k1     : 30.0
-- info:      import_export_parameters.co_sim.addP.k2     : -1.0
-- info:      import_export_parameters.co_sim.addP.u1     : -20.0
-- info:      import_export_parameters.co_sim.addP.y      : -600.0
-- info:      import_export_parameters.co_sim.addI.k2     : 20.0
-- info:      Top-System-Level-Vars
-- info:      import_export_parameters.co_sim.Input_cref  : -20.0
-- info:      import_export_parameters.co_sim.T_cref      : 20.0
-- info:      import_export_parameters.co_sim.k_cref      : 30.0
-- info:      import_export_parameters.co_sim.Output_cref : -600.0
-- info:      import_export_parameters.co_sim.foo.F_cref  : 40.0
-- info:    Simulation
-- info:      import_export_parameters.co_sim.addP.k1     : 30.0
-- info:      import_export_parameters.co_sim.addP.k2     : -1.0
-- info:      import_export_parameters.co_sim.addP.u1     : -20.0
-- info:      import_export_parameters.co_sim.addP.y      : -600.0
-- info:      import_export_parameters.co_sim.addI.k2     : 20.0
-- info:      Top-System-Level-Vars
-- info:      import_export_parameters.co_sim.Input_cref  : -20.0
-- info:      import_export_parameters.co_sim.T_cref      : 20.0
-- info:      import_export_parameters.co_sim.k_cref      : 30.0
-- info:      import_export_parameters.co_sim.Output_cref : -600.0
-- info:      import_export_parameters.co_sim.foo.F_cref  : 40.0
-- info:    0 warnings
-- info:    2 errors
-- endResult
