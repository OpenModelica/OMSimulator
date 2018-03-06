from ctypes import cdll
import ctypes, sys

class OMSimulator:
  def __init__(self):
    if (sys.platform == 'win32'):
      self.obj=cdll.LoadLibrary("OMSimulatorLib.dll")
    else:
      self.obj=cdll.LoadLibrary("libOMSimulatorLib.so")
    self.setCtypesArguments()
    self.setResultTypes()

  def setResultTypes(self):
    self.obj.oms_compareSimulationResults.restype = ctypes.c_int
    self.obj.oms_getBoolean.restype = ctypes.c_int
    self.obj.oms_getCurrentTime.restype = ctypes.c_double
    self.obj.oms_getInteger.restype = ctypes.c_int
    self.obj.oms_getInterfaceName.restype = ctypes.c_char_p
    self.obj.oms_getInterfaceVariable.restype = ctypes.c_char_p
    self.obj.oms_getMaxIterations.restype = ctypes.c_int
    self.obj.oms_getNumberOfInterfaces.restype = ctypes.c_int
    self.obj.oms_getReal.restype = ctypes.c_double
    self.obj.oms_getVersion.restype = ctypes.c_char_p
    self.obj.oms_loadModel.restype = ctypes.c_void_p
    self.obj.oms_newModel.restype = ctypes.c_void_p

  def setCtypesArguments(self):
    self.obj.oms_addConnection.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_compareSimulationResults.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_double, ctypes.c_double]
    self.obj.oms_describe.argtypes = [ctypes.c_void_p]
    self.obj.oms_doSteps.argtypes = [ctypes.c_void_p, ctypes.c_int]
    self.obj.oms_exportCompositeStructure.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    self.obj.oms_exportDependencyGraph.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    self.obj.oms_exportXML.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    self.obj.oms_getBoolean.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    self.obj.oms_getCurrentTime.argtypes = [ctypes.c_void_p]
    self.obj.oms_getInteger.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    self.obj.oms_getInterfaceCausality.argtypes = [ctypes.c_void_p, ctypes.c_int]
    self.obj.oms_getInterfaceName.argtypes = [ctypes.c_void_p, ctypes.c_int]
    self.obj.oms_getInterfaceVariable.argtypes = [ctypes.c_void_p, ctypes.c_int]
    self.obj.oms_getMaxIterations.argtypes = [ctypes.c_void_p]
    self.obj.oms_getNumberOfInterfaces.argtypes = [ctypes.c_void_p]
    self.obj.oms_getReal.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    self.obj.oms_importXML.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    self.obj.oms_initialize.argtypes = [ctypes.c_void_p]
    self.obj.oms_instantiateFMU.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_loadModel.argtypes = [ctypes.c_char_p]
    self.obj.oms_reset.argtypes = [ctypes.c_void_p]
    self.obj.oms_setBoolean.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]
    self.obj.oms_setCommunicationInterval.argtypes = [ctypes.c_void_p, ctypes.c_double]
    self.obj.oms_setInteger.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int]
    self.obj.oms2_setLogFile.argtypes = [ctypes.c_char_p]
    self.obj.oms_setMaxIterations.argtypes = [ctypes.c_void_p, ctypes.c_int]
    self.obj.oms_setReal.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_setResultFile.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    self.obj.oms_setSolverMethod.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_setStartTime.argtypes = [ctypes.c_void_p, ctypes.c_double]
    self.obj.oms_setStopTime.argtypes = [ctypes.c_void_p, ctypes.c_double]
    self.obj.oms2_setTempDirectory.argtypes = [ctypes.c_char_p]
    self.obj.oms_setTolerance.argtypes = [ctypes.c_void_p, ctypes.c_double]
    self.obj.oms_setVariableFilter.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms2_setWorkingDirectory.argtypes =[ctypes.c_char_p]
    self.obj.oms_simulate.argtypes = [ctypes.c_void_p]
    self.obj.oms_stepUntil.argtypes = [ctypes.c_void_p, ctypes.c_double]
    self.obj.oms_terminate.argtypes = [ctypes.c_void_p]
    self.obj.oms_unload.argtypes = [ctypes.c_void_p]

  def newModel(self):
    return self.obj.oms_newModel()

  def loadModel(self, filename):
    return self.obj.oms_loadModel(str.encode(filename))

  def instantiateFMU(self, model, filename, instanceName):
    self.obj.oms_instantiateFMU(model, str.encode(filename), str.encode(instanceName))

  def instantiateTable(self, model, filename, instanceName):
    self.obj.oms_instantiateTable(model, str.encode(filename), str.encode(instanceName))

  def describe(self, model):
    self.obj.oms_describe(model)

  def initialize(self, model):
    self.obj.oms_initialize(model)

  def terminate(self, model):
    self.obj.oms_terminate(model)

  def unload(self, model):
    self.obj.oms_unload(model)

  def reset(self, model):
    self.obj.oms_reset(model)

  def simulate(self, model):
    self.obj.oms_simulate(model)

  def addConnection(self, model, fromconnection, toconnection):
    self.obj.oms_addConnection(model, str.encode(fromconnection), str.encode(toconnection))

  def importXML(self, model, filename):
    self.obj.oms_importXML(model, str.encode(filename))

  def exportXML(self, model, filename):
    self.obj.oms_exportXML(model, str.encode(filename))

  def exportDependencyGraph(self, model, filename):
    self.obj.oms_exportDependencyGraph(model, str.encode(filename))

  def exportCompositeStructure(self, model, filename):
    self.obj.oms_exportCompositeStructure(model, str.encode(filename))

  def getCurrentTime(self, model):
    time=ctypes.c_double()
    self.obj.oms_getCurrentTime(model, ctypes.byref(time))
    return time.value

  def doSteps(self, model, numberOfSteps):
    self.obj.oms_doSteps(model, numberOfSteps)

  def stepUntil(self, model, timeValue):
    self.obj.oms_stepUntil(model, timeValue)

  def getVersion(self):
    return self.obj.oms_getVersion()

  def getBoolean(self, model, var):
    return self.obj.oms_getBoolean(model, str.encode(var))

  def getInteger(self, model, var):
    return self.obj.oms_getInteger(model, str.encode(var))

  def getMaxIterations(self, model):
    return self.obj.oms_getMaxIterations(model)

  def setMaxIterations(self, model, maxIterations):
    self.obj.oms_setMaxIterations(model, maxIterations)

  def getReal(self, model, var):
    return self.obj.oms_getReal(model, str.encode(var))

  def setBoolean(self, model, var, value):
    self.obj.oms_setBoolean(model, str.encode(var), value)

  def setInteger(self, model, var, value):
    self.obj.oms_setInteger(model, str.encode(var), value)

  def setReal(self, model, var, value):
    self.obj.oms_setReal(model, str.encode(var), value)

  def setResultFile(self, model, filename):
    self.obj.oms_setResultFile(model, str.encode(filename))

  def setTempDirectory(self, filename):
    self.obj.oms2_setTempDirectory(str.encode(filename))

  def setWorkingDirectory(self, path):
    self.obj.oms2_setWorkingDirectory(str.encode(path))

  def setStartTime(self, model, startTime):
    self.obj.oms_setStartTime(model, startTime)

  def setStopTime(self, model, stopTime):
    self.obj.oms_setStopTime(model, stopTime)

  def setTolerance(self, model, tolerance):
    self.obj.oms_setTolerance(model, tolerance)

  def setCommunicationInterval(self, model, CommunicationInterval):
    self.obj.oms_setCommunicationInterval(model, CommunicationInterval)

  def setSolverMethod(self, model, instanceName, method):
    self.obj.oms_setSolverMethod(model, str.encode(instanceName), str.encode(method))

  def setLogFile(self, filename):
    self.obj.oms2_setLogFile(str.encode(filename))

  def compareSimulationResults(self, filenameA, filenameB, var, relTol, absTol):
    return self.obj.oms_compareSimulationResults(str.encode(filenameA), str.encode(filenameB), str.encode(var), relTol, absTol)

  def setVariableFilter(self, model, instanceFilter, variableFilter):
    self.obj.oms_setVariableFilter(model, str.encode(instanceFilter), str.encode(variableFilter))

  def getNumberOfInterfaces(self, model):
    return self.obj.oms_getNumberOfInterfaces(model)

  def getInterfaceCausality(self, model, idx):
    self.obj.oms_getInterfaceCausality(model, idx)

  def getInterfaceName(self, model, idx):
    return self.obj.oms_getInterfaceName(model, idx)

  def getInterfaceVariable(self, model, idx):
    return self.obj.oms_getInterfaceVariable(model, idx)
