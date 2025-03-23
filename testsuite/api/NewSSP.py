## status: correct
## teardown_command: rm -rf NewSSP.ssp
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

from OMSimulator import SSD, SSP

## This example creates a new SSP file containing a default SSD file.
## It then creates another SSD file and adds it to the SSP file using the add method.
## Finally, the SSP file is exported to a new file and re-imported.

model = SSP()
variantB = SSD('Variant-B')
model.add(variantB)
model.activeVariantName = 'Variant-B'
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='Add.fmu')
model.list()
model.export('NewSSP.ssp')

model2 = SSP('NewSSP.ssp')
model2.list()

## Result:
## <OMSimulator.ssp.SSP object at 0x7fb742f2c3d0>
## |-- Resources:
## |--   resources/Add.fmu
## |-- <OMSimulator.ssd.SSD object at 0x7fb742f2da80>
## |-- Inactive variant "default": None
## |--   System: default
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## |-- <OMSimulator.ssd.SSD object at 0x7fb7403aec20>
## |-- Active variant "Variant-B": None
## |--   System: Variant-B
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## <OMSimulator.ssp.SSP object at 0x7fb742f2dc30>
## |-- Resources:
## |--   resources/Add.fmu
## |-- <OMSimulator.ssd.SSD object at 0x7fb7403ae8f0>
## |-- Active variant "Variant-B": /tmp/tmppx_37k_r/SystemStructure.ssd
## |--   System: Variant-B
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## |-- <OMSimulator.ssd.SSD object at 0x7fb7403aeb60>
## |-- Inactive variant "default": /tmp/tmppx_37k_r/default.ssd
## |--   System: default
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## endResult
