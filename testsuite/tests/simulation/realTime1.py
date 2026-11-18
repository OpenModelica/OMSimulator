## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: no

import csv

from OMSimulator import SSP, Settings, CRef, Capi

Settings.suppressPath = True
Capi.setCommandLineOption('--realTime=true')
Capi.setCommandLineOption('--wallTime=true')

model = SSP()
model.addResource('../resources/Modelica.Blocks.Sources.Sine.fmu', new_name='resources/Sine.fmu')
model.activeVariant.stopTime = 3.0
model.addComponent(CRef('default', 'Sine'), 'resources/Sine.fmu')
model.export('realTime1.ssp')

instantiated_model = SSP('realTime1.ssp').instantiate()
instantiated_model.setFixedStepSize(1.0)
instantiated_model.setResultFile('realTime1_res.csv')
instantiated_model.initialize()
instantiated_model.simulate()
instantiated_model.terminate()
instantiated_model.delete()

# The wall-clock times vary from run to run, so they are rounded to whole seconds.
# The system's wall-clock time must follow the simulated time.
print('time  model.root.$wallTime  model.root.Sine.$wallTime', flush=True)
with open('realTime1_res.csv') as f:
  for row in csv.DictReader(f):
    print(f"{float(row['time']):4.1f}  {round(float(row['model.root.$wallTime'])):8.1f}  {round(float(row['model.root.Sine.$wallTime'])):8.1f}", flush=True)

## Result:
## info:    Result file: realTime1_res.csv (bufferSize=10)
## time  model.root.$wallTime  model.root.Sine.$wallTime
##  0.0       0.0       0.0
##  1.0       1.0       0.0
##  2.0       2.0       0.0
##  3.0       3.0       0.0
## endResult
