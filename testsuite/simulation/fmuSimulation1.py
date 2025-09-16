## status: correct
## teardown_command:
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

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

print(f"info: After instantiation:")
print(f"info:    k: {fmu.getValue('k')}", flush=True)
print(f"info:    u: {fmu.getValue('u')}", flush=True)
print(f"info:    y: {fmu.getValue('y')}", flush=True)

fmu.initialize()
fmu.simulate()
print(f"info: After Simulation:")
print(f"info:    k: {fmu.getValue('k')}", flush=True)
print(f"info:    u: {fmu.getValue('u')}", flush=True)
print(f"info:    y: {fmu.getValue('y')}", flush=True)

fmu.terminate()
fmu.delete()

## Result:
## info: After instantiation:
## info:    k: 5.0
## info:    u: 10.0
## info:    y: 50.0
## info:    Result file: Gain_res.mat (bufferSize=1)
## info: After Simulation:
## info:    k: 5.0
## info:    u: 10.0
## info:    y: 50.0
## endResult
