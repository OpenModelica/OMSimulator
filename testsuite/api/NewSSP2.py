## status: correct
## teardown_command: rm NewSSP2.ssp
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

import logging
logging.basicConfig(level=logging.INFO)

from OMSimulator import SSD, SSP, Settings

Settings.suppressPath = True

# This example creates a new SSP file containing a default SSD file.
# It then creates another SSD file and adds it to the SSP file using the SSD constructor, referencing the SSP file directly.
# Finally, the SSP file is exported to a new file and then re-imported.

model = SSP()
variantB = SSD('Variant-B')
model.add(variantB)
model.activeVariantName = 'Variant-B'
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')

model.list()
model.export('NewSSP2.ssp')

model2 = SSP('NewSSP2.ssp')
model2.list()

## Result:
## INFO:OMSimulator.ssp:Temporary directory created: <hidden>
## INFO:OMSimulator.ssd:SSD 'default' exported to <hidden>
## INFO:OMSimulator.ssd:SSD 'Variant-B' exported to <hidden>
## INFO:OMSimulator.ssp:SSP file 'NewSSP2.ssp' successfully exported!
## INFO:OMSimulator.ssp:Temporary directory created: <hidden>
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |-- Active Variant: Variant-B
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": None
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "Variant-B": None
## |-- |-- System: Variant-B 'None'
## |-- |-- |-- Connectors:
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |-- Active Variant: Variant-B
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "Variant-B": <hidden>
## |-- |-- System: Variant-B 'None'
## |-- |-- |-- Connectors:
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## INFO:OMSimulator.ssp:Temporary directory removed: <hidden>
## INFO:OMSimulator.ssp:Temporary directory removed: <hidden>
## endResult
