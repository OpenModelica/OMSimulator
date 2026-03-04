## status: correct
## teardown_command: rm -rf LinearTransformation3_res.mat
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, Connector, Causality, SignalType, SSV, SSM

Settings.suppressPath = True


## this example shows how to use linear transformation in parameter mapping

model2 = SSP('../resources/LinearTransformation3.ssp')
model2.list()
instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,
instantiated_model.setResultFile("LinearTransformation3_res.mat")

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
## info:    default.Gain1.k: 13.0
## info:    default.Gain1.u: 14.0
## info:    default.Gain1.y: 182.0
## info:    default.Add1.u1: 0.0
## info:    Result file: LinearTransformation3_res.mat (bufferSize=1)
## info: After simulation:
## info:    default.Gain1.k: 13.0
## info:    default.Gain1.u: 14.0
## info:    default.Gain1.y: 182.0
## info:    default.Add1.u1: 182.0
## endResult
