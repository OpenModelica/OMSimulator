## status: correct
## teardown_command: rm -rf NewSSP.ssp tmp-NewSSP/
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

import logging
logging.basicConfig(level=logging.INFO)

from OMSimulator import SSD, SSP, Settings

Settings.suppressPath = True

# This example creates a new SSP file containing a default SSD file.
# It then creates another SSD file and adds it to the SSP file using the add method.
# Finally, the SSP file is exported to a new file and re-imported.

model = SSP(temp_dir="./tmp-NewSSP/model1/")
variantB = SSD('Variant-B')
model.add(variantB)
model.activeVariantName = 'Variant-B'
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu')

model.list()
model.export('NewSSP.ssp')

model2 = SSP('NewSSP.ssp', temp_dir="./tmp-NewSSP/model2/")
model2.list()

## Result:
## INFO:OMSimulator.ssp:Temporary directory created: <hidden>
## INFO:OMSimulator.ssd:SSD 'default' exported to <hidden>
## INFO:OMSimulator.ssd:SSD 'Variant-B' exported to <hidden>
## INFO:OMSimulator.ssp:SSP file 'NewSSP.ssp' successfully exported!
## INFO:OMSimulator.ssp:Temporary directory created: <hidden>
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Modelica.Blocks.Math.Add.fmu
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
## |--   resources/Modelica.Blocks.Math.Add.fmu
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
