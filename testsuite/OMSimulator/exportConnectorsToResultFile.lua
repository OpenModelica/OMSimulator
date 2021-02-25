-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./exportConnectors/")

oms_newModel("exportConnectors")

oms_addSystem("exportConnectors.Root", oms_system_wc)
oms_addConnector("exportConnectors.Root.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("exportConnectors.Root.C2", oms_causality_output, oms_signal_type_real)
oms_addConnector("exportConnectors.Root.C3", oms_causality_parameter, oms_signal_type_real)


-- oms_addSystem("exportConnectors.Root.System1", oms_system_sc)
-- oms_addConnector("exportConnectors.Root.System1.T1", oms_causality_input, oms_signal_type_real)

oms_addSubModel("exportConnectors.Root.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")

print("info:  Before Setting Start Values")
print("info:      exportConnectors.Root.C1                    : " .. oms_getReal("exportConnectors.Root.C1"))
print("info:      exportConnectors.Root.C3                    : " .. oms_getReal("exportConnectors.Root.C3"))
print("info:      exportConnectors.Root.Gain.u                : " .. oms_getReal("exportConnectors.Root.Gain.u"))
print("info:      exportConnectors.Root.Gain.k                : " .. oms_getReal("exportConnectors.Root.Gain.k"))


oms_setReal("exportConnectors.Root.C1", 2.5)
oms_setReal("exportConnectors.Root.C3", 4.5)
oms_setReal("exportConnectors.Root.Gain.u", 7.5)

print("info:  After Setting Start Values")
print("info:      exportConnectors.Root.C1                    : " .. oms_getReal("exportConnectors.Root.C1"))
print("info:      exportConnectors.Root.C3                    : " .. oms_getReal("exportConnectors.Root.C3"))
print("info:      exportConnectors.Root.Gain.u                : " .. oms_getReal("exportConnectors.Root.Gain.u"))

oms_instantiate("exportConnectors")
print("info:  After instantiation")
print("info:      exportConnectors.Root.C1                    : " .. oms_getReal("exportConnectors.Root.C1"))
print("info:      exportConnectors.Root.C3                    : " .. oms_getReal("exportConnectors.Root.C3"))
print("info:      exportConnectors.Root.Gain.u                : " .. oms_getReal("exportConnectors.Root.Gain.u"))

oms_initialize("exportConnectors")
print("info:  Initialization")
print("info:      exportConnectors.Root.C1                    : " .. oms_getReal("exportConnectors.Root.C1"))
print("info:      exportConnectors.Root.C3                    : " .. oms_getReal("exportConnectors.Root.C3"))
print("info:      exportConnectors.Root.Gain.u                : " .. oms_getReal("exportConnectors.Root.Gain.u"))

-- set external inputs
oms_setReal("exportConnectors.Root.C1", -12.5)
oms_setReal("exportConnectors.Root.Gain.u", 10.5)

oms_simulate("exportConnectors")
print("info:  Simulation")
print("info:      exportConnectors.Root.C1                    : " .. oms_getReal("exportConnectors.Root.C1"))
print("info:      exportConnectors.Root.C3                    : " .. oms_getReal("exportConnectors.Root.C3"))
print("info:      exportConnectors.Root.Gain.u                : " .. oms_getReal("exportConnectors.Root.Gain.u"))

src = oms_exportSnapshot("exportConnectors")
print(src)

oms_terminate("exportConnectors")
oms_delete("exportConnectors")

-- Result:
-- info:  Before Setting Start Values
-- info:      exportConnectors.Root.C1                    : 0.0
-- info:      exportConnectors.Root.C3                    : 0.0
-- info:      exportConnectors.Root.Gain.u                : 0.0
-- info:      exportConnectors.Root.Gain.k                : 1.0
-- info:  After Setting Start Values
-- info:      exportConnectors.Root.C1                    : 2.5
-- info:      exportConnectors.Root.C3                    : 4.5
-- info:      exportConnectors.Root.Gain.u                : 7.5
-- info:  After instantiation
-- info:      exportConnectors.Root.C1                    : 2.5
-- info:      exportConnectors.Root.C3                    : 4.5
-- info:      exportConnectors.Root.Gain.u                : 7.5
-- info:    Result file: exportConnectors_res.mat (bufferSize=10)
-- info:  Initialization
-- info:      exportConnectors.Root.C1                    : 2.5
-- info:      exportConnectors.Root.C3                    : 4.5
-- info:      exportConnectors.Root.Gain.u                : 7.5
-- info:  Simulation
-- info:      exportConnectors.Root.C1                    : -12.5
-- info:      exportConnectors.Root.C3                    : 4.5
-- info:      exportConnectors.Root.Gain.u                : 10.5
-- error:   [exportSnapshot] Not implemented
--
-- info:    0 warnings
-- info:    1 errors
-- endResult
