## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes


from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True


# This example creates a new SSP file with an FMU instantiated as a component and sets two differents solver for the components and the system.
# It then exports the SSP file and re-imports it to verify the solver settings and the simulates the model.

model = SSP()
model.addResource('../resources/replaceA.fmu', new_name='resources/replaceA.fmu')
model.addResource('../resources/replaceB.fmu', new_name='resources/replaceB.fmu')
model.addResource('../resources/replaceA_extended.fmu', new_name='resources/replaceA_extended.fmu')

model.addComponent(CRef('default', 'A'), 'resources/replaceA.fmu')
model.addComponent(CRef('default', 'B'), 'resources/replaceB.fmu')

model.setValue(CRef('default', 'A', 'u'), 10.0)
model.setValue(CRef('default', 'B', 'u1'), -13.0)
model.setValue(CRef('default', 'A', 't'), -10.0)
model.setValue(CRef('default', 'B', 'z'), -15.0)


model.addConnection(CRef('default', 'A', 'y'), CRef('default', 'B', 'u'))
model.addConnection(CRef('default', 'A', 'dummy'), CRef('default', 'B', 'u1'))

model.export('ReplaceComponent1.ssp')

model2 = SSP('ReplaceComponent1.ssp')
model2.list()

print("Info : After replacement", flush=True)
warnings = model2.replaceComponent(CRef('default', 'A'), 'resources/replaceA_extended.fmu')
model2.list()
for warning in warnings:
  print("Warning :", warning, flush=True)

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/replaceA.fmu
## |--   resources/replaceA_extended.fmu
## |--   resources/replaceB.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: A 'None'
## |-- |-- |-- |-- |-- path: resources/replaceA.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (dummy, Causality.output, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (t, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real u, 10.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Real t, -10.0, None, 'None')
## |-- |-- |-- |-- FMU: B 'None'
## |-- |-- |-- |-- |-- path: resources/replaceB.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (z, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real u1, -13.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Real z, -15.0, None, 'None')
## |-- |-- |-- Connections:
## |-- |-- |-- |-- A.y -> B.u
## |-- |-- |-- |-- A.dummy -> B.u1
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## Info : After replacement
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/replaceA.fmu
## |--   resources/replaceA_extended.fmu
## |--   resources/replaceB.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: A 'None'
## |-- |-- |-- |-- |-- path: resources/replaceA_extended.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (y1, Causality.output, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (foo, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real u, 10.0, None, 'None')
## |-- |-- |-- |-- FMU: B 'None'
## |-- |-- |-- |-- |-- path: resources/replaceB.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (z, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real u1, -13.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Real z, -15.0, None, 'None')
## |-- |-- |-- Connections:
## |-- |-- |-- |-- A.y -> B.u
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## Warning : deleting connection "A.dummy ==> B.u1", as signal "dummy" couldn't be resolved to any signal in the replaced submodel "resources/replaceA_extended.fmu"
## Warning : deleting start value "A.t" in "inline" resources, because the identifier couldn't be resolved to any system signal in the replacing model
## endResult
