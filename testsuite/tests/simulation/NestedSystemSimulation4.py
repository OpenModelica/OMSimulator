## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: no
## the FMUs this test loads leak memory inside their own binaries

from OMSimulator import SSP, CRef, Settings, Connector, Causality, SignalType

Settings.suppressPath = True


# Subsystem outputs connected to top-level outputs, directly and through
# another subsystem:
#   in1 -> sub1.u -> sub1.Gain1.u, sub1.Gain1.y -> sub1.y -> out1
#   sub2.Sine1.y -> sub2.y -> out2
#   sub2.y -> sub3.u -> sub3.Gain2.u, sub3.Gain2.y -> sub3.y -> out3

model = SSP()
root = model.activeVariant.system
root.addConnector(Connector('in1', Causality.input, SignalType.Real))
root.addConnector(Connector('out1', Causality.output, SignalType.Real))
root.addConnector(Connector('out2', Causality.output, SignalType.Real))
root.addConnector(Connector('out3', Causality.output, SignalType.Real))

model.addSystem(CRef('default', 'sub1'))
model.addSystem(CRef('default', 'sub2'))
model.addSystem(CRef('default', 'sub3'))
root.elements[CRef('sub1')].addConnector(Connector('u', Causality.input, SignalType.Real))
root.elements[CRef('sub1')].addConnector(Connector('y', Causality.output, SignalType.Real))
root.elements[CRef('sub2')].addConnector(Connector('y', Causality.output, SignalType.Real))
root.elements[CRef('sub3')].addConnector(Connector('u', Causality.input, SignalType.Real))
root.elements[CRef('sub3')].addConnector(Connector('y', Causality.output, SignalType.Real))

model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')
model.addResource('../resources/Modelica.Blocks.Sources.Sine.fmu', new_name='resources/Sine.fmu')

model.addComponent(CRef('default', 'sub1', 'Gain1'), 'resources/Gain.fmu')
model.addComponent(CRef('default', 'sub2', 'Sine1'), 'resources/Sine.fmu')
model.addComponent(CRef('default', 'sub3', 'Gain2'), 'resources/Gain.fmu')

model.setValue(CRef('default', 'in1'), 3.0)
model.setValue(CRef('default', 'sub1', 'Gain1', 'k'), 2.0)
model.setValue(CRef('default', 'sub3', 'Gain2', 'k'), 10.0)

model.addConnection(CRef('default', 'in1'), CRef('default', 'sub1', 'u'))
model.addConnection(CRef('default', 'sub1', 'u'), CRef('default', 'sub1', 'Gain1', 'u'))
model.addConnection(CRef('default', 'sub1', 'Gain1', 'y'), CRef('default', 'sub1', 'y'))
model.addConnection(CRef('default', 'sub1', 'y'), CRef('default', 'out1'))

model.addConnection(CRef('default', 'sub2', 'Sine1', 'y'), CRef('default', 'sub2', 'y'))
model.addConnection(CRef('default', 'sub2', 'y'), CRef('default', 'out2'))

model.addConnection(CRef('default', 'sub2', 'y'), CRef('default', 'sub3', 'u'))
model.addConnection(CRef('default', 'sub3', 'u'), CRef('default', 'sub3', 'Gain2', 'u'))
model.addConnection(CRef('default', 'sub3', 'Gain2', 'y'), CRef('default', 'sub3', 'y'))
model.addConnection(CRef('default', 'sub3', 'y'), CRef('default', 'out3'))

model.export('NestedSystemSimulation4.ssp')

model2 = SSP('NestedSystemSimulation4.ssp')
instantiated_model = model2.instantiate()
instantiated_model.setResultFile('NestedSystemSimulation4_res.mat')

signals = [
  ('in1',), ('sub1', 'Gain1', 'y'), ('sub1', 'y'), ('out1',),
  ('sub2', 'Sine1', 'y'), ('sub2', 'y'), ('out2',),
  ('sub3', 'Gain2', 'u'), ('sub3', 'Gain2', 'y'), ('out3',)
]

def printValues(label):
  print(f"info: {label}")
  for signal in signals:
    print(f"info:    default.{'.'.join(signal)}: {instantiated_model.getValue(CRef('default', *signal))}", flush=True)

instantiated_model.initialize()
printValues('After initialization:')

instantiated_model.stepUntil(0.25)
printValues('At time 0.25:')

instantiated_model.terminate()
instantiated_model.delete()


## Result:
## info:    Result file: NestedSystemSimulation4_res.mat (bufferSize=10)
## info: After initialization:
## info:    default.in1: 3.0
## info:    default.sub1.Gain1.y: 6.0
## info:    default.sub1.y: 6.0
## info:    default.out1: 6.0
## info:    default.sub2.Sine1.y: 0.0
## info:    default.sub2.y: 0.0
## info:    default.out2: 0.0
## info:    default.sub3.Gain2.u: 0.0
## info:    default.sub3.Gain2.y: 0.0
## info:    default.out3: 0.0
## info: At time 0.25:
## info:    default.in1: 3.0
## info:    default.sub1.Gain1.y: 6.0
## info:    default.sub1.y: 6.0
## info:    default.out1: 6.0
## info:    default.sub2.Sine1.y: 1.0
## info:    default.sub2.y: 1.0
## info:    default.out2: 1.0
## info:    default.sub3.Gain2.u: 1.0
## info:    default.sub3.Gain2.y: 10.0
## info:    default.out3: 10.0
## endResult
