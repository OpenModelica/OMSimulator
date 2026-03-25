## status: correct
## teardown_command: rm -rf dcmotor.ssp tmp-dcmotor/
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

## This example test dc motors fmu's with ME solver

import logging
logging.basicConfig(level=logging.INFO)

from OMSimulator import SSD, SSP, Settings, CRef

Settings.suppressPath = True
model = SSP()
model.addResource('../resources/dcmotor/resources/edrive_mass.fmu', new_name='resources/edrive_mass.fmu')
model.addResource('../resources/dcmotor/resources/emachine_model.fmu', new_name='resources/emachine_model.fmu')
model.addResource('../resources/dcmotor/resources/stimuli_model.fmu', new_name='resources/stimuli_model.fmu')

## add components to default system
model.addComponent(CRef('default', 'edrive_mass'), 'resources/edrive_mass.fmu')
model.addComponent(CRef('default', 'emachine_model'), 'resources/emachine_model.fmu')
model.addComponent(CRef('default', 'stimuli_model'), 'resources/stimuli_model.fmu')

solver1 = {'name' : 'solver1',  'method': 'cvode', 'tolerance': 1e-4}
model.newSolver(solver1)

model.setSolver(CRef('default', 'edrive_mass'), 'solver1')
model.setSolver(CRef('default', 'emachine_model'), 'solver1')
model.setSolver(CRef('default', 'stimuli_model'), 'solver1')


instantiated_model = model.instantiate()
instantiated_model.setStopTime(10.0)
instantiated_model.setResultFile("dc_motor_me_res.mat")
#print(instantiated_model.dumpApiCalls())
instantiated_model.initialize()
instantiated_model.simulate()
instantiated_model.terminate()
instantiated_model.delete()


## Result:
## INFO:OMSimulator.ssp:Temporary directory created: <hidden>
## warning: [fmi2Warning] stimuli_model/: fmi2SetupExperiment: tolerance control not supported for fmuType fmi2ModelExchange, setting toleranceDefined to fmi2False
## warning: [fmi2Warning] emachine_model/: fmi2SetupExperiment: tolerance control not supported for fmuType fmi2ModelExchange, setting toleranceDefined to fmi2False
## warning: [fmi2Warning] edrive_mass/: fmi2SetupExperiment: tolerance control not supported for fmuType fmi2ModelExchange, setting toleranceDefined to fmi2False
## info:    maximum step size for 'model.root': 0.001000
## info:    Result file: dc_motor_me_res.mat (bufferSize=1)
## info:    Final Statistics for 'model.root':
##          NumSteps = 10001 NumRhsEvals  = 10002 NumLinSolvSetups = 501
##          NumNonlinSolvIters = 10001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## INFO:OMSimulator.ssp:Temporary directory removed: <hidden>
## info:    3 warnings
## info:    0 errors
## endResult
