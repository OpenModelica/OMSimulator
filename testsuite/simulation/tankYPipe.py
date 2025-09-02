## status: correct
## teardown_command: rm -rf tankYpipe.ssp tankYpipe_rest.mat
## linux: no
## ucrt64: yes
## win: yes
## mac: no

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True


# This example creates a new SSP file with an FMU instantiated as a component and sets two differents solver for the components and the system.
# It then exports the SSP file and re-imports it to verify the solver settings.
model = SSP()

model.addResource("../resources/tank1.fmu", new_name="resources/tank1.fmu")
model.addResource("../resources/tank2.fmu", new_name="resources/tank2.fmu")
model.addResource("../resources/tank3.fmu", new_name="resources/tank3.fmu")
model.addResource("../resources/ypipe.fmu", new_name="resources/ypipe.fmu")


component1 = model.addComponent(CRef('default', 'tank1'), 'resources/tank1.fmu')
component2 = model.addComponent(CRef('default', 'tank2'), 'resources/tank2.fmu')
component3 = model.addComponent(CRef('default', 'tank3'), 'resources/tank3.fmu')
component4 = model.addComponent(CRef('default', 'ypipe'), 'resources/ypipe.fmu')

model.addConnection(CRef('default', 'tank1', 'outputRate'), CRef('default', 'ypipe', 'inputRate1'))
model.addConnection(CRef('default', 'tank2', 'outputRate'), CRef('default', 'ypipe', 'inputRate2'))
model.addConnection(CRef('default', 'ypipe', 'outputRate'), CRef('default', 'tank3', 'inputRate'))

model.export('tankYPipe.ssp')

model2 = SSP('tankYPipe.ssp')

model2.list()
print("", flush=True)
instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,
instantiated_model.setResultFile("tankYpipe_rest.mat")
instantiated_model.initialize()
instantiated_model.simulate()
instantiated_model.terminate()
instantiated_model.delete()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/tank1.fmu
## |--   resources/tank2.fmu
## |--   resources/tank3.fmu
## |--   resources/ypipe.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: tank1 'None'
## |-- |-- |-- |-- |-- path: resources/tank1.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (inputRate, Causality.input, SignalType.Real, None, 'input flow rate (m^3/s)')
## |-- |-- |-- |-- |-- |-- (level, Causality.output, SignalType.Real, None, 'water level in tank (m)')
## |-- |-- |-- |-- |-- |-- (outputRate, Causality.output, SignalType.Real, None, 'output flow rate (m^3/s)')
## |-- |-- |-- |-- |-- |-- (holeSurface, Causality.parameter, SignalType.Real, None, 'area of hole at bottom of tank (m^2)')
## |-- |-- |-- |-- |-- |-- (initialLevel, Causality.parameter, SignalType.Real, None, 'initial water level (m)')
## |-- |-- |-- |-- |-- |-- (tankSurface, Causality.parameter, SignalType.Real, None, 'tank area (m^2)')
## |-- |-- |-- |-- FMU: tank2 'None'
## |-- |-- |-- |-- |-- path: resources/tank2.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (inputRate, Causality.input, SignalType.Real, None, 'input flow rate (m^3/s)')
## |-- |-- |-- |-- |-- |-- (level, Causality.output, SignalType.Real, None, 'water level in tank (m)')
## |-- |-- |-- |-- |-- |-- (outputRate, Causality.output, SignalType.Real, None, 'output flow rate (m^3/s)')
## |-- |-- |-- |-- |-- |-- (holeSurface, Causality.parameter, SignalType.Real, None, 'area of hole at bottom of tank (m^2)')
## |-- |-- |-- |-- |-- |-- (initialLevel, Causality.parameter, SignalType.Real, None, 'initial water level (m)')
## |-- |-- |-- |-- |-- |-- (tankSurface, Causality.parameter, SignalType.Real, None, 'tank area (m^2)')
## |-- |-- |-- |-- FMU: tank3 'None'
## |-- |-- |-- |-- |-- path: resources/tank3.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (inputRate, Causality.input, SignalType.Real, None, 'input flow rate (m^3/s)')
## |-- |-- |-- |-- |-- |-- (level, Causality.output, SignalType.Real, None, 'water level in tank (m)')
## |-- |-- |-- |-- |-- |-- (outputRate, Causality.output, SignalType.Real, None, 'output flow rate (m^3/s)')
## |-- |-- |-- |-- |-- |-- (holeSurface, Causality.parameter, SignalType.Real, None, 'area of hole at bottom of tank (m^2)')
## |-- |-- |-- |-- |-- |-- (initialLevel, Causality.parameter, SignalType.Real, None, 'initial water level (m)')
## |-- |-- |-- |-- |-- |-- (tankSurface, Causality.parameter, SignalType.Real, None, 'tank area (m^2)')
## |-- |-- |-- |-- FMU: ypipe 'None'
## |-- |-- |-- |-- |-- path: resources/ypipe.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (inputRate1, Causality.input, SignalType.Real, None, 'input flow rate #1 (m^3/s)')
## |-- |-- |-- |-- |-- |-- (inputRate2, Causality.input, SignalType.Real, None, 'input flow rate #2 (m^3/s)')
## |-- |-- |-- |-- |-- |-- (outputRate, Causality.output, SignalType.Real, None, 'output flow rate (m^3/s)')
## |-- |-- |-- Connections:
## |-- |-- |-- |-- tank1.outputRate -> ypipe.inputRate1
## |-- |-- |-- |-- tank2.outputRate -> ypipe.inputRate2
## |-- |-- |-- |-- ypipe.outputRate -> tank3.inputRate
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
##
## info:    Result file: tankYpipe_rest.mat (bufferSize=1)
## endResult
