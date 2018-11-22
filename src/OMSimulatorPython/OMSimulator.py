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

    self.obj.oms2_addConnection.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms2_addConnection.restype = ctypes.c_int

    self.obj.oms2_addFMU.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms2_addFMU.restype = ctypes.c_int

    self.obj.oms2_addSolver.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms2_addSolver.restype = ctypes.c_int

    self.obj.oms2_addTable.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms2_addTable.restype = ctypes.c_int

    self.obj.oms2_addSignalsToResults.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms2_addSignalsToResults.restype = ctypes.c_int

    self.obj.oms2_compareSimulationResults.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_double]
    self.obj.oms2_compareSimulationResults.restype = ctypes.c_int

    self.obj.oms2_deleteConnection.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms2_deleteConnection.restype = ctypes.c_int

    self.obj.oms2_deleteSubModel.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms2_deleteSubModel.restype = ctypes.c_int

    self.obj.oms2_describe.argtypes = [ctypes.c_char_p]
    self.obj.oms2_describe.restype = ctypes.c_int

    self.obj.oms2_doSteps.argtypes = [ctypes.c_char_p, ctypes.c_int]
    self.obj.oms2_doSteps.restype = ctypes.c_int

    self.obj.oms2_exportCompositeStructure.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms2_exportCompositeStructure.restype = ctypes.c_int

    self.obj.oms2_exportDependencyGraphs.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms2_exportDependencyGraphs.restype = ctypes.c_int

    self.obj.oms2_getBoolean.argtypes = [ctypes.c_char_p, ctypes.c_bool]
    self.obj.oms2_getBoolean.restype = ctypes.c_int

    self.obj.oms2_getBooleanParameter.argtypes = [ctypes.c_char_p, ctypes.c_bool]
    self.obj.oms2_getBooleanParameter.restype = ctypes.c_int

    self.obj.oms2_getCurrentTime.argtypes = [ctypes.c_char_p]
    self.obj.oms2_getCurrentTime.restype = ctypes.c_int

    self.obj.oms2_getSubModelPath.argtypes = [ctypes.c_char_p]
    self.obj.oms2_getSubModelPath.restype = ctypes.c_int

    self.obj.oms2_getInteger.argtypes = [ctypes.c_char_p, ctypes.c_int]
    self.obj.oms2_getInteger.restype = ctypes.c_int

    self.obj.oms2_getIntegerParameter.argtypes = [ctypes.c_char_p, ctypes.c_int]
    self.obj.oms2_getIntegerParameter.restype = ctypes.c_int

    self.obj.oms2_getReal.argtypes = [ctypes.c_char_p]
    self.obj.oms2_getReal.restype = ctypes.c_int

    self.obj.oms2_getRealParameter.argtypes = [ctypes.c_char_p]
    self.obj.oms2_getRealParameter.restype = ctypes.c_int

    self.obj.oms2_getVersion.argtypes = None
    self.obj.oms2_getVersion.restype = ctypes.c_char_p

    self.obj.oms2_initialize.argtypes = [ctypes.c_char_p]
    self.obj.oms2_initialize.restype = ctypes.c_int

    self.obj.oms2_loadModel.argtypes = [ctypes.c_char_p]
    self.obj.oms2_loadModel.restype = ctypes.c_int

    self.obj.oms2_parseString.argtypes = [ctypes.c_char_p]
    self.obj.oms2_parseString.restype = ctypes.c_int

    self.obj.oms2_loadString.argtypes = [ctypes.c_char_p]
    self.obj.oms2_loadString.restype = ctypes.c_int

    self.obj.oms2_newFMIModel.argtypes = [ctypes.c_char_p]
    self.obj.oms2_newFMIModel.restype = ctypes.c_int

    self.obj.oms2_removeSignalsFromResults.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms2_removeSignalsFromResults.restype = ctypes.c_int

    self.obj.oms2_rename.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms2_rename.restype = ctypes.c_int

    self.obj.oms2_reset.argtypes = [ctypes.c_char_p]
    self.obj.oms2_reset.restype = ctypes.c_int

    self.obj.oms2_saveModel.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms2_saveModel.restype = ctypes.c_int

    self.obj.oms2_listModel.argtypes = [ctypes.c_char_p]
    self.obj.oms2_listModel.restype = ctypes.c_int

    self.obj.oms2_setBoolean.argtypes = [ctypes.c_char_p, ctypes.c_bool]
    self.obj.oms2_setBoolean.restype = ctypes.c_int

    self.obj.oms2_setBooleanParameter.argtypes = [ctypes.c_char_p, ctypes.c_bool]
    self.obj.oms2_setBooleanParameter.restype = ctypes.c_int

    self.obj.oms2_setCommunicationInterval.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms2_setCommunicationInterval.restype = ctypes.c_int

    self.obj.oms2_setLoggingInterval.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms2_setLoggingInterval.restype = ctypes.c_int

    self.obj.oms2_setInteger.argtypes = [ctypes.c_char_p, ctypes.c_int]
    self.obj.oms2_setInteger.restype = ctypes.c_int

    self.obj.oms2_setIntegerParameter.argtypes = [ctypes.c_char_p, ctypes.c_int]
    self.obj.oms2_setIntegerParameter.restype = ctypes.c_int

    self.obj.oms2_setLogFile.argtypes = [ctypes.c_char_p]
    self.obj.oms2_setLogFile.restype = ctypes.c_int

    self.obj.oms2_setLoggingLevel.argtypes = [ctypes.c_int]
    self.obj.oms2_setLoggingLevel.restype = None

    self.obj.oms2_setMasterAlgorithm.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms2_setMasterAlgorithm.restype = ctypes.c_int

    self.obj.oms2_setReal.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms2_setReal.restype = ctypes.c_int

    self.obj.oms2_setRealParameter.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms2_setRealParameter.restype = ctypes.c_int

    self.obj.oms2_setResultFile.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_uint]
    self.obj.oms2_setResultFile.restype = ctypes.c_int

    self.obj.oms2_getStartTime.argtypes = [ctypes.c_char_p]
    self.obj.oms2_getStartTime.restype = ctypes.c_int

    self.obj.oms2_setStartTime.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms2_setStartTime.restype = ctypes.c_int

    self.obj.oms2_getStopTime.argtypes = [ctypes.c_char_p]
    self.obj.oms2_getStopTime.restype = ctypes.c_int

    self.obj.oms2_setStopTime.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms2_setStopTime.restype = ctypes.c_int

    self.obj.oms2_setTempDirectory.argtypes = [ctypes.c_char_p]
    self.obj.oms2_setTempDirectory.restype = ctypes.c_int

    self.obj.oms2_setWorkingDirectory.argtypes = [ctypes.c_char_p]
    self.obj.oms2_setWorkingDirectory.restype = ctypes.c_int

    self.obj.oms2_simulate.argtypes = [ctypes.c_char_p]
    self.obj.oms2_simulate.restype = ctypes.c_int

    self.obj.oms2_stepUntil.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms2_stepUntil.restype = ctypes.c_int

    self.obj.oms2_unloadModel.argtypes = [ctypes.c_char_p]
    self.obj.oms2_unloadModel.restype = ctypes.c_int

    self.obj.oms2_freeMemory.argtypes = [ctypes.c_void_p]
    self.obj.oms2_freeMemory.restype = ctypes.c_int

    self.obj.oms2_exists.argtypes = [ctypes.c_char_p]
    self.obj.oms2_exists.restype = ctypes.c_int

    ## oms3
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
    return self.obj.oms3_getVersion()
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
    #self.obj.oms2_freeMemory(contents)
    return [status, contents.value]
  def oms3_parseModelName(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms3_parseModelName(self.checkstring(ident), ctypes.byref(contents))
    #self.obj.oms2_freeMemory(contents)
    return [status, contents.value]
  def oms3_importString(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms3_importString(self.checkstring(ident), ctypes.byref(contents))
    #self.obj.oms2_freeMemory(contents)
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

  def addConnection(self, cref, conA, conB):
    return self.obj.oms2_addConnection(self.checkstring(cref), self.checkstring(conA), self.checkstring(conB))
  def addFMU(self, modelIdent, fmuPath, fmuIdent):
    return self.obj.oms2_addFMU(self.checkstring(modelIdent), self.checkstring(fmuPath), self.checkstring(fmuIdent))
  def addSolver(self, modelIdent, solverIdent, method):
    return self.obj.oms2_addSolver(self.checkstring(modelIdent), self.checkstring(solverIdent), self.checkstring(method))
  def addTable(self, modelIdent, tablePath, tableIdent):
    return self.obj.oms2_addTable(self.checkstring(modelIdent), self.checkstring(tablePath), self.checkstring(tableIdent))
  def addSignalsToResults(self, cref, regex):
    return self.obj.oms2_addSignalsToResults(self.checkstring(cref), self.checkstring(regex))
  def compareSimulationResults(self, filenameA, filenameB, var, relTol, absTol):
    return self.obj.oms2_compareSimulationResults(self.checkstring(filenameA), self.checkstring(filenameB), self.checkstring(var), relTol, absTol)
  def deleteConnection(self, cref, conA, conB):
    return self.obj.oms2_deleteConnection(self.checkstring(cref), self.checkstring(conA), self.checkstring(conB))
  def deleteSubModel(self, modelIdent, subModelIdent):
    return self.obj.oms2_deleteSubModel(self.checkstring(modelIdent), self.checkstring(subModelIdent))
  def describe(self, cref):
    return self.obj.oms2_describe(self.checkstring(cref))
  def doSteps(self, ident, numberOfSteps):
    return self.obj.oms2_doSteps(self.checkstring(ident), numberOfSteps)
  def exists(self, cref):
    return self.obj.oms2_exists(self.checkstring(cref))
  def exportCompositeStructure(self, cref, filename):
    return self.obj.oms2_exportCompositeStructure(self.checkstring(cref), self.checkstring(filename))
  def exportDependencyGraphs(self, cref, initialization, simulation):
    return self.obj.oms2_exportDependencyGraphs(self.checkstring(cref), self.checkstring(initialization), self.checkstring(simulation))
  def getBoolean(self, signal):
    value = ctypes.c_bool()
    status = self.obj.oms2_getBoolean(self.checkstring(signal), ctypes.byref(value))
    return [status, value.value]
  def getBooleanParameter(self, signal):
    value = ctypes.c_bool()
    status = self.obj.oms2_getBooleanParameter(self.checkstring(signal), ctypes.byref(value))
    return [status, value.value]
  def getCurrentTime(self, ident):
    time = ctypes.c_double()
    status = self.obj.oms2_getCurrentTime(self.checkstring(ident), ctypes.byref(time))
    return [status, time.value]
  def getSubModelPath(self, cref):
    path = ctypes.c_char_p()
    status = self.obj.oms2_getSubModelPath(self.checkstring(cref), ctypes.byref(path))
    return [status, path.value]
  def getInteger(self, signal):
    value = ctypes.c_int()
    status = self.obj.oms2_getInteger(self.checkstring(signal), ctypes.byref(value))
    return [status, value.value]
  def getIntegerParameter(self, signal):
    value = ctypes.c_int()
    status = self.obj.oms2_getIntegerParameter(self.checkstring(signal), ctypes.byref(value))
    return [status, value.value]
  def getReal(self, signal):
    value = ctypes.c_double()
    status = self.obj.oms2_getReal(self.checkstring(signal), ctypes.byref(value))
    return [status, value.value]
  def getRealParameter(self, signal):
    value = ctypes.c_double()
    status = self.obj.oms2_getRealParameter(self.checkstring(signal), ctypes.byref(value))
    return [status, value.value]
  def getVersion(self):
    return self.obj.oms2_getVersion()
  def initialize(self, ident):
    return self.obj.oms2_initialize(self.checkstring(ident))
  def loadModel(self, filename):
    ident = ctypes.c_char_p()
    status = self.obj.oms2_loadModel(self.checkstring(filename), ctypes.byref(ident))
    return [status, ident.value]
  def parseString(self, contents):
    ident = ctypes.c_char_p()
    status = self.obj.oms2_parseString(self.checkstring(contents), ctypes.byref(ident))
    #self.obj.oms2_freeMemory(ident)
    return [status, ident.value]
  def loadString(self, contents):
    ident = ctypes.c_char_p()
    status = self.obj.oms2_loadString(self.checkstring(contents), ctypes.byref(ident))
    return [status, ident.value]
  def newFMIModel(self, ident):
    return self.obj.oms2_newFMIModel(self.checkstring(ident))
  def rename(self, identOld, identNew):
    return self.obj.oms2_rename(self.checkstring(identOld), self.checkstring(identNew))
  def removeSignalsFromResults(self, cref, regex):
    return self.obj.oms2_removeSignalsFromResults(self.checkstring(cref), self.checkstring(regex))
  def reset(self, ident):
    return self.obj.oms2_reset(self.checkstring(ident))
  def saveModel(self, ident, filename):
    return self.obj.oms2_saveModel(self.checkstring(ident), self.checkstring(filename))
  def listModel(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms2_listModel(self.checkstring(ident), ctypes.byref(contents))
    #self.obj.oms2_freeMemory(contents)
    return [status, contents.value]
  def setBoolean(self, signal, value):
    return self.obj.oms2_setBoolean(self.checkstring(signal), value)
  def setBooleanParameter(self, signal, value):
    return self.obj.oms2_setBooleanParameter(self.checkstring(signal), value)
  def setCommunicationInterval(self, cref, communicationInterval):
    return self.obj.oms2_setCommunicationInterval(self.checkstring(cref), communicationInterval)
  def setLoggingInterval(self, cref, loggingInterval):
    return self.obj.oms2_setLoggingInterval(self.checkstring(cref), loggingInterval)
  def setInteger(self, signal, value):
    return self.obj.oms2_setInteger(self.checkstring(signal), value)
  def setIntegerParameter(self, signal, value):
    return self.obj.oms2_setIntegerParameter(self.checkstring(signal), value)
  def setLogFile(self, filename):
    return self.obj.oms2_setLogFile(self.checkstring(filename))
  def setLoggingLevel(self, logLevel):
    return self.obj.oms2_setLoggingLevel(logLevel)
  def setMasterAlgorithm(self, ident, masterAlgorithm):
    return self.obj.oms2_setMasterAlgorithm(self.checkstring(ident), self.checkstring(masterAlgorithm))
  def setReal(self, signal, value):
    return self.obj.oms2_setReal(self.checkstring(signal), value)
  def setRealParameter(self, signal, value):
    return self.obj.oms2_setRealParameter(self.checkstring(signal), value)
  def setResultFile(self, cref, filename, bufferSize=1):
    return self.obj.oms2_setResultFile(self.checkstring(cref), self.checkstring(filename), bufferSize)
  def getStartTime(self, ident):
    startTime = ctypes.c_double()
    status = self.obj.oms2_getStartTime(self.checkstring(ident), ctypes.byref(startTime))
    return [status, startTime.value]
  def setStartTime(self, cref, startTime):
    return self.obj.oms2_setStartTime(self.checkstring(cref), startTime)
  def getStopTime(self, ident):
    stopTime = ctypes.c_double()
    status = self.obj.oms2_getStopTime(self.checkstring(ident), ctypes.byref(stopTime))
    return [status, stopTime.value]
  def setStopTime(self, cref, stopTime):
    return self.obj.oms2_setStopTime(self.checkstring(cref), stopTime)
  def setTempDirectory(self, filename):
    return self.obj.oms2_setTempDirectory(self.checkstring(filename))
  def setWorkingDirectory(self, path):
    return self.obj.oms2_setWorkingDirectory(self.checkstring(path))
  def simulate(self, ident):
    return self.obj.oms2_simulate(self.checkstring(ident))
  def stepUntil(self, ident, timeValue):
    ident=self.checkstring(ident)
    return self.obj.oms2_stepUntil(self.checkstring(ident), timeValue)
  def unloadModel(self, ident):
    ident=self.checkstring(ident)
    return self.obj.oms2_unloadModel(ident)
  def checkstring(self, ident):
    if isinstance(ident, bytes):
       ident=ident
    elif isinstance(ident, str):
       ident=ident.encode()
    return ident
