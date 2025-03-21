## status: correct
## teardown_command: rm -rf NewSSP.ssp tmp-create_ssp_file/
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
model.export("NewSSP.ssp")

model2 = SSP("NewSSP.ssp")
model2.list()

## Result:
## List SSP file:
## |-- Variant: default
## |--   System: default
## |--   Connectors:
## |-- Variant: Variant-B (Active)
## |--   System: Variant-B
## |--   Connectors:
## List SSP file:
## |-- Variant: Variant-B (Active)
## |--   System: Variant-B
## |--   Connectors:
## |-- Variant: default
## |--   System: default
## |--   Connectors:
## endResult
