## status: correct
## teardown_command:
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import Capi, Settings

Settings.suppressPath = True

# read json object from file
with open('../resources/sim.json', 'r') as file:
  json_data = file.read()

model, status = Capi.instantiateFromJson(json_data)
print(f'instantiateFromJson: {status}')
status = Capi.initialize(model)
print(f'initialize: {status}')
status = Capi.simulate(model)
print(f'simulate: {status}')
status = Capi.terminate(model)
print(f'terminate: {status}')

## Result:
## instantiateFromJson: Status.ok
## initialize: Status.error
## simulate: Status.error
## terminate: Status.error
## endResult
