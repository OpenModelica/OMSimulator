## status: correct
## teardown_command: rm create_ssp_file.ssp
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

import logging
logging.basicConfig(level=logging.INFO)

from OMSimulator import SSD, SSP, Settings

Settings.suppressPath = True

model = SSP()
variantB = SSD('Variant-B')
model.add(variantB)
model.activeVariantName = 'Variant-B'
model.list()
model.export('create_ssp_file.ssp')

model2 = SSP('create_ssp_file.ssp')
model2.list()

## Result:
## INFO:OMSimulator.ssp:Temporary directory created: <hidden>
## INFO:OMSimulator.ssd:SSD 'default' exported to <hidden>
## INFO:OMSimulator.ssd:SSD 'Variant-B' exported to <hidden>
## INFO:OMSimulator.ssp:SSP file 'create_ssp_file.ssp' successfully exported!
## INFO:OMSimulator.ssp:Temporary directory created: <hidden>
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Inactive variant "default": None
## |-- |-- System: default
## |-- |-- Connectors:
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "Variant-B": None
## |-- |-- System: Variant-B
## |-- |-- Connectors:
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "Variant-B": <hidden>
## |-- |-- System: Variant-B
## |-- |-- Connectors:
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Inactive variant "default": <hidden>
## |-- |-- System: default
## |-- |-- Connectors:
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## INFO:OMSimulator.ssp:Temporary directory removed: <hidden>
## INFO:OMSimulator.ssp:Temporary directory removed: <hidden>
## endResult
