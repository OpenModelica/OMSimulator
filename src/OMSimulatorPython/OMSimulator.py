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

    ## oms_system_enu_t
    self.system_none = 0
    self.system_tlm = 1
    self.system_wc = 2
    self.system_sc = 3

    ## oms_causality_enu_t
    self.input = 0
    self.output = 1
    self.parameter = 2
    self.bidir = 3
    self.undefined = 4

    ## oms_signal_type_enu_t
    self.signal_type_real = 0
    self.signal_type_integer = 1
    self.signal_type_boolean = 2
    self.signal_type_string = 3
    self.signal_type_enum = 4
    self.signal_type_bus = 5

    ## tlm_interpolation
    self.default = 0
    self.coarsegrained = 1
    self.finegrained = 2

    ## oms_tlm_domain_t
    self.tlm_domain_input = 0
    self.tlm_domain_output = 1
    self.tlm_domain_mechanical = 2
    self.tlm_domain_rotational = 3
    self.tlm_domain_hydraulic = 4
    self.tlm_domain_electric = 5

    ## oms_solver_enu_t
    self.solver_sc_explicit_euler = 2
    self.solver_sc_cvode = 3
    self.solver_wc_ma = 6
    self.solver_wc_mav = 7

    self.obj.oms_addBus.argtypes = [ctypes.c_char_p]
    self.obj.oms_addBus.restype = ctypes.c_int
    self.obj.oms_addConnection.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addConnection.restype = ctypes.c_int
    self.obj.oms_addConnector.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
    self.obj.oms_addConnector.restype = ctypes.c_int
    self.obj.oms_addConnectorToBus.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addConnectorToBus.restype = ctypes.c_int
    self.obj.oms_addConnectorToTLMBus.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addConnectorToTLMBus.restype = ctypes.c_int
    self.obj.oms_addExternalModel.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addExternalModel.restype = ctypes.c_int
    self.obj.oms_addSignalsToResults.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addSignalsToResults.restype = ctypes.c_int
    self.obj.oms_addSubModel.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addSubModel.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addSubModel.restype = ctypes.c_int
    self.obj.oms_addSubModel.restype = ctypes.c_int
    self.obj.oms_addSystem.argtypes = [ctypes.c_char_p, ctypes.c_int]
    self.obj.oms_addSystem.restype = ctypes.c_int
    self.obj.oms_addTLMBus.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int]
    self.obj.oms_addTLMBus.restype = ctypes.c_int
    self.obj.oms_addTLMConnection.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_double, ctypes.c_double]
    self.obj.oms_addTLMConnection.restype = ctypes.c_int
    self.obj.oms_compareSimulationResults.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_double]
    self.obj.oms_compareSimulationResults.restype = ctypes.c_int
    self.obj.oms_copySystem.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_copySystem.restype = ctypes.c_int
    self.obj.oms_delete.argtypes = [ctypes.c_char_p]
    self.obj.oms_delete.restype = ctypes.c_int
    self.obj.oms_deleteConnection.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_deleteConnection.restype = ctypes.c_int
    self.obj.oms_deleteConnectorFromBus.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_deleteConnectorFromBus.restype = ctypes.c_int
    self.obj.oms_deleteConnectorFromTLMBus.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_deleteConnectorFromTLMBus.restype = ctypes.c_int
    self.obj.oms_export.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_export.restype = ctypes.c_int
    self.obj.oms_exportDependencyGraphs.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_exportDependencyGraphs.restype = ctypes.c_int
    self.obj.oms_getBoolean.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_bool)]
    self.obj.oms_getBoolean.restype = ctypes.c_int
    self.obj.oms_getFixedStepSize.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_double)]
    self.obj.oms_getFixedStepSize.restype = ctypes.c_int
    self.obj.oms_getInteger.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_int)]
    self.obj.oms_getInteger.restype = ctypes.c_int
    self.obj.oms_getReal.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_double)]
    self.obj.oms_getReal.restype = ctypes.c_int
    self.obj.oms_getSolver.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_int)]
    self.obj.oms_getSolver.restype = ctypes.c_int
    self.obj.oms_getStartTime.argtypes = [ctypes.c_char_p]
    self.obj.oms_getStartTime.restype = ctypes.c_int
    self.obj.oms_getStopTime.argtypes = [ctypes.c_char_p]
    self.obj.oms_getStopTime.restype = ctypes.c_int
    self.obj.oms_getSystemType.argtypes = [ctypes.c_char_p]
    self.obj.oms_getSystemType.restype = ctypes.c_int
    self.obj.oms_getVariableStepSize.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double)]
    self.obj.oms_getVariableStepSize.restype = ctypes.c_int
    self.obj.oms_getVersion.argtypes = None
    self.obj.oms_getVersion.restype = ctypes.c_char_p
    self.obj.oms_importFile.argtypes = [ctypes.c_char_p]
    self.obj.oms_importFile.restype = ctypes.c_int
    self.obj.oms_importString.argtypes = [ctypes.c_char_p]
    self.obj.oms_importString.restype = ctypes.c_int
    self.obj.oms_initialize.argtypes = [ctypes.c_char_p]
    self.obj.oms_initialize.restype = ctypes.c_int
    self.obj.oms_instantiate.argtypes = [ctypes.c_char_p]
    self.obj.oms_instantiate.restype = ctypes.c_int
    self.obj.oms_list.argtypes = [ctypes.c_char_p]
    self.obj.oms_list.restype = ctypes.c_int
    self.obj.oms_listUnconnectedConnectors.argtypes = [ctypes.c_char_p]
    self.obj.oms_listUnconnectedConnectors.restype = ctypes.c_int
    self.obj.oms_newModel.argtypes = [ctypes.c_char_p]
    self.obj.oms_newModel.restype = ctypes.c_int
    self.obj.oms_parseModelName.argtypes = [ctypes.c_char_p]
    self.obj.oms_parseModelName.restype = ctypes.c_int
    self.obj.oms_removeSignalsFromResults.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_removeSignalsFromResults.restype = ctypes.c_int
    self.obj.oms_rename.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_rename.restype = ctypes.c_int
    self.obj.oms_reset.argtypes = [ctypes.c_char_p]
    self.obj.oms_reset.restype = ctypes.c_int
    self.obj.oms_setBoolean.argtypes = [ctypes.c_char_p, ctypes.c_bool]
    self.obj.oms_setBoolean.restype = ctypes.c_int
    self.obj.oms_setCommandLineOption.argtypes = [ctypes.c_char_p]
    self.obj.oms_setCommandLineOption.restype = ctypes.c_int
    self.obj.oms_setFixedStepSize.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_setFixedStepSize.restype = ctypes.c_int
    self.obj.oms_setInteger.argtypes = [ctypes.c_char_p, ctypes.c_int]
    self.obj.oms_setInteger.restype = ctypes.c_int
    self.obj.oms_setLogFile.argtypes = [ctypes.c_char_p]
    self.obj.oms_setLogFile.restype = ctypes.c_int
    self.obj.oms_setLoggingInterval.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_setLoggingInterval.restype = ctypes.c_int
    self.obj.oms_setLoggingLevel.argtypes = [ctypes.c_int]
    self.obj.oms_setLoggingLevel.restype = ctypes.c_int
    self.obj.oms_setMaxLogFileSize.argtypes = [ctypes.c_ulong]
    self.obj.oms_setMaxLogFileSize.restype = None
    self.obj.oms_setReal.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_setReal.restype = ctypes.c_int
    self.obj.oms_setRealInputDerivative.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_setRealInputDerivative.restype = ctypes.c_int
    self.obj.oms_setResultFile.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int]
    self.obj.oms_setResultFile.restype = ctypes.c_int
    self.obj.oms_setSignalFilter.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_setSignalFilter.restype = ctypes.c_int
    self.obj.oms_setSolver.argtypes = [ctypes.c_char_p, ctypes.c_int]
    self.obj.oms_setSolver.restype = ctypes.c_int
    self.obj.oms_setStartTime.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_setStartTime.restype = ctypes.c_int
    self.obj.oms_setStopTime.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_setStopTime.restype = ctypes.c_int
    self.obj.oms_setTempDirectory.argtypes = [ctypes.c_char_p]
    self.obj.oms_setTempDirectory.restype = ctypes.c_int
    self.obj.oms_setTLMPositionAndOrientation.argtypes = [ctypes.c_char_p, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double]
    self.obj.oms_setTLMPositionAndOrientation.restype = ctypes.c_int
    self.obj.oms_setTLMSocketData.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
    self.obj.oms_setTLMSocketData.restype = ctypes.c_int
    self.obj.oms_setTolerance.argtypes = [ctypes.c_char_p, ctypes.c_double, ctypes.c_double]
    self.obj.oms_setTolerance.restype = ctypes.c_int
    self.obj.oms_setVariableStepSize.argtypes = [ctypes.c_char_p, ctypes.c_double, ctypes.c_double, ctypes.c_double]
    self.obj.oms_setVariableStepSize.restype = ctypes.c_int
    self.obj.oms_setWorkingDirectory.argtypes = [ctypes.c_char_p]
    self.obj.oms_setWorkingDirectory.restype = ctypes.c_int
    self.obj.oms_simulate.argtypes = [ctypes.c_char_p]
    self.obj.oms_simulate.restype = ctypes.c_int
    self.obj.oms_stepUntil.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_stepUntil.restype = ctypes.c_int
    self.obj.oms_terminate.argtypes = [ctypes.c_char_p]
    self.obj.oms_terminate.restype = ctypes.c_int

  def getVersion(self):
    return self.checkstring(self.obj.oms_getVersion())
  def setLoggingLevel(self, level):
    return self.obj.oms_setLoggingLevel(level)
  def setTolerance(self, cref, absoluteTolerance, relativeTolerance):
    return self.obj.oms_setTolerance(self.checkstring(cref), absoluteTolerance, relativeTolerance)
  def setVariableStepSize(self, cref, initialStepSize, minimumStepSize, maximumStepSize):
    return self.obj.oms_setVariableStepSize(self.checkstring(cref), initialStepSize, minimumStepSize, maximumStepSize)
  def getSystemType(self, cref):
    type = ctypes.c_int()
    status = self.obj.oms_getSystemType(self.checkstring(cref), ctypes.byref(type))
    return [status, type.value]
  def getVariableStepSize(self, cref):
    initialStepSize = ctypes.c_double()
    minimumStepSize = ctypes.c_double()
    maximumStepSize = ctypes.c_double()
    status = self.obj.oms_getVariableStepSize(self.checkstring(cref), ctypes.byref(initialStepSize), ctypes.byref(minimumStepSize), ctypes.byref(maximumStepSize))
    return [initialStepSize.value, minimumStepSize.value, maximumStepSize.value, status]
  def setLogFile(self, filename):
    return self.obj.oms_setLogFile(self.checkstring(filename))
  def setLoggingInterval(self, cref, loggingInterval):
    return self.obj.oms_setLoggingInterval(self.checkstring(cref), loggingInterval)
  def setMaxLogFileSize(self, size):
    return self.obj.oms_setMaxLogFileSize(size)
  def setTempDirectory(self, newTempDir):
    return self.obj.oms_setTempDirectory(self.checkstring(newTempDir))
  def setWorkingDirectory(self, path):
    return self.obj.oms_setWorkingDirectory(self.checkstring(path))
  def newModel(self, cref):
    return self.obj.oms_newModel(self.checkstring(cref))
  def rename(self, cref, newcref):
    return self.obj.oms_rename(self.checkstring(cref), self.checkstring(newcref))
  def delete(self, cref):
    return self.obj.oms_delete(self.checkstring(cref))
  def export(self, cref, filename):
    return self.obj.oms_export(self.checkstring(cref), self.checkstring(filename))
  def importFile(self, filename):
    contents = ctypes.c_char_p()
    status = self.obj.oms_importFile(self.checkstring(filename), ctypes.byref(contents))
    return [status, self.checkstring(contents.value)]
  def list(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms_list(self.checkstring(ident), ctypes.byref(contents))
    return [status, self.checkstring(contents.value)]
  def listUnconnectedConnectors(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms_listUnconnectedConnectors(self.checkstring(ident), ctypes.byref(contents))
    return [status, self.checkstring(contents.value)]
  def parseModelName(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms_parseModelName(self.checkstring(ident), ctypes.byref(contents))
    return [status, self.checkstring(contents.value)]
  def importString(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms_importString(self.checkstring(ident), ctypes.byref(contents))
    return [status, self.checkstring(contents.value)]
  def addSystem(self, ident, type):
    return self.obj.oms_addSystem(self.checkstring(ident), type)
  def copySystem(self, source, target):
    return self.obj.oms_copySystem(self.checkstring(source), self.checkstring(target))
  def addSubModel(self, cref, fmuPath):
    return self.obj.oms_addSubModel(self.checkstring(cref), self.checkstring(fmuPath))
  def addConnector(self, cref, causality, type):
    return self.obj.oms_addConnector(self.checkstring(cref), causality, type)
  def setCommandLineOption(self, cmd):
    return self.obj.oms_setCommandLineOption(self.checkstring(cmd))
  def addConnection(self, crefA, crefB):
    return self.obj.oms_addConnection(self.checkstring(crefA), self.checkstring(crefB))
  def deleteConnection(self, crefA, crefB):
    return self.obj.oms_deleteConnection(self.checkstring(crefA), self.checkstring(crefB))
  def addBus(self, crefA):
    return self.obj.oms_addBus(self.checkstring(crefA))
  def addConnectorToBus(self, busCref, connectorCref):
    return self.obj.oms_addConnectorToBus(self.checkstring(busCref), self.checkstring(connectorCref))
  def deleteConnectorFromBus(self, busCref, connectorCref):
    return self.obj.oms_deleteConnectorFromBus(self.checkstring(busCref), self.checkstring(connectorCref))
  def addTLMBus(self, cref, domain, dimensions, interpolation):
    return self.obj.oms_addTLMBus(self.checkstring(cref), self.checkstring(domain), dimensions, interpolation)
  def addConnectorToTLMBus(self, busCref, connectorCref, type):
    return self.obj.oms_addConnectorToTLMBus(self.checkstring(busCref), self.checkstring(connectorCref), self.checkstring(type))
  def deleteConnectorFromTLMBus(self, busCref, connectorCref):
    return self.obj.oms_deleteConnectorFromTLMBus(self.checkstring(busCref), self.checkstring(connectorCref))
  def addTLMConnection(self, crefA, crefB, delay, alpha, linearimpedance, angularimpedance):
    return self.obj.oms_addTLMConnection(self.checkstring(crefA), self.checkstring(crefB), delay, alpha, linearimpedance, angularimpedance)
  def compareSimulationResults(self, filenameA, filenameB, var, relTol, absTol):
    return self.obj.oms_compareSimulationResults(self.checkstring(filenameA), self.checkstring(filenameB), self.checkstring(var), relTol, absTol)
  def addExternalModel(self, cref, path, startscript):
    return self.obj.oms_addExternalModel(self.checkstring(cref), self.checkstring(path), self.checkstring(startscript))
  def instantiate(self, cref):
    return self.obj.oms_instantiate(self.checkstring(cref))
  def initialize(self, cref):
    return self.obj.oms_initialize(self.checkstring(cref))
  def simulate(self, cref):
    return self.obj.oms_simulate(self.checkstring(cref))
  def stepUntil(self, cref, stopTime):
    return self.obj.oms_stepUntil(self.checkstring(cref), stopTime)
  def terminate(self, cref):
    return self.obj.oms_terminate(self.checkstring(cref))
  def reset(self, cref):
    return self.obj.oms_reset(self.checkstring(cref))
  def setTLMSocketData(self, cref, address, managerPort, monitorPort):
    return self.obj.oms_setTLMSocketData(self.checkstring(cref), self.checkstring(address), managerPort, monitorPort)
  def setTLMPositionAndOrientation(self, cref, x1, x2, x3, A11, A12, A13, A21, A22, A23, A31, A32, A33):
    return self.obj.oms_setTLMPositionAndOrientation(self.checkstring(cref), x1, x2, x3, A11, A12, A13, A21, A22, A23, A31, A32, A33)
  def exportDependencyGraphs(self, cref, initialization, simulation):
    return self.obj.oms_exportDependencyGraphs(self.checkstring(cref), self.checkstring(initialization), self.checkstring(simulation))
  def getReal(self, cref):
    value = ctypes.c_double()
    status = self.obj.oms_getReal(self.checkstring(cref), ctypes.byref(value))
    return [value.value, status]
  def getInteger(self, cref):
    value = ctypes.c_int()
    status = self.obj.oms_getInteger(self.checkstring(cref), ctypes.byref(value))
    return [value.value, status]
  def getBoolean(self, cref):
    value = ctypes.c_bool()
    status = self.obj.oms_getBoolean(self.checkstring(cref), ctypes.byref(value))
    return [value.value, status]
  def getFixedStepSize(self, cref):
    value = ctypes.c_double()
    status = self.obj.oms_getFixedStepSize(self.checkstring(cref), ctypes.byref(value))
    return [value.value, status]
  def getSolver(self, cref):
    value = ctypes.c_int()
    status = self.obj.oms_getSolver(self.checkstring(cref), ctypes.byref(value))
    return [value.value, status]
  def setReal(self, signal, value):
    return self.obj.oms_setReal(self.checkstring(signal), value)
  def setRealInputDerivative(self, signal, value):
    return self.obj.oms_setRealInputDerivative(self.checkstring(signal), value)
  def setInteger(self, signal, value):
    return self.obj.oms_setInteger(self.checkstring(signal), value)
  def setBoolean(self, signal, value):
    return self.obj.oms_setBoolean(self.checkstring(signal), value)
  def setResultFile(self, cref, filename, bufferSize):
    return self.obj.oms_setResultFile(self.checkstring(cref), self.checkstring(filename), bufferSize)
  def setSignalFilter(self, cref, regex):
    return self.obj.oms_setSignalFilter(self.checkstring(cref), self.checkstring(regex))
  def setSolver(self, cref, solver):
    return self.obj.oms_setSolver(self.checkstring(cref), solver)
  def addSignalsToResults(self, cref, regex):
    return self.obj.oms_addSignalsToResults(self.checkstring(cref), self.checkstring(regex))
  def removeSignalsFromResults(self, cref, regex):
    return self.obj.oms_removeSignalsFromResults(self.checkstring(cref), self.checkstring(regex))
  def getStartTime(self, cref):
    startTime = ctypes.c_double()
    status = self.obj.oms_getStartTime(self.checkstring(cref), ctypes.byref(startTime))
    return [startTime.value, status]
  def setStartTime(self, cref, startTime):
    return self.obj.oms_setStartTime(self.checkstring(cref), startTime)
  def getStopTime(self, cref):
    stopTime = ctypes.c_double()
    status = self.obj.oms_getStopTime(self.checkstring(cref), ctypes.byref(stopTime))
    return [stopTime.value, status]
  def setStopTime(self, cref, stopTime):
    return self.obj.oms_setStopTime(self.checkstring(cref), stopTime)
  def setFixedStepSize(self, cref, stepSize):
    return self.obj.oms_setFixedStepSize(self.checkstring(cref), stepSize)
  def checkstring(self, ident):
    if isinstance(ident, str):
       ident = ident.encode()
    return ident
