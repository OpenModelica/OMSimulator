-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./setExternalInputs_lua/")

oms_newModel("setExternalInputs")

oms_addSystem("setExternalInputs.Root", oms_system_wc)

oms_addSubModel("setExternalInputs.Root.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")

oms_setStopTime("setExternalInputs", 5.0)

oms_instantiate("setExternalInputs")
oms_initialize("setExternalInputs")

oms_stepUntil("setExternalInputs", 1.0)
print("info:      setExternalInputs.Root.Gain.u at time 1.0: " .. oms_getReal("setExternalInputs.Root.Gain.u"))

-- set external input
oms_setReal("setExternalInputs.Root.Gain.u", 5.0)

oms_stepUntil("setExternalInputs", 3.0)
print("info:      setExternalInputs.Root.Gain.u at time 3.0: " .. oms_getReal("setExternalInputs.Root.Gain.u"))

-- set external input
oms_setReal("setExternalInputs.Root.Gain.u", 10.0)

oms_stepUntil("setExternalInputs", 5.0)
print("info:      setExternalInputs.Root.Gain.u at time 5.0: " .. oms_getReal("setExternalInputs.Root.Gain.u"))

src = oms_exportSnapshot("setExternalInputs")
print(src)

oms_terminate("setExternalInputs")
oms_delete("setExternalInputs")

-- Result:
-- info:    Result file: setExternalInputs_res.mat (bufferSize=10)
-- info:      setExternalInputs.Root.Gain.u at time 1.0: 0.0
-- info:      setExternalInputs.Root.Gain.u at time 3.0: 5.0
-- info:      setExternalInputs.Root.Gain.u at time 5.0: 10.0
-- error:   [exportSnapshot] Not implemented
--
-- info:    0 warnings
-- info:    1 errors
-- endResult
