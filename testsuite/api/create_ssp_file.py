## status: correct
## teardown_command: rm create_ssp_file.ssp
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

#import logging
#logging.basicConfig(level=logging.DEBUG)

from OMSimulator import SSP, SSD, Settings

Settings.suppressPath = True

model = SSP()
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
## |-- Inactive variant "default": <hidden>
## |--   System: default
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "Variant-B": <hidden>
## |--   System: Variant-B
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "Variant-B": <hidden>
## |--   System: Variant-B
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Inactive variant "default": <hidden>
## |--   System: default
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## endResult
