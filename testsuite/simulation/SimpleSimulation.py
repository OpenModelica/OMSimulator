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
print(f'status: {status}')
status = Capi.initialize(model)
print(f'status: {status}')
status = Capi.simulate(model)
print(f'status: {status}')
status = Capi.simulate(model)
print(f'status: {status}')
status = Capi.terminate(model)
print(f'status: {status}')

## Result:
## status: Status.ok
## status: Status.ok
## status: Status.ok
## status: Status.ok
## status: Status.ok
## endResult
