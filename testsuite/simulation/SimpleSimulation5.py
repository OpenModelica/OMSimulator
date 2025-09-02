## status: correct
## teardown_command: rm -rf SimpleSimulation5.ssp SimpleSimulation5_res.mat
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True


# This example creates a new SSP file with an FMU instantiated as a component and sets two differents solver for the components and the system.
# It then exports the SSP file and re-imports it to verify the solver settings and the simulates the model.

model = SSP()
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')

component1 = model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
component3 = model.addComponent(CRef('default', 'Gain1'), 'resources/Gain.fmu')


model.addConnection(CRef('default', 'Gain1', 'y'), CRef('default', 'Add1', 'u1'))

#model.list()
model.export('SimpleSimulation5.ssp')

model2 = SSP('SimpleSimulation5.ssp')

instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,
instantiated_model.setResultFile("SimpleSimulation5_res.mat")
instantiated_model.setValue(CRef('default', 'Gain1', 'k'), 2.0)
instantiated_model.setValue(CRef('default', 'Gain1', 'u'), 6.0)

print(f"info: After instantiation:")
print(f"info:    default.Gain1.k: {instantiated_model.getValue(CRef('default', 'Gain1', 'k'))}", flush=True)
print(f"info:    default.Gain1.u: {instantiated_model.getValue(CRef('default', 'Gain1', 'u'))}", flush=True)
print(f"info:    default.Gain1.y: {instantiated_model.getValue(CRef('default', 'Gain1', 'y'))}", flush=True)
print(f"info:    default.Add1.u1: {instantiated_model.getValue(CRef('default', 'Add1', 'u1'))}", flush=True)

instantiated_model.initialize()
instantiated_model.simulate()
print(f"info: After simulation:")
print(f"info:    default.Gain1.k: {instantiated_model.getValue(CRef('default', 'Gain1', 'k'))}", flush=True)
print(f"info:    default.Gain1.u: {instantiated_model.getValue(CRef('default', 'Gain1', 'u'))}", flush=True)
print(f"info:    default.Gain1.y: {instantiated_model.getValue(CRef('default', 'Gain1', 'y'))}", flush=True)
print(f"info:    default.Add1.u1: {instantiated_model.getValue(CRef('default', 'Add1', 'u1'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()

## Result:
## info: After instantiation:
## info:    default.Gain1.k: 2.0
## info:    default.Gain1.u: 6.0
## info:    default.Gain1.y: 12.0
## info:    default.Add1.u1: 0.0
## info:    Result file: SimpleSimulation5_res.mat (bufferSize=1)
## info: After simulation:
## info:    default.Gain1.k: 2.0
## info:    default.Gain1.u: 6.0
## info:    default.Gain1.y: 12.0
## info:    default.Add1.u1: 12.0
## endResult
