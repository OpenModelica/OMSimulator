## status: correct
## teardown_command: rm -rf getDirectionalDerivative_CS_py/
## linux: no
## mingw32: no
## mingw64: yes
## win: yes
## mac: no


from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./getDirectionalDerivative_CS_py/")

oms.newModel("model")

oms.addSystem("model.root", oms.system_sc)

oms.addSubModel("model.root.CauerLowPassAnalog", "../resources/Modelica.Electrical.Analog.Examples.CauerLowPassAnalog.fmu")

oms.setResultFile("model", "getDirectionalDerivatives_ME_Py.mat", 10)

oms.instantiate("model")

oms.initialize("model")
## error msg for wrong cref
oms.getDirectionalDerivative("model.root.CauerLowPassAnalog.der(C1.v2)")

print("info:    Partial Derivatives after Initialization")
print("info:      model.root.CauerLowPassAnalog.der(C1.v): " , oms.getDirectionalDerivative("model.root.CauerLowPassAnalog.der(C1.v)")[0])
print("info:      model.root.CauerLowPassAnalog.der(C3.v): " , oms.getDirectionalDerivative("model.root.CauerLowPassAnalog.der(C3.v)")[0])
print("info:      model.root.CauerLowPassAnalog.der(C5.v): " , oms.getDirectionalDerivative("model.root.CauerLowPassAnalog.der(C5.v)")[0])
print("info:      model.root.CauerLowPassAnalog.der(L1.i): " , oms.getDirectionalDerivative("model.root.CauerLowPassAnalog.der(L1.i)")[0])
print("info:      model.root.CauerLowPassAnalog.der(L2.i): " , oms.getDirectionalDerivative("model.root.CauerLowPassAnalog.der(L2.i)")[0])


oms.terminate("model")
oms.delete("model")

## Result:
## info:    maximum step size for 'model.root': 0.001000
## info:    Result file: getDirectionalDerivatives_ME_Py.mat (bufferSize=10)
## error:   [getDirectionalDerivative] Unknown signal "model.root.CauerLowPassAnalog.der(C1.v2)"
## info:    Final Statistics for 'model.root':
##          NumSteps = 0 NumRhsEvals  = 0 NumLinSolvSetups = 0
##          NumNonlinSolvIters = 0 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## info:    Partial Derivatives after Initialization
## info:      model.root.CauerLowPassAnalog.der(C1.v):  -1.531402597762616
## info:      model.root.CauerLowPassAnalog.der(C3.v):  -0.17304778059738535
## info:      model.root.CauerLowPassAnalog.der(C5.v):  -0.09263294992243676
## info:      model.root.CauerLowPassAnalog.der(L1.i):  0.0
## info:      model.root.CauerLowPassAnalog.der(L2.i):  0.0
## info:    0 warnings
## info:    1 errors
## endResult
