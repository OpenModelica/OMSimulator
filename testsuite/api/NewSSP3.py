## status: correct
## teardown_command: rm -rf NewSSP3.ssp tmp-NewSSP3/
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a component.

model = SSP(temp_dir="./tmp-NewSSP3/")
model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='Add.fmu')
component1 = model.addComponent(CRef('default', 'Add1'), 'Add.fmu')
component2 = model.addComponent(CRef('default', 'Add2'), 'Add.fmu')

model.list()
model.export('NewSSP3.ssp')

# model2 = SSP('NewSSP3.ssp', temp_dir="./tmp-NewSSP3/")
# model2.list()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   Add.fmu
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Active variant "default": <hidden>
## |--   System: default
## |--   Connectors:
## |--   Components:
## |     |  ├── FMU: (Add1)
## |     |  |   ├── path: Add.fmu
## |     |  |   ├── Connectors:
## |     |  |   |   * (u1, input, Real)
## |     |  |   |   * (u2, input, Real)
## |     |  |   |   * (y, output, Real)
## |     |  |   |   * (k1, parameter, Real)
## |     |  |   |   * (k2, parameter, Real)
## |     |  ├── FMU: (Add2)
## |     |  |   ├── path: Add.fmu
## |     |  |   ├── Connectors:
## |     |  |   |   * (u1, input, Real)
## |     |  |   |   * (u2, input, Real)
## |     |  |   |   * (y, output, Real)
## |     |  |   |   * (k1, parameter, Real)
## |     |  |   |   * (k2, parameter, Real)
## |--   DefaultExperiment
## |--     startTime: 0.0
## |--     stopTime: 1.0
## endResult
