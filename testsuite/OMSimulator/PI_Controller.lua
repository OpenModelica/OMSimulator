-- status: correct
-- teardown_command: rm -rf PI_Controller-lua/ PI_Controller_init.dot PI_Controller_sim.dot PI_Controller_res.mat PI_Controller.ssp
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./PI_Controller-lua/")

oms_newModel("PI_Controller")
oms_addSystem("PI_Controller.co_sim", oms_system_wc)

-- instantiate FMUs
oms_addSubModel("PI_Controller.co_sim.addP", "../resources/Modelica.Blocks.Math.Add.fmu")
oms_addSubModel("PI_Controller.co_sim.P", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms_addSubModel("PI_Controller.co_sim.addI", "../resources/Modelica.Blocks.Math.Add3.fmu")
oms_addSubModel("PI_Controller.co_sim.I", "../resources/Modelica.Blocks.Continuous.Integrator.fmu")
oms_addSubModel("PI_Controller.co_sim.addPI", "../resources/Modelica.Blocks.Math.Add.fmu")
oms_addSubModel("PI_Controller.co_sim.gainPI", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms_addSubModel("PI_Controller.co_sim.limiter", "../resources/Modelica.Blocks.Nonlinear.Limiter.fmu")
oms_addSubModel("PI_Controller.co_sim.addSat", "../resources/Modelica.Blocks.Math.Add.fmu")
oms_addSubModel("PI_Controller.co_sim.gainTrack", "../resources/Modelica.Blocks.Math.Gain.fmu")

-- add connections
oms_addConnection("PI_Controller.co_sim.addP.y", "PI_Controller.co_sim.P.u")
oms_addConnection("PI_Controller.co_sim.addI.y", "PI_Controller.co_sim.I.u")
oms_addConnection("PI_Controller.co_sim.P.y", "PI_Controller.co_sim.addPI.u1")
oms_addConnection("PI_Controller.co_sim.I.y", "PI_Controller.co_sim.addPI.u2")
oms_addConnection("PI_Controller.co_sim.addPI.y", "PI_Controller.co_sim.gainPI.u")
oms_addConnection("PI_Controller.co_sim.gainPI.y", "PI_Controller.co_sim.limiter.u")
oms_addConnection("PI_Controller.co_sim.gainPI.y", "PI_Controller.co_sim.addSat.u2")
oms_addConnection("PI_Controller.co_sim.limiter.y", "PI_Controller.co_sim.addSat.u1")
oms_addConnection("PI_Controller.co_sim.addSat.y", "PI_Controller.co_sim.gainTrack.u")
oms_addConnection("PI_Controller.co_sim.gainTrack.y", "PI_Controller.co_sim.addI.u3")

-- instantiate lookup table
oms_addSubModel("PI_Controller.co_sim.setpoint", "../resources/setpoint.csv")
oms_addSubModel("PI_Controller.co_sim.driveTrain", "../resources/drivetrain.csv")

-- add connections
oms_addConnection("PI_Controller.co_sim.setpoint.speed", "PI_Controller.co_sim.addP.u1")
oms_addConnection("PI_Controller.co_sim.setpoint.speed", "PI_Controller.co_sim.addI.u1")
oms_addConnection("PI_Controller.co_sim.driveTrain.w", "PI_Controller.co_sim.addP.u2")
oms_addConnection("PI_Controller.co_sim.driveTrain.w", "PI_Controller.co_sim.addI.u2")

-- add connectors
oms_addConnector("PI_Controller.co_sim.speed", oms_causality_output, oms_signal_type_real)
oms_addConnection("PI_Controller.co_sim.setpoint.speed", "PI_Controller.co_sim.speed")

-- simulation settings
oms_setStartTime("PI_Controller", 0.0)
oms_setStopTime("PI_Controller", 4.0)
oms_setFixedStepSize("PI_Controller.co_sim", 1e-3)
oms_setResultFile("PI_Controller", "PI_Controller.mat", 100)

--oms_exportDependencyGraphs("PI_Controller.co_sim", "PI_Controller_init.dot", "PI_Controller_sim.dot")
--os.execute("gvpr -c \"N[$.degree==0]{delete(root, $)}\" PI_Controller_init.dot | dot -Tpdf -o PI_Controller_init.pdf")
--os.execute("gvpr -c \"N[$.degree==0]{delete(root, $)}\" PI_Controller_sim.dot | dot -Tpdf -o PI_Controller_sim.pdf")

oms_export("PI_Controller", "PI_Controller.ssp");
oms_delete("PI_Controller")
oms_importFile("PI_Controller.ssp");

oms_instantiate("PI_Controller")

-- set parameters
k = 100.0
yMax = 12.0
yMin = -yMax
wp = 1.0
Ni = 0.1
xi_start = 0.0
oms_setReal("PI_Controller.co_sim.addP.k1", wp)
oms_setReal("PI_Controller.co_sim.addP.k2", -1.0)
oms_setReal("PI_Controller.co_sim.addI.k2", -1.0)
oms_setReal("PI_Controller.co_sim.I.y_start", xi_start)
oms_setReal("PI_Controller.co_sim.I.k", 10)
oms_setReal("PI_Controller.co_sim.gainPI.k", k)
oms_setReal("PI_Controller.co_sim.limiter.uMax", yMax)
oms_setReal("PI_Controller.co_sim.addSat.k2", -1.0)
oms_setReal("PI_Controller.co_sim.gainTrack.k", 1.0/(k*Ni))

print("info:    Parameter settings")
print("info:      PI_Controller.co_sim.addP.k1: " .. oms_getReal("PI_Controller.co_sim.addP.k1"))
print("info:      PI_Controller.co_sim.addP.k2: " .. oms_getReal("PI_Controller.co_sim.addP.k2"))
print("info:      PI_Controller.co_sim.addI.k2: " .. oms_getReal("PI_Controller.co_sim.addI.k2"))
print("info:      PI_Controller.co_sim.I.y_start: " .. oms_getReal("PI_Controller.co_sim.I.y_start"))
print("info:      PI_Controller.co_sim.gainPI.k: " .. oms_getReal("PI_Controller.co_sim.gainPI.k"))
print("info:      PI_Controller.co_sim.limiter.uMax: " .. oms_getReal("PI_Controller.co_sim.limiter.uMax"))
print("info:      PI_Controller.co_sim.addSat.k2: " .. oms_getReal("PI_Controller.co_sim.addSat.k2"))
print("info:      PI_Controller.co_sim.gainTrack.k: " .. oms_getReal("PI_Controller.co_sim.gainTrack.k"))

oms_initialize("PI_Controller")
print("info:    Initialization")
print("info:      limiter.u: " .. oms_getReal("PI_Controller.co_sim.limiter.u"))
print("info:      limiter.y: " .. oms_getReal("PI_Controller.co_sim.limiter.y"))

oms_simulate("PI_Controller")
print("info:    Simulation")
print("info:      limiter.u: " .. oms_getReal("PI_Controller.co_sim.limiter.u"))
print("info:      limiter.y: " .. oms_getReal("PI_Controller.co_sim.limiter.y"))

oms_terminate("PI_Controller")
oms_delete("PI_Controller")

-- Result:
-- info:    Parameter settings
-- info:      PI_Controller.co_sim.addP.k1: 1.0
-- info:      PI_Controller.co_sim.addP.k2: -1.0
-- info:      PI_Controller.co_sim.addI.k2: -1.0
-- info:      PI_Controller.co_sim.I.y_start: 0.0
-- info:      PI_Controller.co_sim.gainPI.k: 100.0
-- info:      PI_Controller.co_sim.limiter.uMax: 12.0
-- info:      PI_Controller.co_sim.addSat.k2: -1.0
-- info:      PI_Controller.co_sim.gainTrack.k: 0.1
-- info:    Result file: PI_Controller.mat (bufferSize=100)
-- info:    Initialization
-- info:      limiter.u: 0.0
-- info:      limiter.y: 0.0
-- info:    Simulation
-- info:      limiter.u: -10.041439549286
-- info:      limiter.y: -10.041439549286
-- endResult
