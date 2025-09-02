## status: correct
## teardown_command: rm -rf duplicateVariant1.ssp tmp-duplicateVariant1/
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes


from OMSimulator import SSD, SSP, Settings, CRef

Settings.suppressPath = True

# This example creates a new SSP file containing a default SSD file.
# It then duplicates the default SSD file and renames it to "Variant-B".
# The values of the duplicated SSD file are modified, and the original SSD file is not changed.
# The new SSP file is then exported to "duplicateVariant1.ssp".

model = SSP(temp_dir="./tmp-duplicateVariant1/")

# model.activeVariantName = 'Variant-B'
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

model.list()
model.export('duplicateVariant1.ssp')

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Modelica.Blocks.Math.Add.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": None
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
## |-- Variant "Variant-B": None
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
## endResult
