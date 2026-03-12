## status: correct
## teardown_command: rm -rf FMI3_SimpleSimulation1.ssp FMI3_SimpleSimulation1_res.mat
## linux: no
## ucrt64: yes
## win: yes
## mac: no

from OMSimulator import SSP, Settings, CRef

Settings.suppressPath = True


model = SSP()
model.addResource('../resources/BouncingBall3.fmu', new_name='resources/BouncingBall3.fmu')
model.addComponent(CRef('default', 'BouncingBall'), 'resources/BouncingBall3.fmu')

#model.list()
model.export('FMI3_SimpleSimulation1.ssp')

model2 = SSP('FMI3_SimpleSimulation1.ssp')

model2.list()
print("", flush=True)
instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,
instantiated_model.setResultFile("FMI3_SimpleSimulation1_res.mat")
instantiated_model.initialize()
instantiated_model.simulate()
print(f"info: After simulation:")
print(f"info:    default.BouncingBall.g: {instantiated_model.getValue(CRef('default', 'BouncingBall', 'g'))}", flush=True)
print(f"info:    default.BouncingBall.e: {instantiated_model.getValue(CRef('default', 'BouncingBall', 'e'))}", flush=True)
instantiated_model.terminate()
instantiated_model.delete()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/BouncingBall3.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: BouncingBall 'None'
## |-- |-- |-- |-- |-- path: resources/BouncingBall3.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (h, Causality.output, SignalType.Float64, None, 'Position of the ball')
## |-- |-- |-- |-- |-- |-- (v, Causality.output, SignalType.Float64, None, 'Velocity of the ball')
## |-- |-- |-- |-- |-- |-- (g, Causality.parameter, SignalType.Float64, None, 'Gravity acting on the ball')
## |-- |-- |-- |-- |-- |-- (e, Causality.parameter, SignalType.Float64, None, 'Coefficient of restitution')
## |-- UnitDefinitions:
## |-- |-- Unit: m
## |-- |-- |-- BaseUnit: m: 1
## |-- |-- Unit: m/s
## |-- |-- |-- BaseUnit: m: 1, s: -1
## |-- |-- Unit: m/s2
## |-- |-- |-- BaseUnit: m: 1, s: -2
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
##
## Loading FMI version 3...
## info:    Result file: FMI3_SimpleSimulation1_res.mat (bufferSize=1)
## info: After simulation:
## info:    default.BouncingBall.g: -9.81
## info:    default.BouncingBall.e: 0.7
## endResult
