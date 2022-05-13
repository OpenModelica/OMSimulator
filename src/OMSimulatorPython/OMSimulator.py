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

    # map functions
    self.addBus = Scope._capi.addBus
    self.addConnection = Scope._capi.addConnection
    self.addConnector = Scope._capi.addConnector
    self.addConnectorToBus = Scope._capi.addConnectorToBus
    self.addConnectorToTLMBus = Scope._capi.addConnectorToTLMBus
    self.addExternalModel = Scope._capi.addExternalModel
    self.addResources = Scope._capi.addResources
    self.addSignalsToResults = Scope._capi.addSignalsToResults
    self.addSubModel = Scope._capi.addSubModel
    self.addSystem = Scope._capi.addSystem
    self.addTLMBus = Scope._capi.addTLMBus
    self.addTLMConnection = Scope._capi.addTLMConnection
    self.compareSimulationResults = Scope._capi.compareSimulationResults
    self.copySystem = Scope._capi.copySystem
    self.deleteConnection = Scope._capi.deleteConnection
    self.deleteConnectorFromBus = Scope._capi.deleteConnectorFromBus
    self.deleteConnectorFromTLMBus = Scope._capi.deleteConnectorFromTLMBus
    self.deleteResources = Scope._capi.deleteResources
    self.doStep = Scope._capi.doStep
    self.export = Scope._capi.export
    self.exportDependencyGraphs = Scope._capi.exportDependencyGraphs
    self.exportSnapshot = Scope._capi.exportSnapshot
    self.exportSSMTemplate = Scope._capi.exportSSMTemplate
    self.exportSSVTemplate = Scope._capi.exportSSVTemplate
    self.faultInjection = Scope._capi.faultInjection
    self.getBoolean = Scope._capi.getBoolean
    self.getDirectionalDerivative = Scope._capi.getDirectionalDerivative
    self.getFixedStepSize = Scope._capi.getFixedStepSize
    self.getInteger = Scope._capi.getInteger
    self.getReal = Scope._capi.getReal
    self.getResultFile = Scope._capi.getResultFile
    self.getSolver = Scope._capi.getSolver
    self.getStartTime = Scope._capi.getStartTime
    self.getStopTime = Scope._capi.getStopTime
    self.getSystemType = Scope._capi.getSystemType
    self.getTime = Scope._capi.getTime
    self.getVariableStepSize = Scope._capi.getVariableStepSize
    self.getVersion = Scope._capi.getVersion
    self.importFile = Scope._capi.importFile
    self.importSnapshot = Scope._capi.importSnapshot
    self.initialize = Scope._capi.initialize
    self.instantiate = Scope._capi.instantiate
    self.list = Scope._capi.list
    self.listUnconnectedConnectors = Scope._capi.listUnconnectedConnectors
    self.loadSnapshot = Scope._capi.loadSnapshot
    self.newResources = Scope._capi.newResources
    self.reduceSSV = Scope._capi.reduceSSV
    self.referenceResources = Scope._capi.referenceResources
    self.removeSignalsFromResults = Scope._capi.removeSignalsFromResults
    self.rename = Scope._capi.rename
    self.reset = Scope._capi.reset
    self.setBoolean = Scope._capi.setBoolean
    self.setCommandLineOption = Scope._capi.setCommandLineOption
    self.setFixedStepSize = Scope._capi.setFixedStepSize
    self.setInteger = Scope._capi.setInteger
    self.setLogFile = Scope._capi.setLogFile
    self.setLoggingInterval = Scope._capi.setLoggingInterval
    self.setLoggingLevel = Scope._capi.setLoggingLevel
    self.setMaxLogFileSize = Scope._capi.setMaxLogFileSize
    self.setReal = Scope._capi.setReal
    self.setRealInputDerivative = Scope._capi.setRealInputDerivative
    self.setResultFile = Scope._capi.setResultFile
    self.setSolver = Scope._capi.setSolver
    self.setStartTime = Scope._capi.setStartTime
    self.setStopTime = Scope._capi.setStopTime
    self.setTempDirectory = Scope._capi.setTempDirectory
    self.setTLMPositionAndOrientation = Scope._capi.setTLMPositionAndOrientation
    self.setTLMSocketData = Scope._capi.setTLMSocketData
    self.setTolerance = Scope._capi.setTolerance
    self.setUnit = Scope._capi.setUnit
    self.setVariableStepSize = Scope._capi.setVariableStepSize
    self.setWorkingDirectory = Scope._capi.setWorkingDirectory
    self.simulate = Scope._capi.simulate
    self.stepUntil = Scope._capi.stepUntil
    self.terminate = Scope._capi.terminate

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

  def newModel(self, cref):
    status = Scope._capi.newModel(cref)
    if status == self.status_ok:
        Scope._Scope.append(cref)
    return status

  def delete(self, cref):
    status = Scope._capi.delete(cref)
    if status == self.status_ok:
        Scope._Scope = [model for model in Scope._Scope if model != cref]
    return status
