## status: correct
## teardown_command: rm -rf NewSSP.ssp
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, SSD

model = SSP()
variantB = SSD('Variant-B')
model.add(variantB)
model.activeVariantName = 'Variant-B'
model.list()
model.export("NewSSP.ssp")

model2 = SSP("NewSSP.ssp")
model2.list()

## Result:
## DEBUG Temp:  /tmp/tmp_waudok5
## DEBUG: Active variant set to 'default'
## ├── Variant: default
## ├──   System: default
## ├──   Connectors:
## ├── Variant: Variant-B (Active)
## ├──   System: Variant-B
## ├──   Connectors:
## SSP file 'NewSSP.ssp' successfully exported!
## DEBUG Temp:  /tmp/tmpsibw90t4
## DEBUG UNZIP:  NewSSP.ssp /tmp/tmpsibw90t4
## DEBUG ssd_files:  ['SystemStructure.ssd', 'default.ssd']
## DEBUG resources:  []
## DEBUG: Active variant set to 'Variant-B'
## ├── Variant: Variant-B (Active)
## ├──   System: Variant-B
## ├──   Connectors:
## ├── Variant: default
## ├──   System: default
## ├──   Connectors:
## DEBUG Temp Removed:  /tmp/tmp_waudok5
## DEBUG Temp Removed:  /tmp/tmpsibw90t4
## endResult
