## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: yes

from OMSimulator import FMU

# This test case selects every solver the FMU API offers, by name, for both
# kinds of an FMU that exports Co-Simulation and Model Exchange. The names
# are mapped to the values of oms_solver_enu_t; a solver added to that enum
# renumbers the ones after it, and a stale value makes setSolver fail.

for mode, solvers in [('cs', ['ma']), ('me', ['euler', 'cvode'])]:
  for solver in solvers:
    fmu = FMU('../resources/Dahlquist3.fmu', instanceName=f'Dahlquist_{mode}_{solver}')
    fmu.mode = mode
    fmu.stopTime = 1.0
    fmu.stepSize = 0.1
    fmu.instantiate()
    fmu.setSolver(solver)
    fmu.setResultFile(f'fmuSimulation3_{mode}_{solver}_res.mat')
    fmu.initialize()
    fmu.simulate()
    print(f"info: {mode} {solver}: x = {fmu.getValue('x'):.4f}", flush=True)
    fmu.terminate()
    fmu.delete()
## Result:
## info:    Result file: fmuSimulation3_cs_ma_res.mat (bufferSize=10)
## info: cs ma: x = 0.3487
## info:    Result file: fmuSimulation3_me_euler_res.mat (bufferSize=10)
## info: me euler: x = 0.3487
## info:    maximum step size for 'Dahlquist_me_cvode.root': 0.100000
## info:    Result file: fmuSimulation3_me_cvode_res.mat (bufferSize=10)
## info: me cvode: x = 0.3679
## info:    Final Statistics for 'Dahlquist_me_cvode.root':
##          NumSteps = 29 NumRhsEvals  = 39 NumLinSolvSetups = 9
##          NumNonlinSolvIters = 38 NumNonlinSolvConvFails = 0 NumErrTestFails = 1
## endResult
