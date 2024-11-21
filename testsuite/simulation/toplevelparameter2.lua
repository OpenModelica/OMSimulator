-- status: correct
-- teardown_command: rm -rf toplevelparameters1/
-- linux: no
-- ucrt64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./toplevelparameters2/")
oms_setWorkingDirectory("./toplevelparameters2/")


oms_importFile("../../resources/toplevelparameter2.ssp")

oms_setResultFile("model", "toplevelparameter2.mat")

oms_instantiate("model")
oms_initialize("model")
oms_simulate("model")

print("info:    Parameter values")
print("info:      model.root.addP.k1     : " .. oms_getReal("model.root.addP.k1"))
print("info:      model.root.addP.k2     : " .. oms_getReal("model.root.addP.k2"))

-- Top level cref's
print("info:    Top-System-Level-Vars")
print("info:      model.root.Input_cref  : " .. oms_getReal("model.root.Input_cref"))


oms_terminate("model")
oms_delete("model")

-- Result:
-- info:    Result file: toplevelparameter2.mat (bufferSize=1)
-- info:    Parameter values
-- info:      model.root.addP.k1     : -200.0
-- info:      model.root.addP.k2     : -400.0
-- info:    Top-System-Level-Vars
-- info:      model.root.Input_cref  : -20.0
-- endResult
