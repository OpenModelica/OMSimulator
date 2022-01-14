-- status: correct
-- teardown_command: rm -rf getDirectionalDerivative_ME_lua/
-- linux: no
-- mingw32: no
-- mingw64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./getDirectionalDerivative_ME_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_sc)

oms_addSubModel("model.root.CauerLowPassAnalog", "../resources/Modelica.Electrical.Analog.Examples.CauerLowPassAnalog.fmu")

oms_setResultFile("model", "getDirectionalDerivatives_ME.mat", 10)

oms_instantiate("model")

oms_initialize("model")
-- error msg for wrong cref
oms_getDirectionalDerivative("model.root.CauerLowPassAnalog.der(C1.v2)")

print("info:    Partial Derivatives after Initialization")
print("info:      model.root.CauerLowPassAnalog.der(C1.v): " .. oms_getDirectionalDerivative("model.root.CauerLowPassAnalog.der(C1.v)"))
print("info:      model.root.CauerLowPassAnalog.der(C3.v): " .. oms_getDirectionalDerivative("model.root.CauerLowPassAnalog.der(C3.v)"))
print("info:      model.root.CauerLowPassAnalog.der(C5.v): " .. oms_getDirectionalDerivative("model.root.CauerLowPassAnalog.der(C5.v)"))
print("info:      model.root.CauerLowPassAnalog.der(L1.i): " .. oms_getDirectionalDerivative("model.root.CauerLowPassAnalog.der(L1.i)"))
print("info:      model.root.CauerLowPassAnalog.der(L2.i): " .. oms_getDirectionalDerivative("model.root.CauerLowPassAnalog.der(L2.i)"))


oms_terminate("model")
oms_delete("model")


-- Result:
-- info:    maximum step size for 'model.root': 0.001000
-- info:    Result file: getDirectionalDerivatives_ME.mat (bufferSize=10)
-- error:   [getDirectionalDerivative] Unknown signal "model.root.CauerLowPassAnalog.der(C1.v2)"
-- info:    Partial Derivatives after Initialization
-- info:      model.root.CauerLowPassAnalog.der(C1.v): -1.5314025977626
-- info:      model.root.CauerLowPassAnalog.der(C3.v): -0.17304778059739
-- info:      model.root.CauerLowPassAnalog.der(C5.v): -0.092632949922437
-- info:      model.root.CauerLowPassAnalog.der(L1.i): 0.0
-- info:      model.root.CauerLowPassAnalog.der(L2.i): 0.0
-- info:    Final Statistics for 'model.root':
--          NumSteps = 0 NumRhsEvals  = 0 NumLinSolvSetups = 0
--          NumNonlinSolvIters = 0 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
-- info:    0 warnings
-- info:    1 errors
-- endResult
