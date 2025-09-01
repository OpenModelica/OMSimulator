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

    self.obj.oms_addConnection.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_bool]
    self.obj.oms_addConnection.restype = ctypes.c_int
    self.obj.oms_addConnector.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
    self.obj.oms_addConnector.restype = ctypes.c_int
    self.obj.oms_addSubModel.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addSubModel.restype = ctypes.c_int
    self.obj.oms_addSystem.argtypes = [ctypes.c_char_p, ctypes.c_int]
    self.obj.oms_addSystem.restype = ctypes.c_int
    self.obj.oms_delete.argtypes = [ctypes.c_char_p]
    self.obj.oms_delete.restype = ctypes.c_int
    self.obj.oms_getVersion.argtypes = None
    self.obj.oms_getVersion.restype = ctypes.c_char_p
    self.obj.oms_getBoolean.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_bool)]
    self.obj.oms_getBoolean.restype = ctypes.c_int
    self.obj.oms_getInteger.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_int)]
    self.obj.oms_getInteger.restype = ctypes.c_int
    self.obj.oms_getReal.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_double)]
    self.obj.oms_getReal.restype = ctypes.c_int
    self.obj.oms_getString.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p)]
    self.obj.oms_getString.restype = ctypes.c_int
    self.obj.oms_getVariableType.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_int)]
    self.obj.oms_getVariableType.restype = ctypes.c_int
    self.obj.oms_initialize.argtypes = [ctypes.c_char_p]
    self.obj.oms_initialize.restype = ctypes.c_int
    self.obj.oms_instantiate.argtypes = [ctypes.c_char_p]
    self.obj.oms_instantiate.restype = ctypes.c_int
    self.obj.oms_newModel.argtypes = [ctypes.c_char_p]
    self.obj.oms_newModel.restype = ctypes.c_int
    self.obj.oms_setCommandLineOption.argtypes = [ctypes.c_char_p]
    self.obj.oms_setCommandLineOption.restype = ctypes.c_int
    self.obj.oms_setTempDirectory.argtypes = [ctypes.c_char_p]
    self.obj.oms_setTempDirectory.restype = ctypes.c_int
    self.obj.oms_setExportName.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_setExportName.restype = ctypes.c_int
    self.obj.oms_setBoolean.argtypes = [ctypes.c_char_p, ctypes.c_bool]
    self.obj.oms_setBoolean.restype = ctypes.c_int
    self.obj.oms_setInteger.argtypes = [ctypes.c_char_p, ctypes.c_int]
    self.obj.oms_setInteger.restype = ctypes.c_int
    self.obj.oms_setReal.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_setReal.restype = ctypes.c_int
    self.obj.oms_setString.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_setString.restype = ctypes.c_int
    self.obj.oms_setResultFile.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int]
    self.obj.oms_setResultFile.restype = ctypes.c_int
    self.obj.oms_simulate.argtypes = [ctypes.c_char_p]
    self.obj.oms_simulate.restype = ctypes.c_int
    self.obj.oms_stepUntil.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_stepUntil.restype = ctypes.c_int
    self.obj.oms_terminate.argtypes = [ctypes.c_char_p]
    self.obj.oms_terminate.restype = ctypes.c_int

  def addConnection(self, crefA, crefB, suppressUnit=False):
    status = self.obj.oms_addConnection(crefA.encode(), crefB.encode(), suppressUnit)
    return Status(status)

  def addConnector(self, cref, causality : int, signal_type : int):
    status = self.obj.oms_addConnector(cref.encode(), causality, signal_type)
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

  def delete(self, cref):
    status = self.obj.oms_delete(cref.encode())
    return Status(status)

  def getVersion(self):
    return self.obj.oms_getVersion().decode('utf-8')

  def getReal(self, cref):
    value = ctypes.c_double()
    status = self.obj.oms_getReal(cref.encode(), ctypes.byref(value))
    return [value.value, Status(status)]

  def getInteger(self, cref):
    value = ctypes.c_int()
    status = self.obj.oms_getInteger(cref.encode(), ctypes.byref(value))
    return [value.value, Status(status)]

  def getString(self, cref):
    value = ctypes.c_char_p()
    status = self.obj.oms_getString(cref.encode(), ctypes.byref(value))
    value_ = value.value.decode('utf-8') if value.value else None
    self.obj.oms_freeMemory(value)
    return [value_, Status(status)]

  def getBoolean(self, cref):
    value = ctypes.c_bool()
    status = self.obj.oms_getBoolean(cref.encode(), ctypes.byref(value))
    return [value.value, Status(status)]

  def getVariableType(self, cref):
    value = ctypes.c_int()
    status = self.obj.oms_getVariableType(cref.encode(), ctypes.byref(value))
    return [value.value, Status(status)]

  def initialize(self, cref) -> Status:
    '''Enters initialization mode.'''
    status = self.obj.oms_initialize(cref.encode())
    return Status(status)

  def instantiate(self, cref):
    status = self.obj.oms_instantiate(cref.encode())
    return Status(status)

  def newModel(self, model_name: str = "default") -> Status:
    '''Create a new model with the given name.'''
    status = self.obj.oms_newModel(model_name.encode('utf-8'))
    return Status(status)

  def setCommandLineOption(self, cmd):
    status = self.obj.oms_setCommandLineOption(cmd.encode())
    return Status(status)

  def setTempDirectory(self, newTempDir):
    status = self.obj.oms_setTempDirectory(newTempDir.encode())
    return Status(status)

  def setExportName(self, cref, exportName):
    '''Set the export name for a model or system.
    This is used to specify the name under which the model will be exported.'''
    status = self.obj.oms_setExportName(cref.encode(), exportName.encode())
    return Status(status)

  def setReal(self, cref, value):
    '''Set a real value for a model or system.'''
    status = self.obj.oms_setReal(cref.encode(), value)
    return Status(status)

  def setInteger(self, cref, value):
    '''Set a integer value for a model or system.'''
    status = self.obj.oms_setInteger(cref.encode(), value)
    return Status(status)

  def setBoolean(self, cref, value):
    '''Set a boolean value for a model or system.'''
    status = self.obj.oms_setBoolean(cref.encode(), value)
    return Status(status)

  def setString(self, cref, value):
    '''Set a string value for a model or system.'''
    status = self.obj.oms_setString(cref.encode(), value)
    return Status(status)

  def setResultFile(self, cref, filename, bufferSize=1) -> Status:
    '''Set the result file for a model or system.
    The bufferSize specifies how many values are buffered before writing to the file.'''
    status = self.obj.oms_setResultFile(cref.encode(), filename.encode(), bufferSize)
    return Status(status)

  def simulate(self, cref) -> Status:
    '''Exits initialization mode and runs the simulation until stopTime is reached.'''
    status = self.obj.oms_simulate(cref.encode())
    return Status(status)

  def stepUntil(self, model, stopTime) -> Status:
    '''Step the simulation until the specified stop time.
    Note: If the model is in initialization mode, this will exit initialization mode.'''
    status = self.obj.oms_stepUntil(model, stopTime)
    return Status(status)

  def terminate(self, cref) -> Status:
    '''Terminate the simulation and free resources.'''
    status = self.obj.oms_terminate(cref.encode())
    return Status(status)


Capi = capi()
