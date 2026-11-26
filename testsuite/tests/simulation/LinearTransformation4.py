## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: no
## the FMUs this test loads leak memory inside their own binaries

from OMSimulator import SSP, CRef, Settings, Connector, Causality, SignalType
from OMSimulator.connection import LinearTransformation

Settings.suppressPath = True


# LinearTransformation on connections between system connectors that start at
# a top-level input:
#   in1 -[2*x+1]-> sub.u -> sub.Gain1.u
#   in1 -[2*x]-> outer.u -[10*x+1]-> outer.inner.u -> outer.inner.Gain2.u
#   in1 -[-x]-> out1

model = SSP()
root = model.activeVariant.system
root.addConnector(Connector('in1', Causality.input, SignalType.Real))
root.addConnector(Connector('out1', Causality.output, SignalType.Real))

for path in [('sub',), ('outer',), ('outer', 'inner')]:
  model.addSystem(CRef('default', *path))
  system = root
  for name in path:
    system = system.elements[CRef(name)]
  system.addConnector(Connector('u', Causality.input, SignalType.Real))

model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')
model.addComponent(CRef('default', 'sub', 'Gain1'), 'resources/Gain.fmu')
model.addComponent(CRef('default', 'outer', 'inner', 'Gain2'), 'resources/Gain.fmu')
model.setValue(CRef('default', 'in1'), 3.0)

def addConnection(system, start, end, factor=None, offset=None):
  model.addConnection(CRef('default', *start), CRef('default', *end))
  if factor is not None:
    system.connections[-1].linearTransformation = LinearTransformation(factor, offset)

outer = root.elements[CRef('outer')]
addConnection(root, ('in1',), ('sub', 'u'), 2.0, 1.0)
addConnection(root, ('sub', 'u'), ('sub', 'Gain1', 'u'))
addConnection(root, ('in1',), ('outer', 'u'), 2.0, 0.0)
addConnection(outer, ('outer', 'u'), ('outer', 'inner', 'u'), 10.0, 1.0)
addConnection(root, ('outer', 'inner', 'u'), ('outer', 'inner', 'Gain2', 'u'))
addConnection(root, ('in1',), ('out1',), -1.0, 0.0)

model.export('LinearTransformation4.ssp')

model2 = SSP('LinearTransformation4.ssp')
instantiated_model = model2.instantiate()
instantiated_model.setResultFile('LinearTransformation4_res.mat')

signals = [('in1',), ('sub', 'Gain1', 'u'), ('outer', 'u'), ('outer', 'inner', 'Gain2', 'u'), ('out1',)]

def printValues(label):
  print(f"info: {label}")
  for signal in signals:
    print(f"info:    default.{'.'.join(signal)}: {instantiated_model.getValue(CRef('default', *signal))}", flush=True)

instantiated_model.initialize()
printValues('After initialization:')

instantiated_model.setValue(CRef('default', 'in1'), 5.0)
instantiated_model.simulate()
printValues('After setValue(in1, 5.0) and simulation:')

instantiated_model.terminate()
instantiated_model.delete()


## Result:
## info:    Result file: LinearTransformation4_res.mat (bufferSize=10)
## info: After initialization:
## info:    default.in1: 3.0
## info:    default.sub.Gain1.u: 7.0
## info:    default.outer.u: 6.0
## info:    default.outer.inner.Gain2.u: 61.0
## info:    default.out1: -3.0
## info: After setValue(in1, 5.0) and simulation:
## info:    default.in1: 5.0
## info:    default.sub.Gain1.u: 11.0
## info:    default.outer.u: 10.0
## info:    default.outer.inner.Gain2.u: 101.0
## info:    default.out1: -5.0
## endResult
