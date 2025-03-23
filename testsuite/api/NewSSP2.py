## status: correct
## teardown_command: rm -rf NewSSP2.ssp
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

from OMSimulator import SSD, SSP

## This example creates a new SSP file containing a default SSD file.
## It then creates another SSD file and adds it to the SSP file using the SSD constructor, referencing the SSP file directly.
## Finally, the SSP file is exported to a new file and then re-imported.

model = SSP()
variantB = SSD('Variant-B', model=model)
model.activeVariantName = 'Variant-B'
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='Add.fmu')
model.list()
model.export('NewSSP2.ssp')

model2 = SSP('NewSSP2.ssp')
model2.list()

## Result:
## <OMSimulator.ssp.SSP object at 0x7fad0f9303d0>
## |-- Resources:
## |--   resources/Add.fmu
## |-- <OMSimulator.ssd.SSD object at 0x7fad0f931a80>
## |-- Inactive variant "default": None
## |--   System: default
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## |-- <OMSimulator.ssd.SSD object at 0x7fad0cdaac20>
## |-- Active variant "Variant-B": None
## |--   System: Variant-B
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## <OMSimulator.ssp.SSP object at 0x7fad0f931c30>
## |-- Resources:
## |--   resources/Add.fmu
## |-- <OMSimulator.ssd.SSD object at 0x7fad0cdaa8f0>
## |-- Active variant "Variant-B": /tmp/tmpeo_ryrzc/SystemStructure.ssd
## |--   System: Variant-B
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## |-- <OMSimulator.ssd.SSD object at 0x7fad0cdaab60>
## |-- Inactive variant "default": /tmp/tmpeo_ryrzc/default.ssd
## |--   System: default
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## endResult
