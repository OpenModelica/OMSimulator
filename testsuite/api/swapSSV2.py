## status: correct
## teardown_command: rm -rf swapSSV2.ssp tmp-swapSSV2/ swap3.ssv swap4.ssv
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, SSV, Connector, Causality, SignalType

Settings.suppressPath = True

# This example creates a new SSP file with top level systems and subsystem connectors
# and set parameter values to ssv file and reference them.
# It add test to swap ssv reference from one ssv file to another ssv file

model = SSP(temp_dir="./tmp-swapSSV2/")

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
ssv1.export("swap3.ssv")

ssv2 = SSV()
ssv2.setValue("param1", 200.0)
ssv2.setValue("input1", 300.0)
ssv2.export("swap4.ssv")

## add swap3.ssv to to ssp resources
model.addResource("swap3.ssv", "resources/swap3.ssv")
## reference myfile1.ssv to Add1
model.addSSVReference(CRef('default'), 'resources/swap3.ssv')

## add swap4.ssv to to ssp resources
model.addResource("swap4.ssv", "resources/swap4.ssv")
model.addSSVReference(CRef('default', 'sub-system'), 'resources/swap4.ssv')

#model.list()
model.export('swapSSV2.ssp')

model2 = SSP('swapSSV2.ssp')
model2.list()

## remove ssv reference from Add1
model2.removeSSVReference(CRef('default'), 'resources/swap3.ssv')
model2.removeSSVReference(CRef('default', 'sub-system'), 'resources/swap4.ssv')
print("After removing swap3.ssv from default and swap4.ssv from sub-system")
print("=====================================================================")
model2.list()

model2.addSSVReference(CRef('default'), 'resources/swap4.ssv')
model2.addSSVReference(CRef('default', 'sub-system'), 'resources/swap3.ssv')

print("After adding swap4.ssv to default and swap3.ssv to sub-system")
print("==============================================================")
model2.list()


## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/swap3.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real param1, 2.0, None, 'None')
## |--   |-- |-- (Real input1, 3.0, None, 'None')
## |--   resources/swap4.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real param1, 200.0, None, 'None')
## |--   |-- |-- (Real input1, 300.0, None, 'None')
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- Parameter Bindings: resources/swap3.ssv
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/swap4.ssv
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## After removing swap3.ssv from default and swap4.ssv from sub-system
## =====================================================================
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/swap3.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real param1, 2.0, None, 'None')
## |--   |-- |-- (Real input1, 3.0, None, 'None')
## |--   resources/swap4.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real param1, 200.0, None, 'None')
## |--   |-- |-- (Real input1, 300.0, None, 'None')
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## After adding swap4.ssv to default and swap3.ssv to sub-system
## ==============================================================
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/swap3.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real param1, 2.0, None, 'None')
## |--   |-- |-- (Real input1, 3.0, None, 'None')
## |--   resources/swap4.ssv
## |--   |-- Parameter Bindings:
## |--   |-- |-- (Real param1, 200.0, None, 'None')
## |--   |-- |-- (Real input1, 300.0, None, 'None')
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- Parameter Bindings: resources/swap4.ssv
## |-- |-- |-- Elements:
## |-- |-- |-- |-- System: sub-system 'None'
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (param1, Causality.parameter, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (input1, Causality.input, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- Parameter Bindings: resources/swap3.ssv
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
