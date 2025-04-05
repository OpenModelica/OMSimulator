import ctypes
import os

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

    self.obj.oms_activateVariant.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_activateVariant.restype = ctypes.c_int
    self.obj.oms_addBus.argtypes = [ctypes.c_char_p]
    self.obj.oms_addBus.restype = ctypes.c_int
    self.obj.oms_addConnection.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_bool]
    self.obj.oms_addConnection.restype = ctypes.c_int
    self.obj.oms_addConnector.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
    self.obj.oms_addConnector.restype = ctypes.c_int
    self.obj.oms_addConnectorToBus.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addConnectorToBus.restype = ctypes.c_int
    self.obj.oms_addResources.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addResources.restype = ctypes.c_int
    self.obj.oms_addSignalsToResults.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addSignalsToResults.restype = ctypes.c_int
    self.obj.oms_addSubModel.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_addSubModel.restype = ctypes.c_int
    self.obj.oms_addSystem.argtypes = [ctypes.c_char_p, ctypes.c_int]
    self.obj.oms_addSystem.restype = ctypes.c_int
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
    self.obj.oms_deleteResources.argtypes = [ctypes.c_char_p]
    self.obj.oms_deleteResources.restype = ctypes.c_int
    self.obj.oms_doStep.argtypes = [ctypes.c_char_p]
    self.obj.oms_doStep.restype = ctypes.c_int
    self.obj.oms_duplicateVariant.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_duplicateVariant.restype = ctypes.c_int
    self.obj.oms_export.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_export.restype = ctypes.c_int
    self.obj.oms_exportDependencyGraphs.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_exportDependencyGraphs.restype = ctypes.c_int
    self.obj.oms_exportSnapshot.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p)]
    self.obj.oms_exportSnapshot.restype = ctypes.c_int
    self.obj.oms_exportSSMTemplate.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_exportSSMTemplate.restype = ctypes.c_int
    self.obj.oms_exportSSVTemplate.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_exportSSVTemplate.restype = ctypes.c_int
    self.obj.oms_freeMemory.argtypes = [ctypes.c_void_p]
    self.obj.oms_freeMemory.restype = None
    self.obj.oms_getBoolean.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_bool)]
    self.obj.oms_getBoolean.restype = ctypes.c_int
    self.obj.oms_getDirectionalDerivative.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_double)]
    self.obj.oms_getDirectionalDerivative.restype = ctypes.c_int
    self.obj.oms_getFixedStepSize.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_double)]
    self.obj.oms_getFixedStepSize.restype = ctypes.c_int
    self.obj.oms_getInteger.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_int)]
    self.obj.oms_getInteger.restype = ctypes.c_int
    self.obj.oms_getModelState.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_int)]
    self.obj.oms_getModelState.restype = ctypes.c_int
    self.obj.oms_getReal.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_double)]
    self.obj.oms_getReal.restype = ctypes.c_int
    self.obj.oms_getResultFile.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p), ctypes.POINTER(ctypes.c_int)]
    self.obj.oms_getResultFile.restype = ctypes.c_int
    self.obj.oms_getSolver.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_int)]
    self.obj.oms_getSolver.restype = ctypes.c_int
    self.obj.oms_getStartTime.argtypes = [ctypes.c_char_p]
    self.obj.oms_getStartTime.restype = ctypes.c_int
    self.obj.oms_getStopTime.argtypes = [ctypes.c_char_p]
    self.obj.oms_getStopTime.restype = ctypes.c_int
    self.obj.oms_getString.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p)]
    self.obj.oms_getString.restype = ctypes.c_int
    self.obj.oms_getSystemType.argtypes = [ctypes.c_char_p]
    self.obj.oms_getSystemType.restype = ctypes.c_int
    self.obj.oms_getState.argtypes = [ctypes.c_char_p]
    self.obj.oms_getState.restype = ctypes.c_int
    self.obj.oms_setState.argtypes = [ctypes.c_char_p]
    self.obj.oms_setState.restype = ctypes.c_int
    self.obj.oms_freeState.argtypes = [ctypes.c_char_p]
    self.obj.oms_freeState.restype = ctypes.c_int
    self.obj.oms_getTime.argtypes = [ctypes.c_char_p]
    self.obj.oms_getTime.restype = ctypes.c_int
    self.obj.oms_getVariableStepSize.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double)]
    self.obj.oms_getVariableStepSize.restype = ctypes.c_int
    self.obj.oms_getVersion.argtypes = None
    self.obj.oms_getVersion.restype = ctypes.c_char_p
    self.obj.oms_importFile.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p)]
    self.obj.oms_importFile.restype = ctypes.c_int
    self.obj.oms_importSnapshot.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p)]
    self.obj.oms_importSnapshot.restype = ctypes.c_int
    self.obj.oms_initialize.argtypes = [ctypes.c_char_p]
    self.obj.oms_initialize.restype = ctypes.c_int
    self.obj.oms_instantiate.argtypes = [ctypes.c_char_p]
    self.obj.oms_instantiate.restype = ctypes.c_int
    self.obj.oms_list.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p)]
    self.obj.oms_list.restype = ctypes.c_int
    self.obj.oms_listVariants.argtypes = [ctypes.c_char_p]
    self.obj.oms_listVariants.restype = ctypes.c_int
    self.obj.oms_listUnconnectedConnectors.argtypes = [ctypes.c_char_p]
    self.obj.oms_listUnconnectedConnectors.restype = ctypes.c_int
    self.obj.oms_loadSnapshot.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_char_p)]
    self.obj.oms_loadSnapshot.restype = ctypes.c_int
    self.obj.oms_newModel.argtypes = [ctypes.c_char_p]
    self.obj.oms_newModel.restype = ctypes.c_int
    self.obj.oms_newResources.argtypes = [ctypes.c_char_p]
    self.obj.oms_newResources.restype = ctypes.c_int
    self.obj.oms_reduceSSV.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_reduceSSV.restype = ctypes.c_int
    self.obj.oms_referenceResources.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_referenceResources.restype = ctypes.c_int
    self.obj.oms_removeSignalsFromResults.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_removeSignalsFromResults.restype = ctypes.c_int
    self.obj.oms_rename.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_rename.restype = ctypes.c_int
    self.obj.oms_replaceSubModel.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_bool, ctypes.POINTER(ctypes.c_int)]
    self.obj.oms_replaceSubModel.restype = ctypes.c_int
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
    self.obj.oms_setSolver.argtypes = [ctypes.c_char_p, ctypes.c_int]
    self.obj.oms_setSolver.restype = ctypes.c_int
    self.obj.oms_setStartTime.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_setStartTime.restype = ctypes.c_int
    self.obj.oms_setStopTime.argtypes = [ctypes.c_char_p, ctypes.c_double]
    self.obj.oms_setStopTime.restype = ctypes.c_int
    self.obj.oms_setString.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_setString.restype = ctypes.c_int
    self.obj.oms_setTempDirectory.argtypes = [ctypes.c_char_p]
    self.obj.oms_setTempDirectory.restype = ctypes.c_int
    self.obj.oms_setTolerance.argtypes = [ctypes.c_char_p, ctypes.c_double, ctypes.c_double]
    self.obj.oms_setTolerance.restype = ctypes.c_int
    self.obj.oms_setUnit.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    self.obj.oms_setUnit.restype = ctypes.c_int
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

    # oms3 api
    self.obj.oms3_modelJson.argtypes = [ctypes.c_char_p]
    self.obj.oms3_modelJson.restype = ctypes.c_void_p
    self.obj.oms3_instantiate.argtypes = [ctypes.c_void_p]
    self.obj.oms3_instantiate.restype = ctypes.c_int
    self.obj.oms3_initialize.argtypes = [ctypes.c_void_p]
    self.obj.oms3_initialize.restype = ctypes.c_int
    self.obj.oms3_simulate.argtypes = [ctypes.c_void_p]
    self.obj.oms3_simulate.restype = ctypes.c_int
    self.obj.oms3_stepUntil.argtypes = [ctypes.c_void_p, ctypes.c_double]
    self.obj.oms3_stepUntil.restype = ctypes.c_int
    self.obj.oms3_terminate.argtypes = [ctypes.c_void_p]
    self.obj.oms3_terminate.restype = ctypes.c_int

  def oms3_modelJson(self, model_json_desc):
    return self.obj.oms3_modelJson(model_json_desc.encode('utf-8'))
  def oms3_instantiate(self, model):
    return self.obj.oms3_instantiate(model)
  def oms3_initialize(self, model):
    return self.obj.oms3_initialize(model)
  def oms3_simulate(self, model):
    return self.obj.oms3_simulate(model)
  def oms3_stepUntil(self, model, stopTime):
    return self.obj.oms3_stepUntil(model, stopTime)
  def oms3_terminate(self, model):
    return self.obj.oms3_terminate(model)

  # oms2 api
  def activateVariant(self, crefA, crefB):
    return self.obj.oms_activateVariant(crefA.encode(), crefB.encode())
  def addBus(self, crefA):
    return self.obj.oms_addBus(crefA.encode())
  def addConnection(self, crefA, crefB, suppressUnit=False):
    return self.obj.oms_addConnection(crefA.encode(), crefB.encode(), suppressUnit)
  def addConnector(self, cref, causality, type_):
    return self.obj.oms_addConnector(cref.encode(), causality, type_)
  def addConnectorToBus(self, busCref, connectorCref):
    return self.obj.oms_addConnectorToBus(busCref.encode(), connectorCref.encode())
  def addResources(self, cref, path):
    return self.obj.oms_addResources(cref.encode(), path.encode())
  def addSignalsToResults(self, cref, regex):
    return self.obj.oms_addSignalsToResults(cref.encode(), regex.encode())
  def addSubModel(self, cref, fmuPath):
    return self.obj.oms_addSubModel(cref.encode(), fmuPath.encode())
  def addSystem(self, ident, type_):
    return self.obj.oms_addSystem(ident.encode(), type_)
  def compareSimulationResults(self, filenameA, filenameB, var, relTol, absTol):
    return self.obj.oms_compareSimulationResults(filenameA.encode(), filenameB.encode(), var.encode(), relTol, absTol)
  def copySystem(self, source, target):
    return self.obj.oms_copySystem(source.encode(), target.encode())
  def delete(self, cref):
    return self.obj.oms_delete(cref.encode())
  def deleteConnection(self, crefA, crefB):
    return self.obj.oms_deleteConnection(crefA.encode(), crefB.encode())
  def deleteConnectorFromBus(self, busCref, connectorCref):
    return self.obj.oms_deleteConnectorFromBus(busCref.encode(), connectorCref.encode())
  def deleteResources(self, crefA):
    return self.obj.oms_deleteResources(crefA.encode())
  def doStep(self, cref):
    return self.obj.oms_doStep(cref.encode())
  def duplicateVariant(self, crefA, crefB):
    return self.obj.oms_duplicateVariant(crefA.encode(), crefB.encode())
  def export(self, cref, filename):
    return self.obj.oms_export(cref.encode(), filename.encode())
  def exportDependencyGraphs(self, cref, initialization, event, simulation):
    return self.obj.oms_exportDependencyGraphs(cref.encode(), initialization.encode(), event.encode(), simulation.encode())
  def exportSnapshot(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms_exportSnapshot(ident.encode(), ctypes.byref(contents))
    contents_ = contents.value.decode('utf-8') if contents.value else None
    self.obj.oms_freeMemory(contents)
    return [contents_, status]
  def exportSSMTemplate(self, ident, filename):
    return self.obj.oms_exportSSMTemplate(ident.encode(), filename.encode())
  def exportSSVTemplate(self, ident, filename):
    return self.obj.oms_exportSSVTemplate(ident.encode(), filename.encode())
  def getBoolean(self, cref):
    value = ctypes.c_bool()
    status = self.obj.oms_getBoolean(cref.encode(), ctypes.byref(value))
    return [value.value, status]
  def getDirectionalDerivative(self, crefA, crefB=""):
    value = ctypes.c_double()
    status = self.obj.oms_getDirectionalDerivative(crefA.encode(), crefB.encode(), ctypes.byref(value))
    return [value.value, status]
  def getFixedStepSize(self, cref):
    value = ctypes.c_double()
    status = self.obj.oms_getFixedStepSize(cref.encode(), ctypes.byref(value))
    return [value.value, status]
  def getInteger(self, cref):
    value = ctypes.c_int()
    status = self.obj.oms_getInteger(cref.encode(), ctypes.byref(value))
    return [value.value, status]
  def getModelState(self, cref):
    value = ctypes.c_int()
    status = self.obj.oms_getModelState(cref.encode(), ctypes.byref(value))
    return [value.value, status]
  def getReal(self, cref):
    value = ctypes.c_double()
    status = self.obj.oms_getReal(cref.encode(), ctypes.byref(value))
    return [value.value, status]
  def getResultFile(self, cref):
    filename = ctypes.c_char_p()
    bufferSize = ctypes.c_int()
    status = self.obj.oms_getResultFile(cref.encode(), ctypes.byref(filename), ctypes.byref(bufferSize))
    return [filename.value.decode('utf-8') if filename.value else None, bufferSize.value, status]
  def getSolver(self, cref):
    value = ctypes.c_int()
    status = self.obj.oms_getSolver(cref.encode(), ctypes.byref(value))
    return [value.value, status]
  def getStartTime(self, cref):
    startTime = ctypes.c_double()
    status = self.obj.oms_getStartTime(cref.encode(), ctypes.byref(startTime))
    return [startTime.value, status]
  def getStopTime(self, cref):
    stopTime = ctypes.c_double()
    status = self.obj.oms_getStopTime(cref.encode(), ctypes.byref(stopTime))
    return [stopTime.value, status]
  def getString(self, cref):
    value = ctypes.c_char_p()
    status = self.obj.oms_getString(cref.encode(), ctypes.byref(value))
    value_ = value.value.decode('utf-8') if value.value else None
    self.obj.oms_freeMemory(value)
    return [value_, status]
  def getSystemType(self, cref):
    type_ = ctypes.c_int()
    status = self.obj.oms_getSystemType(cref.encode(), ctypes.byref(type_))
    return [type_.value, status]
  def getState(self, cref):
    return self.obj.oms_getState(cref.encode())
  def setState(self, cref):
    return self.obj.oms_setState(cref.encode())
  def freeState(self, cref):
    return self.obj.oms_freeState(cref.encode())
  def getTime(self, cref):
    time = ctypes.c_double()
    status = self.obj.oms_getTime(cref.encode(), ctypes.byref(time))
    return [time.value, status]
  def getVariableStepSize(self, cref):
    initialStepSize = ctypes.c_double()
    minimumStepSize = ctypes.c_double()
    maximumStepSize = ctypes.c_double()
    status = self.obj.oms_getVariableStepSize(cref.encode(), ctypes.byref(initialStepSize), ctypes.byref(minimumStepSize), ctypes.byref(maximumStepSize))
    return [initialStepSize.value, minimumStepSize.value, maximumStepSize.value, status]
  def getVersion(self):
    return self.obj.oms_getVersion().decode('utf-8')
  def importFile(self, filename):
    cref = ctypes.c_char_p()
    status = self.obj.oms_importFile(filename.encode(), ctypes.byref(cref))
    return [cref.value.decode('utf-8') if cref.value else None, status]
  def importSnapshot(self, ident, snapshot):
    newCref = ctypes.c_char_p()
    status = self.obj.oms_importSnapshot(ident.encode(), snapshot.encode(), ctypes.byref(newCref))
    return [newCref.value.decode('utf-8') if newCref.value else None, status]
  def initialize(self, cref):
    return self.obj.oms_initialize(cref.encode())
  def instantiate(self, cref):
    return self.obj.oms_instantiate(cref.encode())
  def list(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms_list(ident.encode(), ctypes.byref(contents))
    contents_ = contents.value.decode('utf-8') if contents.value else None
    self.obj.oms_freeMemory(contents)
    return [contents_, status]
  def listVariants(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms_listVariants(ident.encode(), ctypes.byref(contents))
    contents_ = contents.value.decode('utf-8') if contents.value else None
    self.obj.oms_freeMemory(contents)
    return [contents_, status]
  def listUnconnectedConnectors(self, ident):
    contents = ctypes.c_char_p()
    status = self.obj.oms_listUnconnectedConnectors(ident.encode(), ctypes.byref(contents))
    contents_ = contents.value.decode('utf-8') if contents.value else None
    self.obj.oms_freeMemory(contents)
    return [contents_, status]
  def loadSnapshot(self, ident, snapshot):
    newCref = ctypes.c_char_p()
    status = self.obj.oms_loadSnapshot(ident.encode(), snapshot.encode(), ctypes.byref(newCref))
    return [newCref.value.decode('utf-8') if newCref.value else None, status]
  def newModel(self, cref):
    return self.obj.oms_newModel(cref.encode())
  def newResources(self, crefA):
    return self.obj.oms_newResources(crefA.encode())
  def reduceSSV(self, crefA, crefB, crefC, crefD=""):
    return self.obj.oms_reduceSSV(crefA.encode(), crefB.encode(), crefC.encode(), crefD.encode())
  def referenceResources(self, crefA, crefB=""):
    return self.obj.oms_referenceResources(crefA.encode(), crefB.encode())
  def removeSignalsFromResults(self, cref, regex):
    return self.obj.oms_removeSignalsFromResults(cref.encode(), regex.encode())
  def rename(self, cref, newcref):
    return self.obj.oms_rename(cref.encode(), newcref.encode())
  def replaceSubModel(self, cref, fmuPath, dryRun):
    value = ctypes.c_int()
    status = self.obj.oms_replaceSubModel(cref.encode(), fmuPath.encode(), dryRun, ctypes.byref(value))
    return [value.value, status]
  def reset(self, cref):
    return self.obj.oms_reset(cref.encode())
  def setBoolean(self, signal, value):
    return self.obj.oms_setBoolean(signal.encode(), value)
  def setCommandLineOption(self, cmd):
    return self.obj.oms_setCommandLineOption(cmd.encode())
  def setFixedStepSize(self, cref, stepSize):
    return self.obj.oms_setFixedStepSize(cref.encode(), stepSize)
  def setInteger(self, signal, value):
    return self.obj.oms_setInteger(signal.encode(), value)
  def setLogFile(self, filename):
    return self.obj.oms_setLogFile(filename.encode())
  def setLoggingInterval(self, cref, loggingInterval):
    return self.obj.oms_setLoggingInterval(cref.encode(), loggingInterval)
  def setLoggingLevel(self, level):
    return self.obj.oms_setLoggingLevel(level)
  def setMaxLogFileSize(self, size):
    return self.obj.oms_setMaxLogFileSize(size)
  def setReal(self, signal, value):
    return self.obj.oms_setReal(signal.encode(), value)
  def setRealInputDerivative(self, signal, value):
    return self.obj.oms_setRealInputDerivative(signal.encode(), value)
  def setResultFile(self, cref, filename, bufferSize=1):
    return self.obj.oms_setResultFile(cref.encode(), filename.encode(), bufferSize)
  def setSolver(self, cref, solver):
    return self.obj.oms_setSolver(cref.encode(), solver)
  def setStartTime(self, cref, startTime):
    return self.obj.oms_setStartTime(cref.encode(), startTime)
  def setStopTime(self, cref, stopTime):
    return self.obj.oms_setStopTime(cref.encode(), stopTime)
  def setString(self, signal, value):
    return self.obj.oms_setString(signal.encode(), value.encode())
  def setTempDirectory(self, newTempDir):
    return self.obj.oms_setTempDirectory(newTempDir.encode())
  def setTolerance(self, cref, relativeTolerance):
    return self.obj.oms_setTolerance(cref.encode(), relativeTolerance)
  def setUnit(self, signal, value):
    return self.obj.oms_setUnit(signal.encode(), value.encode())
  def setVariableStepSize(self, cref, initialStepSize, minimumStepSize, maximumStepSize):
    return self.obj.oms_setVariableStepSize(cref.encode(), initialStepSize, minimumStepSize, maximumStepSize)
  def setWorkingDirectory(self, path):
    return self.obj.oms_setWorkingDirectory(path.encode())
  def simulate(self, cref):
    return self.obj.oms_simulate(cref.encode())
  def stepUntil(self, cref, stopTime):
    return self.obj.oms_stepUntil(cref.encode(), stopTime)
  def terminate(self, cref):
    return self.obj.oms_terminate(cref.encode())
