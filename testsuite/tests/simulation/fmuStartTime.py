## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: yes

from OMSimulator import FMU

# TODO: fails until a start time set after instantiate() reaches the FMU
# (fmi2SetupExperiment) and the solver; both still start at t = 0.

# Simulates der(x) = -x from a start time other than zero, with the start
# time set after instantiate(). The start value of x applies at the start
# time, so with x = 2 at t = 3 and 20 Euler steps of 0.1, x(5) = 2*0.9^20.

for mode, solver in [('cs', 'ma'), ('me', 'euler')]:
  fmu = FMU('../resources/Dahlquist.fmu', instanceName=f'Dahlquist_{mode}')
  fmu.mode = mode
  fmu.stepSize = 0.1
  fmu.instantiate()
  fmu.setSolver(solver)
  fmu.setStartTime(3.0)
  fmu.setStopTime(5.0)
  fmu.setValue('x', 2.0)
  fmu.initialize()
  print(f"info: {mode}: after initialize: time = {fmu.getValue('time'):.4f}, x = {fmu.getValue('x'):.4f}", flush=True)
  fmu.simulate()
  print(f"info: {mode}: after simulate:   time = {fmu.getValue('time'):.4f}, x = {fmu.getValue('x'):.4f}", flush=True)
  fmu.terminate()
  fmu.delete()
## Result:
## info:    Result file: Dahlquist_cs_res.mat (bufferSize=10)
## info: cs: after initialize: time = 3.0000, x = 2.0000
## info: cs: after simulate:   time = 5.0000, x = 0.2432
## info:    Result file: Dahlquist_me_res.mat (bufferSize=10)
## info: me: after initialize: time = 3.0000, x = 2.0000
## info: me: after simulate:   time = 5.0000, x = 0.2432
## endResult
