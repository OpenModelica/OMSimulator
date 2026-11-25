## status: correct
## linux: no
## ucrt64: yes
## win: yes
## mac: no

from OMSimulator import SSP, Settings, CRef

Settings.suppressPath = True

model = SSP()
model.addResource('../resources/Modelica.Electrical.Analog.Examples.CauerLowPassAnalog.fmu', new_name='resources/CauerLowPassAnalog.fmu')

component1 = model.addComponent(CRef('default', 'CauerLowPassAnalog'), 'resources/CauerLowPassAnalog.fmu')

model.export('SimpleSimulation4.ssp')

model2 = SSP('SimpleSimulation4.ssp')

print(f"info: Before instantiation:")
print(f"info:    default.CauerLowPassAnalog.R1.T: {model.getValue(CRef('default', 'CauerLowPassAnalog', 'R1.T'))}", flush=True)
print(f"info:    default.CauerLowPassAnalog.C1.C: {model.getValue(CRef('default', 'CauerLowPassAnalog', 'C1.C'))}", flush=True)

instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,
instantiated_model.setResultFile("SimpleSimulation4_res.mat")

print(f"info: After instantiation:")
print(f"info:    default.CauerLowPassAnalog.R1.T: {instantiated_model.getValue(CRef('default', 'CauerLowPassAnalog', 'R1.T'))}", flush=True)
print(f"info:    default.CauerLowPassAnalog.C1.C: {instantiated_model.getValue(CRef('default', 'CauerLowPassAnalog', 'C1.C'))}", flush=True)

instantiated_model.initialize()

print(f"info: After initialization:")
print(f"info:    default.CauerLowPassAnalog.R1.T: {instantiated_model.getValue(CRef('default', 'CauerLowPassAnalog', 'R1.T'))}", flush=True)
print(f"info:    default.CauerLowPassAnalog.C1.C: {instantiated_model.getValue(CRef('default', 'CauerLowPassAnalog', 'C1.C'))}", flush=True)

instantiated_model.simulate()
print(f"info: After Simulation:")
print(f"info:    default.CauerLowPassAnalog.R1.T: {instantiated_model.getValue(CRef('default', 'CauerLowPassAnalog', 'R1.T'))}", flush=True)
print(f"info:    default.CauerLowPassAnalog.C1.C: {instantiated_model.getValue(CRef('default', 'CauerLowPassAnalog', 'C1.C'))}", flush=True)

instantiated_model.terminate()
instantiated_model.delete()

## Result:
## info: Before instantiation:
## info:    default.CauerLowPassAnalog.R1.T: None
## info:    default.CauerLowPassAnalog.C1.C: None
## warning: no start value available for signal model.root.CauerLowPassAnalog.R1.T before initialization, returning 0; call getValue after initialize() to get the calculated value
## info: After instantiation:
## info:    default.CauerLowPassAnalog.R1.T: 0.0
## warning: no start value available for signal model.root.CauerLowPassAnalog.C1.C before initialization, returning 0; call getValue after initialize() to get the calculated value
## info:    default.CauerLowPassAnalog.C1.C: 0.0
## info:    Result file: SimpleSimulation4_res.mat (bufferSize=10)
## info: After initialization:
## info:    default.CauerLowPassAnalog.R1.T: 300.15
## info:    default.CauerLowPassAnalog.C1.C: 1.072
## info: After Simulation:
## info:    default.CauerLowPassAnalog.R1.T: 300.15
## info:    default.CauerLowPassAnalog.C1.C: 1.072
## info:    2 warnings
## info:    0 errors
## endResult
