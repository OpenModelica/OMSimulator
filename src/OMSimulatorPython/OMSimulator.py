from ctypes import cdll
import ctypes, sys

class OMSimulator:
  def __init__(self):
    if (sys.platform == 'win32'):
      self.obj=cdll.LoadLibrary("OMSimulatorLib.dll")
    else:
      self.obj=cdll.LoadLibrary("libOMSimulatorLib.so")

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

  def addConnection(self, cref, conA, conB):
    return self.obj.oms2_addConnection(str.encode(cref), str.encode(conA), str.encode(conB))
  def addFMU(self, modelIdent, fmuPath, fmuIdent):
    return self.obj.oms2_addFMU(str.encode(modelIdent), str.encode(fmuPath), str.encode(fmuIdent))
  def addSolver(self, modelIdent, solverIdent, method):
    return self.obj.oms2_addSolver(str.encode(modelIdent), str.encode(solverIdent), str.encode(method))
  def addTable(self, modelIdent, tablePath, tableIdent):
    return self.obj.oms2_addTable(str.encode(modelIdent), str.encode(tablePath), str.encode(tableIdent))
  def addSignalsToResults(self, cref, regex):
    return self.obj.oms2_addSignalsToResults(str.encode(cref), str.encode(regex))
  def compareSimulationResults(self, filenameA, filenameB, var, relTol, absTol):
    return self.obj.oms2_compareSimulationResults(str.encode(filenameA), str.encode(filenameB), str.encode(var), relTol, absTol)
  def deleteConnection(self, cref, conA, conB):
    return self.obj.oms2_deleteConnection(str.encode(cref), str.encode(conA), str.encode(conB))
  def deleteSubModel(self, modelIdent, subModelIdent):
    return self.obj.oms2_deleteSubModel(str.encode(modelIdent), str.encode(subModelIdent))
  def describe(self, cref):
    return self.obj.oms2_describe(str.encode(cref))
  def doSteps(self, ident, numberOfSteps):
    return self.obj.oms2_doSteps(str.encode(ident), numberOfSteps)
  def exists(self, cref):
    return self.obj.oms2_exists(str.encode(cref))
  def exportCompositeStructure(self, cref, filename):
    return self.obj.oms2_exportCompositeStructure(str.encode(cref), str.encode(filename))
  def exportDependencyGraphs(self, cref, initialization, simulation):
    return self.obj.oms2_exportDependencyGraphs(str.encode(cref), str.encode(initialization), str.encode(simulation))
  def getBoolean(self, signal):
    value = ctypes.c_bool()
    status = self.obj.oms2_getBoolean(str.encode(signal), ctypes.byref(value))
    return [status, value.value]
  def getBooleanParameter(self, signal):
    value = ctypes.c_bool()
    status = self.obj.oms2_getBooleanParameter(str.encode(signal), ctypes.byref(value))
    return [status, value.value]
  def getCurrentTime(self, ident):
    time = ctypes.c_double()
    status = self.obj.oms2_getCurrentTime(str.encode(ident), ctypes.byref(time))
    return [status, time.value]
  def getSubModelPath(self, cref):
    path = ctypes.c_char_p()
    status = self.obj.oms2_getSubModelPath(str.encode(cref), ctypes.byref(path))
    return [status, path.value]
  def getInteger(self, signal):
    value = ctypes.c_int()
    status = self.obj.oms2_getInteger(str.encode(signal), ctypes.byref(value))
    return [status, value.value]
  def getIntegerParameter(self, signal):
    value = ctypes.c_int()
    status = self.obj.oms2_getIntegerParameter(str.encode(signal), ctypes.byref(value))
    return [status, value.value]
  def getReal(self, signal):
    value = ctypes.c_double()
    status = self.obj.oms2_getReal(str.encode(signal), ctypes.byref(value))
    return [status, value.value]
  def getRealParameter(self, signal):
    value = ctypes.c_double()
    status = self.obj.oms2_getRealParameter(str.encode(signal), ctypes.byref(value))
    return [status, value.value]
  def getVersion(self):
    return self.obj.oms2_getVersion()
  def initialize(self, ident):
    return self.obj.oms2_initialize(str.encode(ident))
  def loadModel(self, filename):
    ident = ctypes.c_char_p()
    status = self.obj.oms2_loadModel(str.encode(filename), ctypes.byref(ident))
    return [status, ident.value]
  def parseString(self, contents):
    ident = ctypes.c_char_p()
    status = self.obj.oms2_parseString(str.encode(contents), ctypes.byref(ident))
    self.obj.oms2_freeMemory(ident)
    return [status, ident.value]
  def loadString(self, contents):
    ident = ctypes.c_char_p()
    status = self.obj.oms2_loadString(str.encode(contents), ctypes.byref(ident))
    return [status, ident.value]
  def newFMIModel(self, ident):
    return self.obj.oms2_newFMIModel(str.encode(ident))
  def rename(self, identOld, identNew):
    return self.obj.oms2_rename(str.encode(identOld), str.encode(identNew))
  def removeSignalsFromResults(self, cref, regex):
    return self.obj.oms2_removeSignalsFromResults(str.encode(cref), str.encode(regex))
  def reset(self, ident):
    return self.obj.oms2_reset(str.encode(ident))
  def saveModel(self, ident, filename):
    return self.obj.oms2_saveModel(str.encode(ident), str.encode(filename))
  def listModel(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms2_listModel(str.encode(ident), ctypes.byref(contents))
    self.obj.oms2_freeMemory(contents)
    return [status, contents.value]
  def setBoolean(self, signal, value):
    return self.obj.oms2_setBoolean(str.encode(signal), value)
  def setBooleanParameter(self, signal, value):
    return self.obj.oms2_setBooleanParameter(str.encode(signal), value)
  def setCommunicationInterval(self, cref, communicationInterval):
    return self.obj.oms2_setCommunicationInterval(str.encode(cref), communicationInterval)
  def setLoggingInterval(self, cref, loggingInterval):
    return self.obj.oms2_setLoggingInterval(str.encode(cref), loggingInterval)
  def setInteger(self, signal, value):
    return self.obj.oms2_setInteger(str.encode(signal), value)
  def setIntegerParameter(self, signal, value):
    return self.obj.oms2_setIntegerParameter(str.encode(signal), value)
  def setLogFile(self, filename):
    return self.obj.oms2_setLogFile(str.encode(filename))
  def setLoggingLevel(self, logLevel):
    return self.obj.oms2_setLoggingLevel(logLevel)
  def setMasterAlgorithm(self, ident, masterAlgorithm):
    return self.obj.oms2_setMasterAlgorithm(str.encode(ident), str.encode(masterAlgorithm))
  def setReal(self, signal, value):
    return self.obj.oms2_setReal(str.encode(signal), value)
  def setRealParameter(self, signal, value):
    return self.obj.oms2_setRealParameter(str.encode(signal), value)
  def setResultFile(self, cref, filename, bufferSize=1):
    return self.obj.oms2_setResultFile(str.encode(cref), str.encode(filename), bufferSize)
  def getStartTime(self, ident):
    startTime = ctypes.c_double()
    status = self.obj.oms2_getStartTime(str.encode(ident), ctypes.byref(startTime))
    return [status, startTime.value]
  def setStartTime(self, cref, startTime):
    return self.obj.oms2_setStartTime(str.encode(cref), startTime)
  def getStopTime(self, ident):
    stopTime = ctypes.c_double()
    status = self.obj.oms2_getStopTime(str.encode(ident), ctypes.byref(stopTime))
    return [status, stopTime.value]
  def setStopTime(self, cref, stopTime):
    return self.obj.oms2_setStopTime(str.encode(cref), stopTime)
  def setTempDirectory(self, filename):
    return self.obj.oms2_setTempDirectory(str.encode(filename))
  def setWorkingDirectory(self, path):
    return self.obj.oms2_setWorkingDirectory(str.encode(path))
  def simulate(self, ident):
    return self.obj.oms2_simulate(str.encode(ident))
  def stepUntil(self, ident, timeValue):
    return self.obj.oms2_stepUntil(str.encode(ident), timeValue)
  def unloadModel(self, ident):
    return self.obj.oms2_unloadModel(str.encode(ident))
