## status: correct
## teardown_command: rm -rf duplicateVariant2.ssp tmp-duplicateVariant2/
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes


from OMSimulator import SSD, SSP, Settings, CRef

Settings.suppressPath = True

# This example creates a new SSP file containing a default SSD file.
# It then duplicates the default SSD file and renames it to "Variant-B".
# The values of the duplicated SSD file are modified, and the original SSD file is not changed.
# The new SSP file is then exported to "duplicateVariant2.ssp".

model = SSP(temp_dir="./tmp-duplicateVariant2/")

model.addResource('../resources/Modelica.Blocks.Math.Add.fmu')
model.addComponent(CRef('default', 'Add1'), 'resources/Modelica.Blocks.Math.Add.fmu')
model.setValue(CRef('default', 'Add1', 'u1'), 10.0)
model.setValue(CRef('default', 'Add1', 'u2'), 20.0)
model.setValue(CRef('default', 'Add1', 'k1'), 30.0)


## duplicate variant
variantB = model.variants['default'].duplicate("Variant-B")
## add new variant to model
model.add(variantB)
variantB.setValue(CRef('default', 'Add1', 'u1'), 100.0)
variantB.setValue(CRef('default', 'Add1', 'u2'), 200.0)

model.export('duplicateVariant2.ssp')

model2 = SSP('duplicateVariant2.ssp')

variantC = model2.variants['Variant-B'].duplicate("Variant-C")
model2.add(variantC)
variantC.setValue(CRef('default', 'Add1', 'u1'), -100.0)
## add new component Add2 to variantC
variantC.addComponent(CRef('default', 'Add2'), 'resources/Modelica.Blocks.Math.Add.fmu')

## set active variant to Variant-C and add component Add3 to variantC
model2.activeVariantName = 'Variant-C'
model2.addComponent(CRef('default', 'Add3'), 'resources/Modelica.Blocks.Math.Add.fmu')
variantC.setValue(CRef('default', 'Add3', 'u1'), -200.0)
variantC.setValue(CRef('default', 'Add3', 'k1'), -30.0)

model2.list()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Modelica.Blocks.Math.Add.fmu
## |-- Active Variant: Variant-C
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- path: resources/Modelica.Blocks.Math.Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real u1, 10.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Real u2, 20.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Real k1, 30.0, None, 'None')
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "Variant-B": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- path: resources/Modelica.Blocks.Math.Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real u1, 100.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Real u2, 200.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Real k1, 30.0, None, 'None')
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "Variant-C": None
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: Add1 'None'
## |-- |-- |-- |-- |-- path: resources/Modelica.Blocks.Math.Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real u1, -100.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Real u2, 200.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Real k1, 30.0, None, 'None')
## |-- |-- |-- |-- FMU: Add2 'None'
## |-- |-- |-- |-- |-- path: resources/Modelica.Blocks.Math.Add.fmu
## |-- |-- |-- |-- FMU: Add3 'None'
## |-- |-- |-- |-- |-- path: resources/Modelica.Blocks.Math.Add.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- Inline Parameter Bindings:
## |-- |-- |-- |-- |-- |-- (Real u1, -200.0, None, 'None')
## |-- |-- |-- |-- |-- |-- (Real k1, -30.0, None, 'None')
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
