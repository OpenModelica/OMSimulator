## status: correct
## teardown_command: rm -rf NewSSP3.ssp
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef

# This example creates a new SSP file with an FMU instantiated as a component.

model = SSP()

model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='Add.fmu')
model.addComponent(CRef('default', 'Add'), 'Add.fmu')

model.list()
model.export('NewSSP3.ssp')

model2 = SSP('NewSSP3.ssp')
model2.list()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Add.fmu
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": None
## |--   System: default
## |--   Connectors:
## |--   Components:
## {'Add': 'Add.fmu'}
## Add Add.fmu
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## Traceback (most recent call last):
##   File "OMSimulator/testsuite/api/NewSSP3.py", line 19, in <module>
##     model.export('NewSSP3.ssp')
##   File "OMSimulator/install/lib/x86_64-linux-gnu/OMSimulator/ssp.py", line 164, in export
##     ssd.export(ssd_file_path)
##   File "OMSimulator/install/lib/x86_64-linux-gnu/OMSimulator/ssd.py", line 92, in export
##     self.system.export(root)
##   File "OMSimulator/install/lib/x86_64-linux-gnu/OMSimulator/system.py", line 110, in export
##     raise Exception("Not implemented - need to lookup fmu in ssp resources")
## Exception: Not implemented - need to lookup fmu in ssp resources
## endResult
