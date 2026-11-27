## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: no
## the FMUs this test loads leak memory inside their own binaries

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True


# A table in the root system connected to the input of a model-exchange FMU
# with its own solver, i.e. in the subsystem 'model.root.solver' (#1702).
# der(x) = a*x + u with a = -1, x(0) = 1 and u = 10: x(2) = 10 - 9*exp(-2) = 8.782

with open('u.csv', 'w') as file:
  file.write('time,u\n0,10\n2,10\n')

model = SSP()
model.addResource('../resources/HelloWorldWithInput.fmu', new_name='resources/hw.fmu')
model.addComponent(CRef('default', 'hw'), 'resources/hw.fmu')
model.addResource('u.csv', new_name='resources/u.csv')
model.addComponent(CRef('default', 'table'), 'resources/u.csv')
model.addConnection(CRef('default', 'table', 'u'), CRef('default', 'hw', 'u'))
model.getElement(CRef('default', 'hw')).implementation = 'me'
model.newSolver({'name': 'solver', 'method': 'cvode', 'relativeTolerance': 1e-6})
model.setSolver(CRef('default', 'hw'), 'solver')

instantiated_model = model.instantiate()
instantiated_model.setStopTime(2.0)
instantiated_model.setFixedStepSize(0.1)
instantiated_model.setResultFile('tableToModelExchange1_res.mat')
instantiated_model.initialize()
instantiated_model.simulate()
print(f"info:    default.hw.x: {instantiated_model.getValue(CRef('default', 'hw', 'x'))}", flush=True)
instantiated_model.terminate()
instantiated_model.delete()


## Result:
## info:    maximum step size for 'model.root.solver': 0.001000
## info:    Result file: tableToModelExchange1_res.mat (bufferSize=10)
## info:    default.hw.x: 8.781982913256808
## info:    Final Statistics for 'model.root.solver':
##          NumSteps = 2002 NumRhsEvals  = 2005 NumLinSolvSetups = 102
##          NumNonlinSolvIters = 2004 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## endResult
