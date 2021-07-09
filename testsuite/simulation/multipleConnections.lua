-- status: correct
-- teardown_command: rm -rf multipleConnections_lua/
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./multipleConnections_lua/")
oms_setWorkingDirectory("./multipleConnections_lua/")

oms_newModel("model")

oms_addSystem("model.System", oms_system_wc)
oms_addConnector("model.System.Input1", oms_causality_input, oms_signal_type_real)
oms_addConnector("model.System.Input2", oms_causality_input, oms_signal_type_real)
oms_addConnector("model.System.parameter1", oms_causality_parameter, oms_signal_type_real)
oms_addConnector("model.System.parameter2", oms_causality_parameter, oms_signal_type_real)

oms_addConnector("model.System.Output1", oms_causality_output, oms_signal_type_real)

oms_setReal("model.System.Input1", 10)
oms_setReal("model.System.Input2", 20)
oms_setReal("model.System.parameter1", 30)
oms_setReal("model.System.parameter1", 40)

-- instantiate FMUs
oms_addSubModel("model.System.add", "../../resources/Modelica.Blocks.Math.Add.fmu")
oms_addSubModel("model.System.gain", "../../resources/Modelica.Blocks.Math.Gain.fmu")

-- valid connections, inputs -> multiple components inputs
oms_addConnection("model.System.Input1", "model.System.add.u1")
oms_addConnection("model.System.Input1", "model.System.gain.u")
oms_addConnection("model.System.parameter1", "model.System.gain.k")
oms_addConnection("model.System.gain.y", "model.System.add.u2")

-- illegal connections
-- input -> already connected input
oms_addConnection("model.System.Input2", "model.System.add.u1")
-- parameter -> already connected parameter
oms_addConnection("model.System.parameter2", "model.System.gain.k")
-- output -> already connected input
oms_addConnection("model.System.Output1", "model.System.add.u2")

oms_instantiate("model")
oms_initialize("model")
oms_simulate("model")

print("info:    Simulation")
print("info:      model.System.Input1         : " .. oms_getReal("model.System.Input1"))
print("info:      model.System.Input2         : " .. oms_getReal("model.System.Input2"))
print("info:      model.System.parameter1     : " .. oms_getReal("model.System.parameter1"))

print("info:      model.System.add.u1         : " .. oms_getReal("model.System.add.u1"))
print("info:      model.System.add.u2         : " .. oms_getReal("model.System.add.u2"))
print("info:      model.System.gain.u         : " .. oms_getReal("model.System.gain.u"))
print("info:      model.System.gain.k         : " .. oms_getReal("model.System.gain.k"))

-- Result:
-- error:   [addConnection] Connector add.u1 is already connected to Input1
-- error:   [addConnection] Connector gain.k is already connected to parameter1
-- error:   [addConnection] Connector add.u2 is already connected to gain.y
-- info:    Result file: model_res.mat (bufferSize=10)
-- info:    Simulation
-- info:      model.System.Input1         : 10.0
-- info:      model.System.Input2         : 20.0
-- info:      model.System.parameter1     : 40.0
-- info:      model.System.add.u1         : 10.0
-- info:      model.System.add.u2         : 400.0
-- info:      model.System.gain.u         : 10.0
-- info:      model.System.gain.k         : 40.0
-- info:    0 warnings
-- info:    3 errors
-- endResult
