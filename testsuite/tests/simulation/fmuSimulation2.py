## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: yes

from OMSimulator import FMU, Variable, SignalType, Causality

# This test case tests the new python API construct
# FMU which basically loads the FMU ,instantiates, initializes, simulates and
# terminates the FMU

fmu = FMU('../resources/Modelica.Blocks.Math.Gain.fmu', instanceName='Gain')

fmu.instantiate()
fmu.setResultFile('Gain_res.mat')

## Set the parameter and input values
fmu.setValue('k', 5.0)
fmu.setValue('u', 10.0)

fmu.setStopTime(10.0)

fmu.setStepSize(0.1)

fmu.setTolerance(1e-6)

fmu.initialize()
print(f"info: After initialization:")
print(f"info:    k: {fmu.getValue('k')}", flush=True)
print(f"info:    u: {fmu.getValue('u')}", flush=True)

fmu.simulate()
print(f"info: After Simulation:")
print(f"info:    k: {fmu.getValue('k')}", flush=True)
print(f"info:    u: {fmu.getValue('u')}", flush=True)
print(f"info:    y: {fmu.getValue('y')}", flush=True)
fmu.reset()
fmu.setValue('k', 100.0)
fmu.setValue('u', 200.0)
fmu.initialize()
print(f"info: After Reset:")
print(f"info:    k: {fmu.getValue('k')}", flush=True)
print(f"info:    u: {fmu.getValue('u')}", flush=True)
print(f"info:    y: {fmu.getValue('y')}", flush=True)

fmu.terminate()
fmu.delete()

## Result:
## info:    Result file: Gain_res.mat (bufferSize=10)
## info: After initialization:
## info:    k: 5.0
## info:    u: 10.0
## info: After Simulation:
## info:    k: 5.0
## info:    u: 10.0
## info:    y: 50.0
## info:    Result file: Gain_res.mat (bufferSize=10)
## info: After Reset:
## info:    k: 100.0
## info:    u: 200.0
## info:    y: 20000.0
## endResult
