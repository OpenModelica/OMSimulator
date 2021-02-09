import ctypes
import os
import platform
import sys

from OMSimulator import Scope


class OMSimulator:
  def __init__(self, temp_directory=None):
    ## oms_status_enu_t
    self.status_ok = 0
    self.status_warning = 1
    self.status_discard = 2
    self.status_error = 3
    self.status_fatal = 4
    self.status_pending = 5

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
    self.solver_wc_mav2 = 8

    ## oms_fault_type_enu_t
    self.fault_type_bias = 0
    self.fault_type_gain = 1
    self.fault_type_const = 2

    if temp_directory is not None:
      self.setTempDirectory(temp_directory)

  def __enter__(self):
    return self

  def __exit__(self, type, value, traceback):
    for model in Scope._Scope:
      self.terminate(model)
      self.delete(model)

  @property
  def version(self):
    '''Returns the version string'''
    return self.getVersion()

  def getVersion(self):
    return Scope._capi.getVersion()

  def setLoggingLevel(self, level):
    return Scope._capi.setLoggingLevel(level)
  def setTolerance(self, cref, absoluteTolerance, relativeTolerance):
    return Scope._capi.setTolerance(cref, absoluteTolerance, relativeTolerance)
  def setVariableStepSize(self, cref, initialStepSize, minimumStepSize, maximumStepSize):
    return Scope._capi.setVariableStepSize(cref, initialStepSize, minimumStepSize, maximumStepSize)
  def getSystemType(self, cref):
    return Scope._capi.getSystemType(cref)
  def getVariableStepSize(self, cref):
    return Scope._capi.getVariableStepSize(cref)
  def setLogFile(self, filename):
    return Scope._capi.setLogFile(filename)
  def setLoggingInterval(self, cref, loggingInterval):
    return Scope._capi.setLoggingInterval(cref, loggingInterval)
  def setMaxLogFileSize(self, size):
    return Scope._capi.setMaxLogFileSize(size)
  def setTempDirectory(self, newTempDir):
    return Scope._capi.setTempDirectory(newTempDir)
  def setWorkingDirectory(self, path):
    return Scope._capi.setWorkingDirectory(path)
  def newModel(self, cref):
    status = Scope._capi.newModel(cref)
    if status == self.status_ok:
        Scope._Scope.append(cref)
    return status
  def rename(self, cref, newcref):
    return Scope._capi.rename(cref, newcref)
  def delete(self, cref):
    status = Scope._capi.delete(cref)
    if status == self.status_ok:
        Scope._Scope = [model for model in Scope._Scope if model != cref]
    return status
  def export(self, cref, filename):
    return Scope._capi.export(cref, filename)
  def importFile(self, filename):
    return Scope._capi.importFile(filename)
  def importSnapshot(self, ident, snapshot):
    status = Scope._capi.importSnapshot(ident, snapshot)
    return status
  def list(self, ident):
    return Scope._capi.list(ident)
  def exportSnapshot(self, ident):
    return Scope._capi.exportSnapshot(ident)
  def exportSSVTemplate(self, ident, filename):
    return Scope._capi.exportSSVTemplate(ident, filename)
  def exportSSMTemplate(self, ident, filename):
    return Scope._capi.exportSSMTemplate(ident, filename)
  def listUnconnectedConnectors(self, ident):
    return Scope._capi.listUnconnectedConnectors(ident)
  def loadSnapshot(self, ident, snapshot):
    status = Scope._capi.loadSnapshot(ident, snapshot)
    return status
  def parseModelName(self, ident):
    return Scope._capi.parseModelName(ident)
  def addSystem(self, ident, type):
    return Scope._capi.addSystem(ident, type)
  def copySystem(self, source, target):
    return Scope._capi.copySystem(source, target)
  def addSubModel(self, cref, fmuPath):
    return Scope._capi.addSubModel(cref, fmuPath)
  def addConnector(self, cref, causality, type):
    return Scope._capi.addConnector(cref, causality, type)
  def setCommandLineOption(self, cmd):
    return Scope._capi.setCommandLineOption(cmd)
  def addConnection(self, crefA, crefB):
    return Scope._capi.addConnection(crefA, crefB)
  def deleteConnection(self, crefA, crefB):
    return Scope._capi.deleteConnection(crefA, crefB)
  def addBus(self, crefA):
    return Scope._capi.addBus(crefA)
  def addConnectorToBus(self, busCref, connectorCref):
    return Scope._capi.addConnectorToBus(busCref, connectorCref)
  def deleteConnectorFromBus(self, busCref, connectorCref):
    return Scope._capi.deleteConnectorFromBus(busCref, connectorCref)
  def addTLMBus(self, cref, domain, dimensions, interpolation):
    return Scope._capi.addTLMBus(cref, domain, dimensions, interpolation)
  def addConnectorToTLMBus(self, busCref, connectorCref, type):
    return Scope._capi.addConnectorToTLMBus(busCref, connectorCref, type)
  def deleteConnectorFromTLMBus(self, busCref, connectorCref):
    return Scope._capi.deleteConnectorFromTLMBus(busCref, connectorCref)
  def addTLMConnection(self, crefA, crefB, delay, alpha, linearimpedance, angularimpedance):
    return Scope._capi.addTLMConnection(crefA, crefB, delay, alpha, linearimpedance, angularimpedance)
  def compareSimulationResults(self, filenameA, filenameB, var, relTol, absTol):
    return Scope._capi.compareSimulationResults(filenameA, filenameB, var, relTol, absTol)
  def addExternalModel(self, cref, path, startscript):
    return Scope._capi.addExternalModel(cref, path, startscript)
  def instantiate(self, cref):
    return Scope._capi.instantiate(cref)
  def initialize(self, cref):
    return Scope._capi.initialize(cref)
  def simulate(self, cref):
    return Scope._capi.simulate(cref)
  def stepUntil(self, cref, stopTime):
    return Scope._capi.stepUntil(cref, stopTime)
  def terminate(self, cref):
    return Scope._capi.terminate(cref)
  def reset(self, cref):
    return Scope._capi.reset(cref)
  def setTLMSocketData(self, cref, address, managerPort, monitorPort):
    return Scope._capi.setTLMSocketData(cref, address, managerPort, monitorPort)
  def setTLMPositionAndOrientation(self, cref, x1, x2, x3, A11, A12, A13, A21, A22, A23, A31, A32, A33):
    return Scope._capi.setTLMPositionAndOrientation(cref, x1, x2, x3, A11, A12, A13, A21, A22, A23, A31, A32, A33)
  def exportDependencyGraphs(self, cref, initialization, event, simulation):
    return Scope._capi.exportDependencyGraphs(cref, initialization, event, simulation)
  def faultInjection(self, cref, faultType, faultValue):
    return Scope._capi.faultInjection(cref, faultType, faultValue)
  def getReal(self, cref):
    [value, status] = Scope._capi.getReal(cref)
    return [value, status]
  def getInteger(self, cref):
    return Scope._capi.getInteger(cref)
  def getBoolean(self, cref):
    return Scope._capi.getBoolean(cref)
  def getFixedStepSize(self, cref):
    return Scope._capi.getFixedStepSize(cref)
  def getSolver(self, cref):
    return Scope._capi.getSolver(cref)
  def setReal(self, signal, value):
    return Scope._capi.setReal(signal, value)
  def setRealInputDerivative(self, signal, value):
    return Scope._capi.setRealInputDerivative(signal, value)
  def setInteger(self, signal, value):
    return Scope._capi.setInteger(signal, value)
  def setBoolean(self, signal, value):
    return Scope._capi.setBoolean(signal, value)
  def setResultFile(self, cref, filename, bufferSize=1):
    return Scope._capi.setResultFile(cref, filename, bufferSize)
  def setSignalFilter(self, cref, regex):
    return Scope._capi.setSignalFilter(cref, regex)
  def setSolver(self, cref, solver):
    return Scope._capi.setSolver(cref, solver)
  def addSignalsToResults(self, cref, regex):
    return Scope._capi.addSignalsToResults(cref, regex)
  def removeSignalsFromResults(self, cref, regex):
    return Scope._capi.removeSignalsFromResults(cref, regex)
  def getStartTime(self, cref):
    return Scope._capi.getStartTime(cref)
  def setStartTime(self, cref, startTime):
    return Scope._capi.setStartTime(cref, startTime)
  def getStopTime(self, cref):
    return Scope._capi.getStopTime(cref)
  def setStopTime(self, cref, stopTime):
    return Scope._capi.setStopTime(cref, stopTime)
  def setFixedStepSize(self, cref, stepSize):
    return Scope._capi.setFixedStepSize(cref, stepSize)
  def getTime(self, cref):
    return Scope._capi.getTime(cref)
