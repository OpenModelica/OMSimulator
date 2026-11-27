## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: yes

from OMSimulator import SSP, CRef, Settings, Connector, Causality, SignalType

Settings.suppressPath = True


# Tables in subsystems connected to top-level outputs:
#   sub.table.y -> sub.y -> out1
#   outer.inner.table.y -> outer.inner.y -> outer.y -> out2

with open('NestedSystemSimulation6.csv', 'w') as file:
  file.write('time,y\n0,0\n1,10\n')

model = SSP()
root = model.activeVariant.system
root.addConnector(Connector('out1', Causality.output, SignalType.Real))
root.addConnector(Connector('out2', Causality.output, SignalType.Real))

for path in [('sub',), ('outer',), ('outer', 'inner')]:
  model.addSystem(CRef('default', *path))
  system = root
  for name in path:
    system = system.elements[CRef(name)]
  system.addConnector(Connector('y', Causality.output, SignalType.Real))

model.addResource('NestedSystemSimulation6.csv', new_name='resources/table.csv')
model.addComponent(CRef('default', 'sub', 'table'), 'resources/table.csv')
model.addComponent(CRef('default', 'outer', 'inner', 'table'), 'resources/table.csv')

model.addConnection(CRef('default', 'sub', 'table', 'y'), CRef('default', 'sub', 'y'))
model.addConnection(CRef('default', 'sub', 'y'), CRef('default', 'out1'))
model.addConnection(CRef('default', 'outer', 'inner', 'table', 'y'), CRef('default', 'outer', 'inner', 'y'))
model.addConnection(CRef('default', 'outer', 'inner', 'y'), CRef('default', 'outer', 'y'))
model.addConnection(CRef('default', 'outer', 'y'), CRef('default', 'out2'))

instantiated_model = model.instantiate()
instantiated_model.setStopTime(1.0)
instantiated_model.setResultFile('NestedSystemSimulation6_res.mat')

def printValues(label):
  print(f"info: {label}")
  for signal in [('out1',), ('out2',)]:
    print(f"info:    default.{'.'.join(signal)}: {instantiated_model.getValue(CRef('default', *signal))}", flush=True)

instantiated_model.initialize()
printValues('After initialization:')

instantiated_model.stepUntil(0.5)
printValues('After stepUntil(0.5):')

instantiated_model.simulate()
printValues('After simulation:')

instantiated_model.terminate()
instantiated_model.delete()


## Result:
## info:    Result file: NestedSystemSimulation6_res.mat (bufferSize=10)
## info: After initialization:
## info:    default.out1: 0.0
## info:    default.out2: 0.0
## info: After stepUntil(0.5):
## info:    default.out1: 5.0000000000000036
## info:    default.out2: 5.0000000000000036
## info: After simulation:
## info:    default.out1: 10.0
## info:    default.out2: 10.0
## endResult
