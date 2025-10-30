-- status: correct
-- teardown_command: rm -rf dualmassoscillator-lua/ DualMassOscillator.mat
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./dualmassoscillator-lua/")

oms_newModel("DualMassOscillator")
oms_addSystem("DualMassOscillator.root", oms_system_sc)

-- instantiate FMUs
oms_addSubModel("DualMassOscillator.root.system1", "../resources/DualMassOscillator.System1.fmu")
oms_addSubModel("DualMassOscillator.root.system2", "../resources/DualMassOscillator.System2.fmu")

-- add connections
oms_addConnection("DualMassOscillator.root.system1.F", "DualMassOscillator.root.system2.F")
oms_addConnection("DualMassOscillator.root.system1.x1", "DualMassOscillator.root.system2.x1")
oms_addConnection("DualMassOscillator.root.system1.v1", "DualMassOscillator.root.system2.v1")
oms_addConnection("DualMassOscillator.root.system1.a1", "DualMassOscillator.root.system2.a1")

-- simulation settings
oms_setResultFile("DualMassOscillator", "DualMassOscillator.mat", 100)
oms_setStopTime("DualMassOscillator", 10.0)
oms_setVariableStepSize("DualMassOscillator.root", 1e-12, 1e-12, 1e-3)

oms_instantiate("DualMassOscillator")
oms_setReal("DualMassOscillator.root.system1.x1_start", 0.0)
oms_setReal("DualMassOscillator.root.system2.x2_start", 0.5)

oms_initialize("DualMassOscillator")
print("info:    Initialization")
print("info:      system1.x1: " .. oms_getReal("DualMassOscillator.root.system1.x1"))
print("info:      system2.x2: " .. oms_getReal("DualMassOscillator.root.system2.x2"))

oms_simulate("DualMassOscillator")
print("info:    Simulation")
print("info:      system1.x1: " .. oms_getReal("DualMassOscillator.root.system1.x1"))
print("info:      system2.x2: " .. oms_getReal("DualMassOscillator.root.system2.x2"))

oms_terminate("DualMassOscillator")
oms_delete("DualMassOscillator")

-- Result:
-- info:    maximum step size for 'DualMassOscillator.root': 0.001000
-- info:    Result file: DualMassOscillator.mat (bufferSize=100)
-- info:    Initialization
-- info:      system1.x1: 0.0
-- info:      system2.x2: 0.5
-- info:    Simulation
-- info:      system1.x1: 0.051037644963887
-- info:      system2.x2: 0.031639500306724
-- info:    Final Statistics for 'DualMassOscillator.root':
--          NumSteps = 10007 NumRhsEvals  = 10008 NumLinSolvSetups = 508
--          NumNonlinSolvIters = 10007 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
-- endResult
