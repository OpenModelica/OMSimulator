## status: correct
## teardown_command: rm -rf create_ssp_file.ssp tmp-create_ssp_file/
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

#import logging
#logging.basicConfig(level=logging.DEBUG)

from OMSimulator import SSP, SSD

model = SSP(temp_dir="./tmp-create_ssp_file")
variantB = SSD('Variant-B')
model.add(variantB)
model.activeVariantName = 'Variant-B'
model.list()
model.export("create_ssp_file.ssp")

model2 = SSP("create_ssp_file.ssp")
model2.list()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
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
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "Variant-B": /tmp/tmpvoval71m/SystemStructure.ssd
## |--   System: Variant-B
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Inactive variant "default": /tmp/tmpvoval71m/default.ssd
## |--   System: default
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## endResult
