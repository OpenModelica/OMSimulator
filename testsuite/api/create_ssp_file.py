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
## <OMSimulator.ssp.SSP object at 0x7f2816003c10>
## |-- Resources:
## |-- <OMSimulator.ssd.SSD object at 0x7f2815f733a0>
## |-- Inactive variant "default": None
## |--   System: default
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## |-- <OMSimulator.ssd.SSD object at 0x7f28133eabc0>
## |-- Active variant "Variant-B": None
## |--   System: Variant-B
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## <OMSimulator.ssp.SSP object at 0x7f2815f73430>
## |-- Resources:
## |-- <OMSimulator.ssd.SSD object at 0x7f28133ea890>
## |-- Active variant "Variant-B": /tmp/tmp2q5b5uxn/SystemStructure.ssd
## |--   System: Variant-B
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## |-- <OMSimulator.ssd.SSD object at 0x7f28133ea770>
## |-- Inactive variant "default": /tmp/tmp2q5b5uxn/default.ssd
## |--   System: default
## |--   Connectors:
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## endResult
