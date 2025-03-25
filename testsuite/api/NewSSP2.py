## status: correct
## teardown_command: rm NewSSP2.ssp
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

from OMSimulator import SSD, SSP, Settings

Settings.suppressPath = True

# This example creates a new SSP file containing a default SSD file.
# It then creates another SSD file and adds it to the SSP file using the SSD constructor, referencing the SSP file directly.
# Finally, the SSP file is exported to a new file and then re-imported.

model = SSP()
variantB = SSD('Variant-B', model=model)
model.activeVariantName = 'Variant-B'
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='Add.fmu')

model.list()
model.export('NewSSP2.ssp')

model2 = SSP('NewSSP2.ssp')
model2.list()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   Add.fmu
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
## |--   resources/Add.fmu
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
