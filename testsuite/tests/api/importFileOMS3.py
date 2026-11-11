## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: no
## asan: no

# SSP files exported with the Python API are imported with the C API
# oms_importFile and simulated, as a C/C++ host application does it.

import ctypes
from OMSimulator import SSP, CRef, Settings, Capi

Settings.suppressPath = True
Capi.setSuppressPath()

lib = Capi.obj
lib.oms_importFile.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p)]
lib.oms_getReal.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_double)]
lib.oms_setResultFile.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int]
for f in ('oms_instantiate', 'oms_initialize', 'oms_simulate', 'oms_terminate', 'oms_delete'):
  getattr(lib, f).argtypes = [ctypes.c_char_p]

def export(filename, system, solver):
  model = SSP()
  model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')
  if system:
    model.addSystem(CRef('default', system))
  prefix = ['default'] + ([system] if system else [])
  model.addComponent(CRef(*prefix, 'Gain1'), 'resources/Gain.fmu')
  model.addComponent(CRef(*prefix, 'Gain2'), 'resources/Gain.fmu')
  model.addConnection(CRef(*prefix, 'Gain1', 'y'), CRef(*prefix, 'Gain2', 'u'))
  model.setValue(CRef(*prefix, 'Gain1', 'u'), 3.0)
  model.setValue(CRef(*prefix, 'Gain1', 'k'), 2.0)
  if solver:
    model.newSolver({'name': 'solver1', 'method': 'euler', 'stepSize': 0.005})
    model.setSolver(CRef(*prefix, 'Gain1'), 'solver1')
    model.setSolver(CRef(*prefix, 'Gain2'), 'solver1')
  model.export(filename)

def importAndSimulate(filename, signal):
  cref = ctypes.c_char_p()
  status = lib.oms_importFile(filename.encode(), ctypes.byref(cref))
  print(f"info:    oms_importFile: {status}", flush=True)
  model = cref.value
  lib.oms_setResultFile(model, (filename + '_res.mat').encode(), 1)
  for f in ('oms_instantiate', 'oms_initialize', 'oms_simulate'):
    print(f"info:    {f}: {getattr(lib, f)(model)}", flush=True)
  value = ctypes.c_double()
  lib.oms_getReal(model + b'.' + signal.encode(), ctypes.byref(value))
  print(f"info:    {signal}: {value.value}", flush=True)
  for f in ('oms_terminate', 'oms_delete'):
    print(f"info:    {f}: {getattr(lib, f)(model)}", flush=True)

## root system without solver settings: weakly coupled
export('importFileOMS3_1.ssp', None, False)
importAndSimulate('importFileOMS3_1.ssp', 'default.Gain2.y')

## solver settings of OMSimulator 3 on the system and the components are ignored
export('importFileOMS3_2.ssp', None, True)
importAndSimulate('importFileOMS3_2.ssp', 'default.Gain2.y')

## nested system: strongly coupled
export('importFileOMS3_3.ssp', 'sub', False)
importAndSimulate('importFileOMS3_3.ssp', 'default.sub.Gain2.y')

## Result:
## warning: oms_importFile is deprecated and will be removed in a future version. Use the Python API instead: import OMSimulator; OMSimulator.SSP("xxx.ssp")
## info:    oms_importFile: 0
## info:    oms_instantiate: 0
## info:    Result file: importFileOMS3_1.ssp_res.mat (bufferSize=1)
## info:    oms_initialize: 0
## info:    oms_simulate: 0
## info:    default.Gain2.y: 6.0
## info:    oms_terminate: 0
## info:    oms_delete: 0
## warning: oms_importFile is deprecated and will be removed in a future version. Use the Python API instead: import OMSimulator; OMSimulator.SSP("xxx.ssp")
## info:    oms_importFile: 0
## info:    oms_instantiate: 0
## info:    Result file: importFileOMS3_2.ssp_res.mat (bufferSize=1)
## info:    oms_initialize: 0
## info:    oms_simulate: 0
## info:    default.Gain2.y: 6.0
## info:    oms_terminate: 0
## info:    oms_delete: 0
## warning: oms_importFile is deprecated and will be removed in a future version. Use the Python API instead: import OMSimulator; OMSimulator.SSP("xxx.ssp")
## info:    oms_importFile: 0
## info:    oms_instantiate: 0
## info:    model doesn't contain any continuous state
## info:    maximum step size for 'default.default.sub': 0.001000
## info:    Result file: importFileOMS3_3.ssp_res.mat (bufferSize=1)
## info:    oms_initialize: 0
## info:    oms_simulate: 0
## info:    default.sub.Gain2.y: 6.0
## info:    Final Statistics for 'default.default.sub':
##          NumSteps = 1001 NumRhsEvals  = 1002 NumLinSolvSetups = 51
##          NumNonlinSolvIters = 1001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## info:    oms_terminate: 0
## info:    oms_delete: 0
## info:    3 warnings
## info:    0 errors
## endResult
