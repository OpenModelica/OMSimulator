## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: no
## asan: no

from OMSimulator import SSP, CRef, Settings, Capi

Settings.suppressPath = True

Capi.setCommandLineOption('--ignoreUnitMismatch=true')
model = SSP()
model.addResource('../resources/A.fmu', new_name='resources/A.fmu')
model.addResource('../resources/B.fmu', new_name='resources/B.fmu')
model.addComponent(CRef('default', 'A'), 'resources/A.fmu')
model.addComponent(CRef('default', 'B'), 'resources/B.fmu')

## mm -> m/s: incompatible units; their conversion factor 0.001 must not be applied
model.addConnection(CRef('default', 'A', 'y1'), CRef('default', 'B', 'u'))
## mm/s -> m/ms: compatible units, converted with factor 1e-6
model.addConnection(CRef('default', 'A', 'y2'), CRef('default', 'B', 'u2'))
model.export('ignoreUnitMismatch1.ssp')

model2 = SSP('ignoreUnitMismatch1.ssp')
instantiated_model = model2.instantiate()
instantiated_model.setResultFile('ignoreUnitMismatch1_res.mat')
instantiated_model.initialize()
for name in [('A', 'y1'), ('B', 'u'), ('A', 'y2'), ('B', 'u2')]:
  print(f"info:    {'.'.join(name)}: {instantiated_model.getValue(CRef('default', *name))}", flush=True)
instantiated_model.terminate()
instantiated_model.delete()

## Result:
## warning: Unit mismatch in connection: A.y1 [mm] -> B.u [m/s]; ignored due to --ignoreUnitMismatch, the value is passed without unit conversion
## info:    Result file: ignoreUnitMismatch1_res.mat (bufferSize=10)
## info:    A.y1: 5.0
## info:    B.u: 5.0
## info:    A.y2: 5.0
## info:    B.u2: 4.9999999999999996e-06
## info:    1 warnings
## info:    0 errors
## endResult
