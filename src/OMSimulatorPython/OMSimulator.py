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

    self.obj.oms3_getVersion.argtypes = None
    self.obj.oms3_getVersion.restype = ctypes.c_char_p

    self.obj.oms3_getSystemType.argtypes = [ctypes.c_char_p]
    self.obj.oms3_getSystemType.restype = ctypes.c_int

    self.obj.oms3_setLogFile.argtypes = [ctypes.c_char_p]
    self.obj.oms3_setLogFile.restype = ctypes.c_int

    self.obj.oms3_setLoggingInterval.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms3_setLoggingInterval.restype = ctypes.c_int

    self.obj.oms3_setMaxLogFileSize.argtypes = [ctypes.c_ulong]
    self.obj.oms3_setMaxLogFileSize.restype = None

    self.obj.oms3_setTempDirectory.argtypes = [ctypes.c_char_p]
    self.obj.oms3_setTempDirectory.restype = ctypes.c_int

    self.obj.oms3_setWorkingDirectory.argtypes = [ctypes.c_char_p]
    self.obj.oms3_setWorkingDirectory.restype = ctypes.c_int

    self.obj.oms3_newModel.argtypes = [ctypes.c_char_p]
    self.obj.oms3_newModel.restype = ctypes.c_int

    self.obj.oms3_rename.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms3_rename.restype = ctypes.c_int

    self.obj.oms3_delete.argtypes = [ctypes.c_char_p]
    self.obj.oms3_delete.restype = ctypes.c_int

    self.obj.oms3_export.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms3_export.restype = ctypes.c_int

    self.obj.oms3_import.argtypes = [ctypes.c_char_p]
    self.obj.oms3_import.restype = ctypes.c_int

    self.obj.oms3_list.argtypes = [ctypes.c_char_p]
    self.obj.oms3_list.restype = ctypes.c_int

    self.obj.oms3_parseModelName.argtypes = [ctypes.c_char_p]
    self.obj.oms3_parseModelName.restype = ctypes.c_int

    self.obj.oms3_importString.argtypes = [ctypes.c_char_p]
    self.obj.oms3_importString.restype = ctypes.c_int

    self.obj.oms3_addSystem.argtypes = [ctypes.c_char_p, ctypes.c_int]
    self.obj.oms3_addSystem.restype = ctypes.c_int

    self.obj.oms3_copySystem.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms3_copySystem.restype = ctypes.c_int

    self.obj.oms3_addSubModel.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms3_addSubModel.restype = ctypes.c_int

    self.obj.oms3_addConnector.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
    self.obj.oms3_addConnector.restype = ctypes.c_int

    self.obj.oms3_setCommandLineOption.argtypes = [ctypes.c_char_p]
    self.obj.oms3_setCommandLineOption.restype = ctypes.c_int

    self.obj.oms3_addConnection.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms3_addConnection.restype = ctypes.c_int

    self.obj.oms3_deleteConnection.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms3_deleteConnection.restype = ctypes.c_int

    self.obj.oms3_addBus.argtypes = [ctypes.c_char_p]
    self.obj.oms3_addBus.restype = ctypes.c_int

    self.obj.oms3_addConnectorToBus.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms3_addConnectorToBus.restype = ctypes.c_int

    self.obj.oms3_deleteConnectorFromBus.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms3_deleteConnectorFromBus.restype = ctypes.c_int

    self.obj.oms3_addTLMBus.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]
    self.obj.oms3_addTLMBus.restype = ctypes.c_int

    self.obj.oms3_addConnectorToTLMBus.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms3_addConnectorToTLMBus.restype = ctypes.c_int

    self.obj.oms3_deleteConnectorFromTLMBus.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms3_deleteConnectorFromTLMBus.restype = ctypes.c_int

    self.obj.oms3_addTLMConnection.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_double, ctypes.c_double]
    self.obj.oms3_addTLMConnection.restype = ctypes.c_int

    self.obj.oms3_addExternalModel.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms3_addExternalModel.restype = ctypes.c_int

    self.obj.oms3_addSubModel.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms3_addSubModel.restype = ctypes.c_int

    self.obj.oms3_instantiate.argtypes = [ctypes.c_char_p]
    self.obj.oms3_instantiate.restype = ctypes.c_int

    self.obj.oms3_initialize.argtypes = [ctypes.c_char_p]
    self.obj.oms3_initialize.restype = ctypes.c_int

    self.obj.oms3_simulate.argtypes = [ctypes.c_char_p]
    self.obj.oms3_simulate.restype = ctypes.c_int

    self.obj.oms3_stepUntil.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms3_stepUntil.restype = ctypes.c_int

    self.obj.oms3_terminate.argtypes = [ctypes.c_char_p]
    self.obj.oms3_terminate.restype = ctypes.c_int

    self.obj.oms3_reset.argtypes = [ctypes.c_char_p]
    self.obj.oms3_reset.restype = ctypes.c_int

    self.obj.oms3_setTLMSocketData.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
    self.obj.oms3_setTLMSocketData.restype = ctypes.c_int

    self.obj.oms3_setTLMPositionAndOrientation.argtypes = [ctypes.c_char_p, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double]
    self.obj.oms3_setTLMPositionAndOrientation.restype = ctypes.c_int

    self.obj.oms3_exportDependencyGraphs.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms3_exportDependencyGraphs.restype = ctypes.c_int

    self.obj.oms3_getReal.argtypes = [ctypes.c_char_p]
    self.obj.oms3_getReal.restype = ctypes.c_int

    self.obj.oms3_setReal.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms3_setReal.restype = ctypes.c_int

    self.obj.oms3_setResultFile.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int]
    self.obj.oms3_setResultFile.restype = ctypes.c_int

    self.obj.oms3_setSignalFilter.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms3_setSignalFilter.restype = ctypes.c_int

    self.obj.oms3_addSignalsToResults.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms3_addSignalsToResults.restype = ctypes.c_int

    self.obj.oms3_removeSignalsFromResults.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms3_removeSignalsFromResults.restype = ctypes.c_int

    self.obj.oms3_getStartTime.argtypes = [ctypes.c_char_p]
    self.obj.oms3_getStartTime.restype = ctypes.c_int

    self.obj.oms3_setStartTime.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms3_setStartTime.restype = ctypes.c_int

    self.obj.oms3_getStopTime.argtypes = [ctypes.c_char_p]
    self.obj.oms3_getStopTime.restype = ctypes.c_int

    self.obj.oms3_setStopTime.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms3_setStopTime.restype = ctypes.c_int

    self.obj.oms3_setFixedStepSize.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms3_setFixedStepSize.restype = ctypes.c_int

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

  def oms3_getVersion(self):
    return self.checkstring(self.obj.oms3_getVersion())
  def oms3_getSystemType(self, cref):
    type = ctypes.c_int()
    status = self.obj.oms3_getSystemType(self.checkstring(cref), ctypes.byref(type))
    return [status, type.value]
  def oms3_setLogFile(self, filename):
    return self.obj.oms3_setLogFile(self.checkstring(filename))
  def oms3_setLoggingInterval(self, cref, loggingInterval):
    return self.obj.oms3_setLoggingInterval(self.checkstring(cref), loggingInterval)
  def oms3_setMaxLogFileSize(self, size):
    return self.obj.oms3_setMaxLogFileSize(size)
  def oms3_setTempDirectory(self, newTempDir):
    return self.obj.oms3_setTempDirectory(self.checkstring(newTempDir))
  def oms3_setWorkingDirectory(self, path):
    return self.obj.oms3_setWorkingDirectory(self.checkstring(path))
  def oms3_newModel(self, cref):
    return self.obj.oms3_newModel(self.checkstring(cref))
  def oms3_rename(self, cref, newcref):
    return self.obj.oms3_rename(self.checkstring(cref), self.checkstring(newcref))
  def oms3_delete(self, cref):
    return self.obj.oms3_delete(self.checkstring(cref))
  def oms3_export(self, cref, filename):
    return self.obj.oms3_export(self.checkstring(cref), self.checkstring(filename))
  def oms3_import(self, filename):
    contents = ctypes.c_char_p()
    status = self.obj.oms3_import(self.checkstring(filename), ctypes.byref(contents))
    return [status, contents.value]
  def oms3_list(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms3_list(self.checkstring(ident), ctypes.byref(contents))
    #self.obj.oms3_freeMemory(contents)
    return [status, contents.value]
  def oms3_parseModelName(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms3_parseModelName(self.checkstring(ident), ctypes.byref(contents))
    #self.obj.oms3_freeMemory(contents)
    return [status, contents.value]
  def oms3_importString(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms3_importString(self.checkstring(ident), ctypes.byref(contents))
    #self.obj.oms3_freeMemory(contents)
    return [status, contents.value]
  def oms3_addSystem(self, ident, type):
    return self.obj.oms3_addSystem(self.checkstring(ident), type)
  def oms3_copySystem(self, source, target):
    return self.obj.oms3_copySystem(self.checkstring(source), self.checkstring(target))
  def oms3_addSubModel(self, cref, fmuPath):
    return self.obj.oms3_addSubModel(self.checkstring(cref), self.checkstring(fmuPath))
  def oms3_addConnector(self, cref, causality, type):
    return self.obj.oms3_addConnector(self.checkstring(cref), causality, type)
  def oms3_setCommandLineOption(self, cmd):
    return self.obj.oms3_setCommandLineOption(self.checkstring(cmd))
  def oms3_addConnection(self, crefA, crefB):
    return self.obj.oms3_addConnection(self.checkstring(crefA), self.checkstring(crefB))
  def oms3_deleteConnection(self, crefA, crefB):
    return self.obj.oms3_deleteConnection(self.checkstring(crefA), self.checkstring(crefB))
  def oms3_addBus(self, crefA):
    return self.obj.oms3_addBus(self.checkstring(crefA))
  def oms3_addConnectorToBus(self, busCref, connectorCref):
    return self.obj.oms3_addConnectorToBus(self.checkstring(busCref), self.checkstring(connectorCref))
  def oms3_deleteConnectorFromBus(self, busCref, connectorCref):
    return self.obj.oms3_deleteConnectorFromBus(self.checkstring(busCref), self.checkstring(connectorCref))
  def oms3_addTLMBus(self, cref, domain, dimensions, interpolation):
    return self.obj.oms3_addTLMBus(self.checkstring(cref), self.checkstring(domain), dimensions, interpolation)
  def oms3_addConnectorToTLMBus(self, busCref, connectorCref, type):
    return self.obj.oms3_addConnectorToTLMBus(self.checkstring(busCref), self.checkstring(connectorCref), self.checkstring(type))
  def oms3_deleteConnectorFromTLMBus(self, busCref, connectorCref):
    return self.obj.oms3_deleteConnectorFromTLMBus(self.checkstring(busCref), self.checkstring(connectorCref))
  def oms3_addTLMConnection(self, crefA, crefB, delay, alpha, linearimpedance, angularimpedance):
    return self.obj.oms3_addTLMConnection(self.checkstring(crefA), self.checkstring(crefB), delay, alpha, linearimpedance, angularimpedance)
  def oms3_addExternalModel(self, cref, path, startscript):
    return self.obj.oms3_addExternalModel(self.checkstring(cref), self.checkstring(path), self.checkstring(startscript))
  def oms3_instantiate(self, cref):
    return self.obj.oms3_instantiate(self.checkstring(cref))
  def oms3_initialize(self, cref):
    return self.obj.oms3_initialize(self.checkstring(cref))
  def oms3_simulate(self, cref):
    return self.obj.oms3_simulate(self.checkstring(cref))
  def oms3_stepUntil(self, cref, stopTime):
    return self.obj.oms3_stepUntil(self.checkstring(cref), stopTime)
  def oms3_terminate(self, cref):
    return self.obj.oms3_terminate(self.checkstring(cref))
  def oms3_reset(self, cref):
    return self.obj.oms3_reset(self.checkstring(cref))
  def oms3_setTLMSocketData(self, cref, address, managerPort, monitorPort):
    return self.obj.oms3_setTLMSocketData(self.checkstring(cref), self.checkstring(address), managerPort, monitorPort)
  def oms3_setTLMPositionAndOrientation(self, cref, x1, x2, x3, A11, A12, A13, A21, A22, A23, A31, A32, A33):
    return self.obj.oms3_setTLMPositionAndOrientation(self.checkstring(cref), x1, x2, x3, A11, A12, A13, A21, A22, A23, A31, A32, A33)
  def oms3_exportDependencyGraphs(self, cref, initialization, simulation):
    return self.obj.oms3_exportDependencyGraphs(self.checkstring(cref), self.checkstring(initialization), self.checkstring(simulation))
  def oms3_getReal(self, cref):
    value = ctypes.c_double()
    status = self.obj.oms3_getReal(self.checkstring(cref), ctypes.byref(value))
    return [value.value, status]
  def oms3_setReal(self, signal, value):
    return self.obj.oms3_setReal(self.checkstring(signal), value)
  def oms3_setResultFile(self, cref, filename, bufferSize):
    return self.obj.oms3_setResultFile(self.checkstring(cref), self.checkstring(filename), bufferSize)
  def oms3_setSignalFilter(self, cref, regex):
    return self.obj.oms3_setSignalFilter(self.checkstring(cref), self.checkstring(regex))
  def oms3_addSignalsToResults(self, cref, regex):
    return self.obj.oms3_addSignalsToResults(self.checkstring(cref), self.checkstring(regex))
  def oms3_removeSignalsFromResults(self, cref, regex):
    return self.obj.oms3_removeSignalsFromResults(self.checkstring(cref), self.checkstring(regex))
  def oms3_getStartTime(self, cref):
    startTime = ctypes.c_double()
    status = self.obj.oms3_getStartTime(self.checkstring(cref), ctypes.byref(startTime))
    return [startTime.value, status]
  def oms3_setStartTime(self, cref, startTime):
    return self.obj.oms3_setStartTime(self.checkstring(cref), startTime)
  def oms3_getStopTime(self, cref):
    stopTime = ctypes.c_double()
    status = self.obj.oms3_getStopTime(self.checkstring(cref), ctypes.byref(stopTime))
    return [stopTime.value, status]
  def oms3_setStopTime(self, cref, stopTime):
    return self.obj.oms3_setStopTime(self.checkstring(cref), stopTime)
  def oms3_setFixedStepSize(self, cref, stepSize):
    return self.obj.oms3_setFixedStepSize(self.checkstring(cref), stepSize)
  def checkstring(self, ident):
    if isinstance(ident, bytes):
       ident=ident
    elif isinstance(ident, str):
       ident=ident.encode()
    return ident
