## status: correct
## linux: yes
## ucrt64: no
## win: no
## mac: no
## the checked-in FMU carries linux64 binaries only; rebuild it with the
## cross-build platforms in resources/ClockedStepBoundary.mos to widen this

from OMSimulator import FMU, Settings

Settings.suppressPath = True

# A clocked FMU integrated with CVode. Modelica.Clocked computes its tick times
# exactly (k * period), while doStepCVODE accumulates the end of each
# communication step. Once the accumulated end lands an ULP past a tick, the
# interval left over after the event at that tick is smaller than CVode can
# integrate, and CVode used to be called with it anyway - failing the whole
# simulation with CV_TOO_CLOSE (flag -27) at t = 0.08.

model = FMU('../resources/ClockedStepBoundary.fmu')
model.mode = 'me'
model.startTime = 0.0
model.stopTime = 0.2
model.tolerance = 1e-8
model.instantiate()
model.setSolver('cvode')
model.setResultFile('ClockedStepBoundary_res.mat')
model.initialize()
model.simulate()
model.terminate()
model.delete()

print('simulation finished', flush=True)

## Result:
## info:    maximum step size for 'AbsoluteClocks.root': 0.008000
## info:    Result file: ClockedStepBoundary_res.mat (bufferSize=10)
## info:    Parameter AbsoluteClocks.fastClock.solverMethod will not be stored in the result file, because the signal type is not supported
## info:    Parameter AbsoluteClocks.slowClock.solverMethod will not be stored in the result file, because the signal type is not supported
## info:    Final Statistics for 'AbsoluteClocks.root':
##          NumSteps = 10 NumRhsEvals  = 15 NumLinSolvSetups = 8
##          NumNonlinSolvIters = 14 NumNonlinSolvConvFails = 0 NumErrTestFails = 1
## simulation finished
## endResult
