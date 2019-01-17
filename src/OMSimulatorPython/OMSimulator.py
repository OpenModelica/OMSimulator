from ctypes import cdll
import ctypes, sys

class OMSimulator:
  def __init__(self):
    if (sys.platform == 'win32'):
      if 'GCC' in sys.version:
        self.obj=cdll.LoadLibrary("libOMSimulator.dll")
      else:
        self.obj=cdll.LoadLibrary("OMSimulator.dll")
    else:
      self.obj=cdll.LoadLibrary("libOMSimulator.so")

    self.oms_system()
    self.oms_causality()
    self.oms_signal_type()
    self.oms_tlm_interpolation()
    self.oms_tlm_domain()

    self.obj.oms_getVersion.argtypes = None
    self.obj.oms_getVersion.restype = ctypes.c_char_p

    self.obj.oms_getSystemType.argtypes = [ctypes.c_char_p]
    self.obj.oms_getSystemType.restype = ctypes.c_int

    self.obj.oms_setLogFile.argtypes = [ctypes.c_char_p]
    self.obj.oms_setLogFile.restype = ctypes.c_int

    self.obj.oms_setLoggingInterval.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_setLoggingInterval.restype = ctypes.c_int

    self.obj.oms_setMaxLogFileSize.argtypes = [ctypes.c_ulong]
    self.obj.oms_setMaxLogFileSize.restype = None

    self.obj.oms_setTempDirectory.argtypes = [ctypes.c_char_p]
    self.obj.oms_setTempDirectory.restype = ctypes.c_int

    self.obj.oms_setWorkingDirectory.argtypes = [ctypes.c_char_p]
    self.obj.oms_setWorkingDirectory.restype = ctypes.c_int

    self.obj.oms_newModel.argtypes = [ctypes.c_char_p]
    self.obj.oms_newModel.restype = ctypes.c_int

    self.obj.oms_rename.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_rename.restype = ctypes.c_int

    self.obj.oms_delete.argtypes = [ctypes.c_char_p]
    self.obj.oms_delete.restype = ctypes.c_int

    self.obj.oms_export.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_export.restype = ctypes.c_int

    self.obj.oms_import.argtypes = [ctypes.c_char_p]
    self.obj.oms_import.restype = ctypes.c_int

    self.obj.oms_list.argtypes = [ctypes.c_char_p]
    self.obj.oms_list.restype = ctypes.c_int

    self.obj.oms_parseModelName.argtypes = [ctypes.c_char_p]
    self.obj.oms_parseModelName.restype = ctypes.c_int

    self.obj.oms_importString.argtypes = [ctypes.c_char_p]
    self.obj.oms_importString.restype = ctypes.c_int

    self.obj.oms_addSystem.argtypes = [ctypes.c_char_p, ctypes.c_int]
    self.obj.oms_addSystem.restype = ctypes.c_int

    self.obj.oms_copySystem.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_copySystem.restype = ctypes.c_int

    self.obj.oms_addSubModel.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addSubModel.restype = ctypes.c_int

    self.obj.oms_addConnector.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
    self.obj.oms_addConnector.restype = ctypes.c_int

    self.obj.oms_setCommandLineOption.argtypes = [ctypes.c_char_p]
    self.obj.oms_setCommandLineOption.restype = ctypes.c_int

    self.obj.oms_addConnection.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addConnection.restype = ctypes.c_int

    self.obj.oms_deleteConnection.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_deleteConnection.restype = ctypes.c_int

    self.obj.oms_addBus.argtypes = [ctypes.c_char_p]
    self.obj.oms_addBus.restype = ctypes.c_int

    self.obj.oms_addConnectorToBus.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addConnectorToBus.restype = ctypes.c_int

    self.obj.oms_deleteConnectorFromBus.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_deleteConnectorFromBus.restype = ctypes.c_int

    self.obj.oms_addTLMBus.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]
    self.obj.oms_addTLMBus.restype = ctypes.c_int

    self.obj.oms_addConnectorToTLMBus.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addConnectorToTLMBus.restype = ctypes.c_int

    self.obj.oms_deleteConnectorFromTLMBus.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_deleteConnectorFromTLMBus.restype = ctypes.c_int

    self.obj.oms_addTLMConnection.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_double, ctypes.c_double]
    self.obj.oms_addTLMConnection.restype = ctypes.c_int

    self.obj.oms_addExternalModel.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addExternalModel.restype = ctypes.c_int

    self.obj.oms_addSubModel.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addSubModel.restype = ctypes.c_int

    self.obj.oms_instantiate.argtypes = [ctypes.c_char_p]
    self.obj.oms_instantiate.restype = ctypes.c_int

    self.obj.oms_initialize.argtypes = [ctypes.c_char_p]
    self.obj.oms_initialize.restype = ctypes.c_int

    self.obj.oms_simulate.argtypes = [ctypes.c_char_p]
    self.obj.oms_simulate.restype = ctypes.c_int

    self.obj.oms_stepUntil.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_stepUntil.restype = ctypes.c_int

    self.obj.oms_terminate.argtypes = [ctypes.c_char_p]
    self.obj.oms_terminate.restype = ctypes.c_int

    self.obj.oms_reset.argtypes = [ctypes.c_char_p]
    self.obj.oms_reset.restype = ctypes.c_int

    self.obj.oms_setTLMSocketData.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
    self.obj.oms_setTLMSocketData.restype = ctypes.c_int

    self.obj.oms_setTLMPositionAndOrientation.argtypes = [ctypes.c_char_p, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double]
    self.obj.oms_setTLMPositionAndOrientation.restype = ctypes.c_int

    self.obj.oms_exportDependencyGraphs.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_exportDependencyGraphs.restype = ctypes.c_int

    self.obj.oms_getReal.argtypes = [ctypes.c_char_p]
    self.obj.oms_getReal.restype = ctypes.c_int

    self.obj.oms_setReal.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_setReal.restype = ctypes.c_int

    self.obj.oms_setResultFile.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int]
    self.obj.oms_setResultFile.restype = ctypes.c_int

    self.obj.oms_setSignalFilter.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_setSignalFilter.restype = ctypes.c_int

    self.obj.oms_addSignalsToResults.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addSignalsToResults.restype = ctypes.c_int

    self.obj.oms_removeSignalsFromResults.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_removeSignalsFromResults.restype = ctypes.c_int

    self.obj.oms_getStartTime.argtypes = [ctypes.c_char_p]
    self.obj.oms_getStartTime.restype = ctypes.c_int

    self.obj.oms_setStartTime.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_setStartTime.restype = ctypes.c_int

    self.obj.oms_getStopTime.argtypes = [ctypes.c_char_p]
    self.obj.oms_getStopTime.restype = ctypes.c_int

    self.obj.oms_setStopTime.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_setStopTime.restype = ctypes.c_int

    self.obj.oms_setFixedStepSize.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_setFixedStepSize.restype = ctypes.c_int

  ## define enum types for accesing typedef enum from c
  def oms_system(self):
    self.oms_system_none = 0
    self.oms_system_tlm = 1
    self.oms_system_wc = 2
    self.oms_system_sc = 3
  def oms_causality(self):
    self.input = 0
    self.output = 1
    self.parameter = 2
    self.bidir = 3
    self.undefined = 4
  def oms_signal_type(self):
    self.oms_signal_type_real = 0
    self.oms_signal_type_integer = 1
    self.oms_signal_type_boolean = 2
    self.oms_signal_type_string = 3
    self.oms_signal_type_enum = 4
    self.oms_signal_type_bus = 5
  def oms_tlm_interpolation(self):
    self.default = 0
    self.coarsegrained = 1
    self.finegrained = 2
  def oms_tlm_domain(self):
    self.oms_tlm_domain_input = 0
    self.oms_tlm_domain_output = 1
    self.oms_tlm_domain_mechanical = 2
    self.oms_tlm_domain_rotational = 3
    self.oms_tlm_domain_hydraulic = 4
    self.oms_tlm_domain_electric = 5

  def oms_getVersion(self):
    return self.checkstring(self.obj.oms_getVersion())
  def oms_getSystemType(self, cref):
    type = ctypes.c_int()
    status = self.obj.oms_getSystemType(self.checkstring(cref), ctypes.byref(type))
    return [status, type.value]
  def oms_setLogFile(self, filename):
    return self.obj.oms_setLogFile(self.checkstring(filename))
  def oms_setLoggingInterval(self, cref, loggingInterval):
    return self.obj.oms_setLoggingInterval(self.checkstring(cref), loggingInterval)
  def oms_setMaxLogFileSize(self, size):
    return self.obj.oms_setMaxLogFileSize(size)
  def oms_setTempDirectory(self, newTempDir):
    return self.obj.oms_setTempDirectory(self.checkstring(newTempDir))
  def oms_setWorkingDirectory(self, path):
    return self.obj.oms_setWorkingDirectory(self.checkstring(path))
  def oms_newModel(self, cref):
    return self.obj.oms_newModel(self.checkstring(cref))
  def oms_rename(self, cref, newcref):
    return self.obj.oms_rename(self.checkstring(cref), self.checkstring(newcref))
  def oms_delete(self, cref):
    return self.obj.oms_delete(self.checkstring(cref))
  def oms_export(self, cref, filename):
    return self.obj.oms_export(self.checkstring(cref), self.checkstring(filename))
  def oms_import(self, filename):
    contents = ctypes.c_char_p()
    status = self.obj.oms_import(self.checkstring(filename), ctypes.byref(contents))
    return [status, contents.value]
  def oms_list(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms_list(self.checkstring(ident), ctypes.byref(contents))
    #self.obj.oms_freeMemory(contents)
    return [status, contents.value]
  def oms_parseModelName(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms_parseModelName(self.checkstring(ident), ctypes.byref(contents))
    #self.obj.oms_freeMemory(contents)
    return [status, contents.value]
  def oms_importString(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms_importString(self.checkstring(ident), ctypes.byref(contents))
    #self.obj.oms_freeMemory(contents)
    return [status, contents.value]
  def oms_addSystem(self, ident, type):
    return self.obj.oms_addSystem(self.checkstring(ident), type)
  def oms_copySystem(self, source, target):
    return self.obj.oms_copySystem(self.checkstring(source), self.checkstring(target))
  def oms_addSubModel(self, cref, fmuPath):
    return self.obj.oms_addSubModel(self.checkstring(cref), self.checkstring(fmuPath))
  def oms_addConnector(self, cref, causality, type):
    return self.obj.oms_addConnector(self.checkstring(cref), causality, type)
  def oms_setCommandLineOption(self, cmd):
    return self.obj.oms_setCommandLineOption(self.checkstring(cmd))
  def oms_addConnection(self, crefA, crefB):
    return self.obj.oms_addConnection(self.checkstring(crefA), self.checkstring(crefB))
  def oms_deleteConnection(self, crefA, crefB):
    return self.obj.oms_deleteConnection(self.checkstring(crefA), self.checkstring(crefB))
  def oms_addBus(self, crefA):
    return self.obj.oms_addBus(self.checkstring(crefA))
  def oms_addConnectorToBus(self, busCref, connectorCref):
    return self.obj.oms_addConnectorToBus(self.checkstring(busCref), self.checkstring(connectorCref))
  def oms_deleteConnectorFromBus(self, busCref, connectorCref):
    return self.obj.oms_deleteConnectorFromBus(self.checkstring(busCref), self.checkstring(connectorCref))
  def oms_addTLMBus(self, cref, domain, dimensions, interpolation):
    return self.obj.oms_addTLMBus(self.checkstring(cref), self.checkstring(domain), dimensions, interpolation)
  def oms_addConnectorToTLMBus(self, busCref, connectorCref, type):
    return self.obj.oms_addConnectorToTLMBus(self.checkstring(busCref), self.checkstring(connectorCref), self.checkstring(type))
  def oms_deleteConnectorFromTLMBus(self, busCref, connectorCref):
    return self.obj.oms_deleteConnectorFromTLMBus(self.checkstring(busCref), self.checkstring(connectorCref))
  def oms_addTLMConnection(self, crefA, crefB, delay, alpha, linearimpedance, angularimpedance):
    return self.obj.oms_addTLMConnection(self.checkstring(crefA), self.checkstring(crefB), delay, alpha, linearimpedance, angularimpedance)
  def oms_addExternalModel(self, cref, path, startscript):
    return self.obj.oms_addExternalModel(self.checkstring(cref), self.checkstring(path), self.checkstring(startscript))
  def oms_instantiate(self, cref):
    return self.obj.oms_instantiate(self.checkstring(cref))
  def oms_initialize(self, cref):
    return self.obj.oms_initialize(self.checkstring(cref))
  def oms_simulate(self, cref):
    return self.obj.oms_simulate(self.checkstring(cref))
  def oms_stepUntil(self, cref, stopTime):
    return self.obj.oms_stepUntil(self.checkstring(cref), stopTime)
  def oms_terminate(self, cref):
    return self.obj.oms_terminate(self.checkstring(cref))
  def oms_reset(self, cref):
    return self.obj.oms_reset(self.checkstring(cref))
  def oms_setTLMSocketData(self, cref, address, managerPort, monitorPort):
    return self.obj.oms_setTLMSocketData(self.checkstring(cref), self.checkstring(address), managerPort, monitorPort)
  def oms_setTLMPositionAndOrientation(self, cref, x1, x2, x3, A11, A12, A13, A21, A22, A23, A31, A32, A33):
    return self.obj.oms_setTLMPositionAndOrientation(self.checkstring(cref), x1, x2, x3, A11, A12, A13, A21, A22, A23, A31, A32, A33)
  def oms_exportDependencyGraphs(self, cref, initialization, simulation):
    return self.obj.oms_exportDependencyGraphs(self.checkstring(cref), self.checkstring(initialization), self.checkstring(simulation))
  def oms_getReal(self, cref):
    value = ctypes.c_double()
    status = self.obj.oms_getReal(self.checkstring(cref), ctypes.byref(value))
    return [value.value, status]
  def oms_setReal(self, signal, value):
    return self.obj.oms_setReal(self.checkstring(signal), value)
  def oms_setResultFile(self, cref, filename, bufferSize):
    return self.obj.oms_setResultFile(self.checkstring(cref), self.checkstring(filename), bufferSize)
  def oms_setSignalFilter(self, cref, regex):
    return self.obj.oms_setSignalFilter(self.checkstring(cref), self.checkstring(regex))
  def oms_addSignalsToResults(self, cref, regex):
    return self.obj.oms_addSignalsToResults(self.checkstring(cref), self.checkstring(regex))
  def oms_removeSignalsFromResults(self, cref, regex):
    return self.obj.oms_removeSignalsFromResults(self.checkstring(cref), self.checkstring(regex))
  def oms_getStartTime(self, cref):
    startTime = ctypes.c_double()
    status = self.obj.oms_getStartTime(self.checkstring(cref), ctypes.byref(startTime))
    return [startTime.value, status]
  def oms_setStartTime(self, cref, startTime):
    return self.obj.oms_setStartTime(self.checkstring(cref), startTime)
  def oms_getStopTime(self, cref):
    stopTime = ctypes.c_double()
    status = self.obj.oms_getStopTime(self.checkstring(cref), ctypes.byref(stopTime))
    return [stopTime.value, status]
  def oms_setStopTime(self, cref, stopTime):
    return self.obj.oms_setStopTime(self.checkstring(cref), stopTime)
  def oms_setFixedStepSize(self, cref, stepSize):
    return self.obj.oms_setFixedStepSize(self.checkstring(cref), stepSize)
  def checkstring(self, ident):
    if isinstance(ident, bytes):
       ident=ident
    elif isinstance(ident, str):
       ident=ident.encode()
    return ident
