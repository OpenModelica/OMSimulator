## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: yes

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True


model = SSP()
model.addResource('../resources/DualMassOscillator.System1.fmu', new_name='resources/system1.fmu')
model.addResource('../resources/DualMassOscillator.System2.fmu', new_name='resources/system2.fmu')


model.addComponent(CRef('default', 'system1'), 'resources/system1.fmu')
model.addComponent(CRef('default', 'system2'), 'resources/system2.fmu')

## add connection
model.addConnection(CRef('default', 'system1', 'F'),  CRef('default', 'system2', 'F'))
model.addConnection(CRef('default', 'system1', 'x1'), CRef('default', 'system2', 'x1'))
model.addConnection(CRef('default', 'system1', 'v1'), CRef('default', 'system2', 'v1'))
model.addConnection(CRef('default', 'system1', 'a1'), CRef('default', 'system2', 'a1'))

solver1 = {'name' : 'solver1',  'method': 'cvode', 'relativeTolerance': 1e-6, 'initialStepSize': 1e-12, 'minimumStepSize': 1e-12, 'maximumStepSize': 1e-3}
model.newSolver(solver1)

model.setSolver(CRef('default', 'system1'), 'solver1')
model.setSolver(CRef('default', 'system2'), 'solver1')


model.export('DualMassOscillator2.ssp')

model2 = SSP('DualMassOscillator2.ssp')

instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,
instantiated_model.setResultFile("DualMassOscillator2_res.mat")
instantiated_model.setStopTime(10)

instantiated_model.setFixedStepSize(1e-4)

## set value
instantiated_model.setValue(CRef('default', 'system1', 'x1_start'), 0.0)
instantiated_model.setValue(CRef('default', 'system2', 'x2_start'), 0.5)

instantiated_model.initialize()
print(f"info:    Initialization")
print(f"info:      default.system1.x1: {instantiated_model.getValue(CRef('default', 'system1', 'x1'))}")
print(f"info:      default.system2.x2: {instantiated_model.getValue(CRef('default', 'system2', 'x2'))}")

instantiated_model.simulate()
print(f"info:    Simulation")
print(f"info:      default.system1.x1: {instantiated_model.getValue(CRef('default', 'system1', 'x1'))}")
print(f"info:      default.system2.x2: {instantiated_model.getValue(CRef('default', 'system2', 'x2'))}")

instantiated_model.terminate()
instantiated_model.delete()

## Result:
## info:    maximum step size for 'model.root': 0.000100
## info:    Result file: DualMassOscillator2_res.mat (bufferSize=10)
## info:    Final Statistics for 'model.root':
##          NumSteps = 100001 NumRhsEvals  = 100002 NumLinSolvSetups = 5001
##          NumNonlinSolvIters = 100001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## info:    Initialization
## info:      default.system1.x1: 0.0
## info:      default.system2.x2: 0.5
## info:    Simulation
## info:      default.system1.x1: 0.05067159474969803
## info:      default.system2.x2: 0.03142555558313003
## endResult
