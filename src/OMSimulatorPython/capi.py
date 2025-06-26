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

    self.obj.oms_newModel.argtypes = [ctypes.c_char_p]
    self.obj.oms_newModel.restype = ctypes.c_int
    self.obj.oms_addSubModel.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addSubModel.restype = ctypes.c_int
    self.obj.oms_addSystem.argtypes = [ctypes.c_char_p, ctypes.c_int]
    self.obj.oms_addSystem.restype = ctypes.c_int
    self.obj.oms_addConnection.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_bool]
    self.obj.oms_addConnection.restype = ctypes.c_int
    self.obj.oms_instantiate.argtypes = [ctypes.c_char_p]
    self.obj.oms_instantiate.restype = ctypes.c_int
    self.obj.oms_initialize.argtypes = [ctypes.c_char_p]
    self.obj.oms_initialize.restype = ctypes.c_int
    self.obj.oms_simulate.argtypes = [ctypes.c_char_p]
    self.obj.oms_simulate.restype = ctypes.c_int
    self.obj.oms_terminate.argtypes = [ctypes.c_char_p]
    self.obj.oms_terminate.restype = ctypes.c_int

  def getVersion(self):
    return self.obj.oms_getVersion().decode('utf-8')

  def newModel(self, model_name: str = "default") -> Status:
    '''Create a new model with the given name.'''
    status = self.obj.oms_newModel(model_name.encode('utf-8'))
    return Status(status)

  def addSubModel(self, model_name: str, submodel_path: str) -> Status:
    '''Add a submodel to the current model.'''
    status = self.obj.oms_addSubModel(model_name.encode('utf-8'), submodel_path.encode('utf-8'))
    return Status(status)

  def addSystem(self, system_name: str, system_type: int) -> Status:
    '''Add a system to the current model.
    system_type: 0 for system, 1 for sub-system.'''
    status = self.obj.oms_addSystem(system_name.encode('utf-8'), system_type)
    return Status(status)

  def addConnection(self, crefA, crefB, suppressUnit=False):
    status = self.obj.oms_addConnection(crefA.encode(), crefB.encode(), suppressUnit)
    return Status(status)

  def instantiateFromJson(self, model_json_desc) -> tuple:
    '''Instantiate a model from a JSON description.'''
    model_ptr = ctypes.c_void_p()
    status = self.obj.oms3_instantiateFromJson(model_json_desc.encode('utf-8'), ctypes.byref(model_ptr))
    if status != Status.ok:
      return None, Status(status)
    return model_ptr, Status(status)

  def instantiate(self, cref):
    status = self.obj.oms_instantiate(cref.encode())
    return Status(status)

  def initialize(self, cref) -> Status:
    '''Enters initialization mode.'''
    status = self.obj.oms_initialize(cref.encode())
    return Status(status)

  def simulate(self, cref) -> Status:
    '''Exits initialization mode and runs the simulation until stopTime is reached.'''
    status = self.obj.oms_simulate(cref.encode())
    return Status(status)

  def stepUntil(self, model, stopTime) -> Status:
    '''Step the simulation until the specified stop time.
    Note: If the model is in initialization mode, this will exit initialization mode.'''
    status = self.obj.oms3_stepUntil(model, stopTime)
    return Status(status)

  def terminate(self, cref) -> Status:
    '''Terminate the simulation and free resources.'''
    status = self.obj.oms_terminate(cref.encode())
    return Status(status)


Capi = capi()
