-- status: correct
-- teardown_command: rm -rf fmidertest_lua/
-- linux: no
-- mingw32: no
-- mingw64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./fmidertest_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.fmidertest", "../resources/fmidertest.fmu")

oms_setResultFile("model", "fmidertest_CS.mat", 10)

oms_instantiate("model")

print("info:    Partial Derivatives during Initialization")
print("info:      model.root.fmidertest.x w.r.t u1     : " .. oms_getDirectionalDerivative("model.root.fmidertest.x", "u1"))
print("info:      model.root.fmidertest.x w.r.t u2     : " .. oms_getDirectionalDerivative("model.root.fmidertest.x", "u2"))

print("info:      model.root.fmidertest.der(x) w.r.t u1: " .. oms_getDirectionalDerivative("model.root.fmidertest.der(x)", "u1"))
print("info:      model.root.fmidertest.der(x) w.r.t u2: " .. oms_getDirectionalDerivative("model.root.fmidertest.der(x)", "u2"))

print("info:      model.root.fmidertest.y w.r.t u1     : " .. oms_getDirectionalDerivative("model.root.fmidertest.y", "u1"))
print("info:      model.root.fmidertest.y w.r.t u2     : " .. oms_getDirectionalDerivative("model.root.fmidertest.y", "u2"))

print("info:    Partial Derivatives during Initialization sum of Jacobians")
print("info:      model.root.fmidertest.x              : " .. oms_getDirectionalDerivative("model.root.fmidertest.x"))
print("info:      model.root.fmidertest.der(x)         : " .. oms_getDirectionalDerivative("model.root.fmidertest.der(x)"))
print("info:      model.root.fmidertest.y              : " .. oms_getDirectionalDerivative("model.root.fmidertest.y"))

oms_initialize("model")


print("info:    Partial Derivatives after Initialization")
print("info:      model.root.fmidertest.der(x)         : " .. oms_getDirectionalDerivative("model.root.fmidertest.der(x)"))
print("info:      model.root.fmidertest.y              : " .. oms_getDirectionalDerivative("model.root.fmidertest.y"))

oms_terminate("model")
oms_delete("model")

-- Result:
-- info:    Partial Derivatives during Initialization
-- info:      model.root.fmidertest.x     : 4.0
-- info:      model.root.fmidertest.der(x): 11.0
-- info:      model.root.fmidertest.y     : 11.0
-- info:    Result file: fmidertest_CS.mat (bufferSize=10)
-- info:    Partial Derivatives after Initialization
-- info:      model.root.fmidertest.der(x): 8.0
-- info:      model.root.fmidertest.y     : 5.0
-- endResult
