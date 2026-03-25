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

## add components to default system
model.addComponent(CRef('default', 'edrive_mass'), 'resources/edrive_mass.fmu')

solver1 = {'name' : 'solver1',  'method': 'cvode', 'tolerance': 1e-4}
model.newSolver(solver1)

model.setSolver(CRef('default', 'edrive_mass'), 'solver1')

instantiated_model = model.instantiate()
instantiated_model.setStopTime(10.0)
instantiated_model.setResultFile("dc_motor_me_res.mat")
print(instantiated_model.dumpApiCalls())
instantiated_model.initialize()
instantiated_model.simulate()
instantiated_model.terminate()
instantiated_model.delete()
