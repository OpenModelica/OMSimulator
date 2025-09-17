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

fmu = FMU('../resources/Modelica.Blocks.Sources.Sine.fmu', instanceName='Sine')

fmu.instantiate()
fmu.setResultFile('Sine_res.mat')


fmu.initialize()

current_time = 0.0
stop_time = 1.0
step_size = 0.1
while current_time < stop_time:
  # advance simulation to the next time
  current_time = round(current_time + step_size, 10) # avoid floating point arithmetic issues
  fmu.stepUntil(current_time)
  print(f"info:    time: {current_time}, y: {fmu.getValue('y')}", flush=True)

fmu.terminate()
fmu.delete()

## Result:
## info:    Result file: Sine_res.mat (bufferSize=1)
## info:    time: 0.1, y: 0.5877852522924735
## info:    time: 0.2, y: 0.9510565162951539
## info:    time: 0.3, y: 0.9510565162951532
## info:    time: 0.4, y: 0.5877852522924718
## info:    time: 0.5, y: -2.0979854138680754e-15
## info:    time: 0.6, y: -0.5877852522924751
## info:    time: 0.7, y: -0.9510565162951546
## info:    time: 0.8, y: -0.9510565162951525
## info:    time: 0.9, y: -0.5877852522924697
## info:    time: 1.0, y: -2.4492127076447545e-16
## endResult
