## status: correct
## teardown_command: rm -rf DualMassOscillator.ssp DualMassOscillator_res.mat
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

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

#model.list()
model.export('DualMassOscillator.ssp')

model2 = SSP('DualMassOscillator.ssp')

instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,
instantiated_model.setResultFile("DualMassOscillator_res.mat")
instantiated_model.setStopTime(10.0)

instantiated_model.setVariableStepSize(1e-12, 1e-12, 1e-3)

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
## info:    Result file: DualMassOscillator_res.mat (bufferSize=1)
## info:    Initialization
## info:      default.system1.x1: 0.0
## info:      default.system2.x2: 0.5
## info:    Simulation
## info:      default.system1.x1: 0.05488241653128615
## info:      default.system2.x2: 0.03387446003048107
## endResult
