## status: correct
## linux: yes
## ucrt64: no
## win: no
## mac: no
## asan: no

from OMSimulator import FMU

# A single model-exchange FMU in DAE mode (fmi-ls-dae), through the FMU API.
# DaeLoopA is described in DaeMode1.py; here its input is a constant, u = 0.5,
# so x(t) = 0.5 + 0.5*exp(-t), y = (x - u)/3 and z = u + y.
# Selecting IDA by name and leaving the default CVODE give the same result:
# a DAE-mode component has no explicit ODE, so the system switches to IDA itself.

for solver in ['ida', 'default']:
  fmu = FMU('../resources/DaeLoopA.fmu', instanceName=f'DaeLoopA_{solver}')
  fmu.stopTime = 1.0
  fmu.instantiate()
  if solver != 'default':
    fmu.setSolver(solver)
  fmu.setResultFile(f'DaeMode2_{solver}_res.mat')
  fmu.setValue('u', 0.5)
  fmu.initialize()
  fmu.simulate()
  print(f"info: {solver}: x = {fmu.getValue('x'):.6f}, y = {fmu.getValue('y'):.6f}, z = {fmu.getValue('z'):.6f}", flush=True)
  fmu.terminate()
  fmu.delete()
## Result:
## info:    FMU "DaeLoopA_ida" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 3 residuals over 1 states and 2 algebraic variables
## info:    system "DaeLoopA_ida.root": DAE Jacobian 3x3, 6 structural nonzeros, 3 colour(s) — sparse (KLU), one residual evaluation per colour instead of one per unknown
## info:    Result file: DaeMode2_ida_res.mat (bufferSize=10)
## info: ida: x = 0.683940, y = 0.061313, z = 0.561313
## info:    Final Statistics for 'DaeLoopA_ida.root':
##          NumSteps = 511 NumResEvals  = 514 NumLinSolvSetups = 13
##          NumNonlinSolvIters = 512 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## info:    FMU "DaeLoopA_default" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 3 residuals over 1 states and 2 algebraic variables
## info:    system "DaeLoopA_default.root" contains a component in DAE mode (fmi-ls-dae); integrating with IDA instead of cvode
## info:    system "DaeLoopA_default.root": DAE Jacobian 3x3, 6 structural nonzeros, 3 colour(s) — sparse (KLU), one residual evaluation per colour instead of one per unknown
## info:    Result file: DaeMode2_default_res.mat (bufferSize=10)
## info: default: x = 0.683940, y = 0.061313, z = 0.561313
## info:    Final Statistics for 'DaeLoopA_default.root':
##          NumSteps = 511 NumResEvals  = 514 NumLinSolvSetups = 13
##          NumNonlinSolvIters = 512 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## endResult
