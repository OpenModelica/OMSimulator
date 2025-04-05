import ctypes
import os
from enum import Enum


class Status(Enum):
  '''Enumeration for status codes (oms_status_enu_t).'''
  ok = 0
  warning = 1
  discard = 2
  error = 3
  fatal = 4
  pending = 5


class capi:
  def __init__(self):
    dirname = os.path.dirname(__file__)
    ## look for dll in the current directory for the python pip package
    omslib = os.path.join(dirname, "@OMSIMULATORLIB_STRING@")

    dllSearchPath = False

    ## look for dll in the OpenModelica top level directory or OMSimulator stand alone directory
    if not os.path.exists(omslib):
      if os.name == 'nt': # Windows
        omslib = os.path.join(dirname, "@OMSIMULATOR_PYTHON_RELATIVE_DLL_DIR@", "@OMSIMULATORLIB_STRING@")
        dllDir = os.add_dll_directory(os.path.dirname(omslib))
        dllSearchPath = True
      else:
        # attempt to fix #8163 on Linux
        omslib = os.path.join(dirname, "..", "@OMSIMULATORLIB_STRING@")

    self.obj=ctypes.CDLL(omslib)

    if os.name == 'nt' and dllSearchPath: # Windows
      dllDir.close()

    self.obj.oms_getVersion.argtypes = None
    self.obj.oms_getVersion.restype = ctypes.c_char_p
    self.obj.oms3_instantiateFromJson.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_void_p)]
    self.obj.oms3_instantiateFromJson.restype = ctypes.c_int
    self.obj.oms3_initialize.argtypes = [ctypes.c_void_p]
    self.obj.oms3_initialize.restype = ctypes.c_int
    self.obj.oms3_simulate.argtypes = [ctypes.c_void_p]
    self.obj.oms3_simulate.restype = ctypes.c_int
    self.obj.oms3_stepUntil.argtypes = [ctypes.c_void_p, ctypes.c_double]
    self.obj.oms3_stepUntil.restype = ctypes.c_int
    self.obj.oms3_terminate.argtypes = [ctypes.c_void_p]
    self.obj.oms3_terminate.restype = ctypes.c_int

  def getVersion(self):
    return self.obj.oms_getVersion().decode('utf-8')
  def instantiateFromJson(self, model_json_desc):
    model_ptr = ctypes.c_void_p()
    status = self.obj.oms3_instantiateFromJson(model_json_desc.encode('utf-8'), ctypes.byref(model_ptr))
    if status != Status.ok:
      return [None, status]
    return [model_ptr, status]
  def initialize(self, model):
    return self.obj.oms3_initialize(model)
  def simulate(self, model):
    return self.obj.oms3_simulate(model)
  def stepUntil(self, model, stopTime):
    return self.obj.oms3_stepUntil(model, stopTime)
  def terminate(self, model):
    return self.obj.oms3_terminate(model)
