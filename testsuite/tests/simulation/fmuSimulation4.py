## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: yes

from OMSimulator import FMU

# Values that are set before the FMU is instantiated are applied when it is
# instantiated; all of them, not only the first one.

fmu = FMU('../resources/Modelica.Blocks.Math.Gain.fmu', instanceName='Gain')

fmu.setValue('k', 5.0)
fmu.setValue('u', 10.0)

fmu.instantiate()
fmu.setResultFile('Gain_res.mat')

print(f"info: After instantiation:")
print(f"info:    k: {fmu.getValue('k')}", flush=True)
print(f"info:    u: {fmu.getValue('u')}", flush=True)

fmu.initialize()
print(f"info: After initialization:")
print(f"info:    y: {fmu.getValue('y')}", flush=True)

fmu.terminate()
fmu.delete()

## Result:
## info: After instantiation:
## info:    k: 5.0
## info:    u: 10.0
## info:    Result file: Gain_res.mat (bufferSize=10)
## info: After initialization:
## info:    y: 50.0
## endResult
