## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: no
## the FMUs this test loads leak memory inside their own binaries

from OMSimulator import SSP, CRef, Settings, Connector, Causality, SignalType

Settings.suppressPath = True


# FMUs two levels deep, with the same subsystem and component names in two
# branches, and a component 'other.outer' with the same name as the subsystem 'outer':
#   in1 -> outer.u -> outer.inner.u -> outer.inner.Gain1.u
#   outer.inner.Gain1.y -> outer.inner.y -> outer.y -> out1
#   outer.y -> other.u -> other.inner.u -> other.inner.Gain1.u
#   other.inner.Gain1.y -> other.inner.y -> other.y -> out2

model = SSP()
root = model.activeVariant.system
root.addConnector(Connector('in1', Causality.input, SignalType.Real))
root.addConnector(Connector('out1', Causality.output, SignalType.Real))
root.addConnector(Connector('out2', Causality.output, SignalType.Real))

for path in [('outer',), ('outer', 'inner'), ('other',), ('other', 'inner')]:
  model.addSystem(CRef('default', *path))
  system = root
  for name in path:
    system = system.elements[CRef(name)]
  system.addConnector(Connector('u', Causality.input, SignalType.Real))
  system.addConnector(Connector('y', Causality.output, SignalType.Real))

model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')
model.addComponent(CRef('default', 'outer', 'inner', 'Gain1'), 'resources/Gain.fmu')
model.addComponent(CRef('default', 'other', 'inner', 'Gain1'), 'resources/Gain.fmu')
model.addComponent(CRef('default', 'other', 'outer'), 'resources/Gain.fmu')

model.setValue(CRef('default', 'in1'), 3.0)
model.setValue(CRef('default', 'outer', 'inner', 'Gain1', 'k'), 2.0)
model.setValue(CRef('default', 'other', 'inner', 'Gain1', 'k'), 10.0)

for branch, source, target in [('outer', ('in1',), ('out1',)), ('other', ('outer', 'y'), ('out2',))]:
  model.addConnection(CRef('default', *source), CRef('default', branch, 'u'))
  model.addConnection(CRef('default', branch, 'u'), CRef('default', branch, 'inner', 'u'))
  model.addConnection(CRef('default', branch, 'inner', 'u'), CRef('default', branch, 'inner', 'Gain1', 'u'))
  model.addConnection(CRef('default', branch, 'inner', 'Gain1', 'y'), CRef('default', branch, 'inner', 'y'))
  model.addConnection(CRef('default', branch, 'inner', 'y'), CRef('default', branch, 'y'))
  model.addConnection(CRef('default', branch, 'y'), CRef('default', *target))

model.export('NestedSystemSimulation5.ssp')

model2 = SSP('NestedSystemSimulation5.ssp')
instantiated_model = model2.instantiate()
instantiated_model.setResultFile('NestedSystemSimulation5_res.mat')

signals = [
  ('in1',), ('outer', 'inner', 'Gain1', 'u'), ('outer', 'inner', 'Gain1', 'y'), ('outer', 'y'), ('out1',),
  ('other', 'inner', 'Gain1', 'u'), ('other', 'inner', 'Gain1', 'y'), ('other', 'y'), ('out2',),
  ('other', 'outer', 'k')
]

def printValues(label):
  print(f"info: {label}")
  for signal in signals:
    print(f"info:    default.{'.'.join(signal)}: {instantiated_model.getValue(CRef('default', *signal))}", flush=True)

instantiated_model.initialize()
printValues('After initialization:')

instantiated_model.simulate()
printValues('After simulation:')

instantiated_model.terminate()
instantiated_model.delete()


## Result:
## info:    Result file: NestedSystemSimulation5_res.mat (bufferSize=10)
## info: After initialization:
## info:    default.in1: 3.0
## info:    default.outer.inner.Gain1.u: 3.0
## info:    default.outer.inner.Gain1.y: 6.0
## info:    default.outer.y: 6.0
## info:    default.out1: 6.0
## info:    default.other.inner.Gain1.u: 6.0
## info:    default.other.inner.Gain1.y: 60.0
## info:    default.other.y: 60.0
## info:    default.out2: 60.0
## info:    default.other.outer.k: 1.0
## info: After simulation:
## info:    default.in1: 3.0
## info:    default.outer.inner.Gain1.u: 3.0
## info:    default.outer.inner.Gain1.y: 6.0
## info:    default.outer.y: 6.0
## info:    default.out1: 6.0
## info:    default.other.inner.Gain1.u: 6.0
## info:    default.other.inner.Gain1.y: 60.0
## info:    default.other.y: 60.0
## info:    default.out2: 60.0
## info:    default.other.outer.k: 1.0
## endResult
