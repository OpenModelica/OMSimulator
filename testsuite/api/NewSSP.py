## status: correct
## teardown_command: rm -rf NewSSP.ssp
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

from OMSimulator import SSD, SSP

# This example creates a new SSP file containing a default SSD file.
# It then creates another SSD file and adds it to the SSP file using the add method.
# Finally, the SSP file is exported to a new file and re-imported.

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
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Inactive variant "default": None
## |--   System: default
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "Variant-B": None
## |--   System: Variant-B
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "Variant-B": /tmp/tmp4j1cxqzj/SystemStructure.ssd
## |--   System: Variant-B
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Inactive variant "default": /tmp/tmp4j1cxqzj/default.ssd
## |--   System: default
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## endResult
