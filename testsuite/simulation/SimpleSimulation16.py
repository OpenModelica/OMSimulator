## status: correct
## teardown_command: rm -rf SimpleSimulation16_res.mat
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Connector, Causality, SignalType

model = SSP()
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
model.addResource('../resources/Modelica.Blocks.Sources.Sine.fmu', new_name='resources/Sine.fmu')

model.addComponent(CRef("default", "Add"), "resources/Add.fmu")
model.addComponent(CRef("default", "Sine"), "resources/Sine.fmu")
model.addConnection(CRef("default", "Sine", "y"), CRef("default", "Add", "u1"))
model.addConnection(CRef("default", "Sine", "y"), CRef("default", "Add", "u2"))

model.addSystem(CRef("default", "sub-system"))
subsystem = model.activeVariant.system.elements[CRef("sub-system")]

model.addComponent(CRef("default", "sub-system", "Add2"), "resources/Add.fmu")

subsystem.addConnector(Connector(CRef("u"), Causality.input, SignalType.Real))
subsystem.addConnection("", "u", "Add2", "u1")
subsystem.addConnection("", "u", "Add2", "u2")

model.addConnection(CRef("default", "Sine", "y"), CRef("default", "sub-system", "u"))

model.list()
print(flush=True)
instantiated = model.instantiate()
instantiated.setResultFile("SimpleSimulation16_res.mat")
instantiated.initialize()
instantiated.simulate()
print(f"default.Sine.y            : {instantiated.getValue(CRef('default', 'Sine', 'y'))}", flush=True)
print(f"default.sub-system.u      : {instantiated.getValue(CRef('default', 'sub-system', 'u'))}", flush=True)
print(f"default.sub-system.Add2.u1: {instantiated.getValue(CRef('default', 'sub-system', 'Add2', 'u1'))}", flush=True)
print(f"default.sub-system.Add2.u2: {instantiated.getValue(CRef('default', 'sub-system', 'Add2', 'u2'))}", flush=True)
instantiated.terminate()
instantiated.delete()


## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |--   resources/Sine.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": None
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: Add 'None'
## |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- FMU: Sine 'None'
## |-- |-- |-- |-- |-- path: resources/Sine.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (amplitude, Causality.parameter, SignalType.Real, None, 'Amplitude of sine wave')
## |-- |-- |-- |-- |-- |-- (f, Causality.parameter, SignalType.Real, Hz, 'Frequency of sine wave')
## |-- |-- |-- |-- |-- |-- (offset, Causality.parameter, SignalType.Real, None, 'Offset of output signal y')
## |-- |-- |-- |-- |-- |-- (phase, Causality.parameter, SignalType.Real, rad, 'Phase of sine wave')
## |-- |-- |-- |-- |-- |-- (startTime, Causality.parameter, SignalType.Real, s, 'Output y = offset for time < startTime')
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Elements:
## |-- |-- |-- |-- |-- |-- FMU: Add2 'None'
## |-- |-- |-- |-- |-- |-- |-- path: resources/Add.fmu
## |-- |-- |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Connections:
## |-- |-- |-- |-- |-- |-- .u -> Add2.u1
## |-- |-- |-- |-- |-- |-- .u -> Add2.u2
## |-- |-- |-- Connections:
## |-- |-- |-- |-- Sine.y -> Add.u1
## |-- |-- |-- |-- Sine.y -> Add.u2
## |-- |-- |-- |-- Sine.y -> sub-system.u
## |-- UnitDefinitions:
## |-- |-- Unit: s
## |-- |-- |-- BaseUnit: s: 1
## |-- |-- Unit: rad
## |-- |-- |-- BaseUnit:
## |-- |-- Unit: Hz
## |-- |-- |-- BaseUnit: s: -1
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
##
## info:    Result file: SimpleSimulation16_res.mat (bufferSize=1)
## default.Sine.y            : -2.4492127076447545e-16
## default.sub-system.u      : -2.4492127076447545e-16
## default.sub-system.Add2.u1: -2.4492127076447545e-16
## default.sub-system.Add2.u2: -2.4492127076447545e-16
## endResult
