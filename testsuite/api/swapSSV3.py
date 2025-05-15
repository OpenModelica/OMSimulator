## status: correct
## teardown_command: rm -rf swapSSV3.ssp tmp-swapSSV3/ swap5.ssv swap6.ssv
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, SSV, Connector, Causality, SignalType

Settings.suppressPath = True

# This example creates a new SSP file with top level systems and subsystem connectors
# and set parameter values to ssv file and reference them.
# It add test to swap ssv reference from one ssv file to another ssv file

model = SSP(temp_dir="./tmp-swapSSV3/")

## add top level system connector
model.activeVariant.system.addConnector(Connector('param1', Causality.parameter, SignalType.Real))
model.activeVariant.system.addConnector(Connector('input1', Causality.input, SignalType.Real))

model.addSystem(CRef('default', 'sub-system'))
## add top level sub-system connector
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('param1', Causality.parameter, SignalType.Real))
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('input1', Causality.input, SignalType.Real))

ssv1 = SSV()
ssv1.setValue("param1", 2.0)
ssv1.setValue("input1", 3.0)
ssv1.export("swap5.ssv")

ssv2 = SSV()
ssv2.setValue("param1", 200.0)
ssv2.setValue("input1", 300.0)
ssv2.export("swap6.ssv")

## add swap5.ssv to to ssp resources
model.addResource("swap5.ssv", "resources/swap5.ssv")
## reference swap5.ssv to Add1
model.addSSVReference(CRef('default'), 'resources/swap5.ssv')

## add swap6.ssv to to ssp resources
model.addResource("swap6.ssv", "resources/swap6.ssv")
model.addSSVReference(CRef('default', 'sub-system'), 'resources/swap6.ssv')

#model.list()
model.export('swapSSV3.ssp')

model2 = SSP('swapSSV3.ssp')
model2.list()

## swap ssv reference from default to sub-system (swap5.ssv to swap6.ssv)
model2.swapSSVReference(CRef('default'), 'resources/swap5.ssv', 'resources/swap6.ssv')
## swap ssv reference from sub-system to default (swap6.ssv to swap5.ssv)
model2.swapSSVReference(CRef('default', 'sub-system'), 'resources/swap6.ssv', 'resources/swap5.ssv')

print("After swapping swap6.ssv to default and swap5.ssv to sub-system")
print("===============================================================")
model2.list()


## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/swap5.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real param1, 2.0, None, 'None')
## |--   |-- |-- (Real input1, 3.0, None, 'None')
## |--   resources/swap6.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real param1, 200.0, None, 'None')
## |--   |-- |-- (Real input1, 300.0, None, 'None')
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- Parameter Bindings: resources/swap5.ssv
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/swap6.ssv
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## After swapping swap6.ssv to default and swap5.ssv to sub-system
## ===============================================================
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/swap5.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real param1, 2.0, None, 'None')
## |--   |-- |-- (Real input1, 3.0, None, 'None')
## |--   resources/swap6.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real param1, 200.0, None, 'None')
## |--   |-- |-- (Real input1, 300.0, None, 'None')
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- Parameter Bindings: resources/swap6.ssv
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/swap5.ssv
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
