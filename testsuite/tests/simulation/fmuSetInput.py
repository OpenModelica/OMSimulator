## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: yes

from OMSimulator import FMU

# Sets an input before and after initialize(). Before initialize() the value
# is a start value; after it, it goes to the FMU at once. y = k*u shows which
# value the FMU uses.

for mode, solver in [('cs', 'ma'), ('me', 'euler')]:
  fmu = FMU('../resources/Modelica.Blocks.Math.Gain.fmu', instanceName=f'Gain_{mode}')
  fmu.mode = mode
  fmu.instantiate()
  fmu.setSolver(solver)
  fmu.setStopTime(2.0)
  fmu.setValue('k', 2.0)
  fmu.setValue('u', 1.0)
  fmu.initialize()
  print(f"info: {mode}: after initialize: u = {fmu.getValue('u'):.4f}, y = {fmu.getValue('y'):.4f}", flush=True)
  fmu.setValue('u', 3.0)
  fmu.stepUntil(1.0)
  print(f"info: {mode}: after t = 1:      u = {fmu.getValue('u'):.4f}, y = {fmu.getValue('y'):.4f}", flush=True)
  fmu.setValue('u', 5.0)
  fmu.stepUntil(2.0)
  print(f"info: {mode}: after t = 2:      u = {fmu.getValue('u'):.4f}, y = {fmu.getValue('y'):.4f}", flush=True)
  fmu.terminate()
  fmu.delete()

  
## Result:
## info:    Result file: Gain_cs_res.mat (bufferSize=10)
## info: cs: after initialize: u = 1.0000, y = 2.0000
## info: cs: after t = 1:      u = 3.0000, y = 6.0000
## info: cs: after t = 2:      u = 5.0000, y = 10.0000
## info:    model doesn't contain any continuous state
## info:    Result file: Gain_me_res.mat (bufferSize=10)
## info: me: after initialize: u = 1.0000, y = 2.0000
## info: me: after t = 1:      u = 3.0000, y = 6.0000
## info: me: after t = 2:      u = 5.0000, y = 10.0000
## endResult
