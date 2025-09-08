from OMSimulator.capi import Capi, Status
from OMSimulator.cref import CRef
from OMSimulator.system import System
from OMSimulator.values import Values
from OMSimulator.ssm import SSM
from OMSimulator.variable import Causality, SignalType
import json
import tempfile
class InstantiatedModel:
  def __init__(self, json_description, system: System, resources: dict):
    config = json.loads(json_description)
    self.modelName = "model" ## create random name, but we cannot commits test as jenkins will gerate new model name
    self.apiCall = []
    self.mappedCrefs = {}  # Store mapped CRefs associated with their export names
    self.system = system
    self.resources = resources

    status = Capi.setCommandLineOption("--suppressPath=true")
    if status != Status.ok:
      raise RuntimeError(f"Failed to set command line option: {status}")
    status = Capi.setTempDirectory(tempfile.mkdtemp())
    if status != Status.ok:
      raise RuntimeError(f"Failed to set temp directory: {status}")
    # Create a new model
    status = Capi.newModel("model")
    if status != Status.ok:
      raise RuntimeError(f"Failed to create new model: {status}")
    # Add a root system
    status = Capi.addSystem("model.root", 1)  #oms_system_wc
    if status != Status.ok:
      raise RuntimeError(f"Failed to create oms_addSystem: {status}")

    # Begin API generation
    self.apiCall.append(f'oms_newModel("{self.modelName}")')
    self.apiCall.append(f'oms_addSystem("{self.modelName}.root", "oms_system_wc")')

    # Iterate over simulation units
    for unit in config["simulation units"]:
      ## check if unit has solver as key
      solvername = "None"
      if "solver" in unit:
        solvername = unit["solver"]['name']
        solver_path = f"{self.modelName}.root.{solvername}"
        self.apiCall.append(f'oms_addSystem("{solver_path}", "oms_system_sc")')
        status = Capi.addSystem(f"model.root.{solvername}", 2)
        if status != Status.ok:
          raise RuntimeError(f"Failed to add oms_addSystem: {status}")
      else:
        solver_path = f"{self.modelName}.root"

      ## add components
      currentSystem = ""
      for comp in unit["components"]:
        comp_path = ".".join([solver_path] + [comp["name"][-1]])
        currentSystem = ".".join(comp["name"][:-1])
        self.apiCall.append(f'oms_addSubModel("{comp_path}", "{comp["path"]}")')
        status = Capi.addSubModel(comp_path, comp["path"])
        if status != Status.ok:
          raise RuntimeError(f"Failed to add oms_addSubModel: {status}")
        export_name = ".".join(comp["name"])
        #print(f"Setting export name for {comp_path} to {export_name}")
        if not export_name in self.mappedCrefs:
          self.mappedCrefs[export_name] = comp_path
          self.mappedCrefs[".".join(comp["name"][:-1])] = solver_path # map parent system too for connector lookup
        status = Capi.setExportName(comp_path, export_name)  # Set export name if provided
        if status != Status.ok:
          raise RuntimeError(f"Failed to set export name: {status}")

      ## top level connectors:
      if currentSystem == self.system.name:
        self._addConnector(self.system.connectors, self.system.name)

      ## add connectors mapped with currentSystem
      for key, element in self.system.elements.items():
        if isinstance(element, System):
          connector_path = ".".join([self.system.name, str(element.name)])
          if currentSystem == connector_path:
            self._addConnector(element.connectors, connector_path)

      ## add connections
      for connection in unit["connections"]:
        start_element = ".".join(connection['start element'])
        end_element = ".".join(connection['end element'])
        start = self.mappedCrefs[start_element] + f".{connection['start connector']}"
        end = self.mappedCrefs[end_element] + f".{connection['end connector']}"
        self.apiCall.append(f'oms_addConnection("{start}", "{end}")')
        status = Capi.addConnection(start, end)
        if status != Status.ok:
          raise RuntimeError(f"Failed to add oms_addConnection: {status}")

    ## set start values
    self.setStartValues(self.system.value, self.system.name, self.system.parameterMapping)
    ## set start values from ssv files
    self.setStartValuesFromSSV(self.system.parameterResources, self.system.name)
    ## iterate start values from sub-system both inline and ssv files if exist
    self.setStartValuesFromElements(self.system.elements, self.system.name)

    self.apiCall.append(f'oms_instantiate("{self.modelName}")')
    status = Capi.instantiate(self.modelName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to instantiate model: {status}")

  def setStartValuesFromElements(self, elements, systemName):
    for key, element in elements.items():
      new_system_name = ".".join([systemName, str(element.name)])
      self.setStartValues(element.value, new_system_name, element.parameterMapping)
      self.setStartValuesFromSSV(element.parameterResources, new_system_name)

      ## recursive for sub-system
      if isinstance(element, System):
        new_system_name = ".".join([systemName, str(element.name)])
        self.setStartValuesFromElements(element.elements, new_system_name)

  def setStartValuesFromSSV(self, parameterResources, systemName):
    if not parameterResources:
      return
    for resource in parameterResources:
      for (key, mapping) in resource.items():
        if key in self.resources:
          ssv = self.resources.get(key)
          ssm = self.resources.get(mapping, None)  # get the ssm mapping if exist
          self.setStartValues(ssv.value, systemName, ssm)
        else:
          raise KeyError(f"Missing required resource: '{key}'")

  def map_cref(self, system_name, var_str):
    ## map cref with the longest path
    ## e.g. default.sub-system.gain1.k -> model.root.gain1.k
    ## e.g. default.sub-system.gain1.R1.T. -> model.root.solver2.gain1.R1.T
    # Combine system name and variable
    cref = f"{system_name}.{var_str}"
    # Split into parts
    parts = cref.split(".")
    ## map with the longest path
    for i in range(len(parts), 0, -1):
      prefix = ".".join(parts[:i])
      if prefix in self.mappedCrefs:
        mapped_prefix = self.mappedCrefs[prefix]
        validate_path = self.validatePath(mapped_prefix.split("."), parts[i:])
        return validate_path

    raise KeyError(f"No mapping found for {cref}")

  def validatePath(self, mapped_prefix : list, suffix : list):
    ## remove common path from suffix
    ## e.g. mapped_prefix = model.root.solver2.gain1
    ##      suffix = gain1.R1.T
    ##      result = model.root.solver2.gain1.R1.T
    result = []
    for item in suffix:
      if item not in mapped_prefix:
        result.append(item)

    return ".".join(mapped_prefix + result)

  def setStartValues(self, value: Values, systemName: str, ssm: SSM | None):
    if value.empty():
      return

    if systemName not in self.mappedCrefs:
      raise KeyError(f"Missing required key: '{systemName}'")

    ## apply ssm mapping if exist and apply start values from mapped parameters
    if ssm and ssm.mappingEntry:
      for source, targets in ssm.mappingEntry.items():
        if CRef(source) in value.start_values:
          source_value, _, _ = value.start_values[CRef(source)]
          for target in targets:
            value_path = self.map_cref(systemName, str(target))
            self.apply_start_value(value_path, source_value)
    else:
      for key, (source_value, _, _) in value.start_values.items():
        value_path = self.map_cref(systemName, str(key))
        self.apply_start_value(value_path, source_value)

  def apply_start_value(self, value_path:str, value):
    # Determine the variable type
    type, status = Capi.getVariableType(value_path)
    if status != Status.ok:
      raise RuntimeError(f"Failed to get variable type for {value_path}: {status}")

    match SignalType(type):
      case SignalType.Real:  # oms_signal_type_real
        self._setReal(value_path, value)
      case SignalType.Integer:  # oms_signal_type_integer
        self._setInteger(value_path, value)
      case SignalType.Boolean:  # oms_signal_type_boolean
        self._setBoolean(value_path, value)
      case SignalType.String:  # oms_signal_type_string
        self._setString(value_path, value)
      case SignalType.Enumeration:  # oms_signal_type_enumeration
        self._setInteger(value_path, value)  # Treat enumeration as integer
      case _:
        raise TypeError(f"Unsupported type: {type}")

  def _addConnector(self, connectors, systemName):
    for connector in connectors:
      name = [systemName] + [str(connector.name)]
      connector_path = ".".join([self.mappedCrefs[systemName], str(connector.name)])
      self.apiCall.append(f'oms_addConnector("{connector_path}", "{connector.causality}", {connector.signal_type})')
      status = Capi.addConnector(connector_path, connector.causality.value, connector.signal_type.value)
      if status != Status.ok:
        raise RuntimeError(f"Failed to add oms_addConnector:{status}")
      export_name = systemName
      if not export_name in self.mappedCrefs:
        self.mappedCrefs[export_name] = connector_path
      status = Capi.setExportName(connector_path, export_name)  # Set export name if provided
      if status != Status.ok:
        raise RuntimeError(f"Failed to set export name: {status}")

  def dumpApiCalls(self):
    """Returns the generated API calls as a string."""
    return "\n".join(self.apiCall)

  def setValue(self, cref: CRef, value):
    """Sets a value for a specific CRef in the model."""
    name = ".".join(cref.names[:-1])
    if name not in self.mappedCrefs:
      raise KeyError(f"Missing required key: '{name}'")

    value_path = ".".join([self.mappedCrefs[name], cref.names[-1]])

    # Determine the variable type
    type, status = Capi.getVariableType(value_path)
    if status != Status.ok:
      raise RuntimeError(f"Failed to get variable type for {cref}: {status}")

    match SignalType(type):
      case SignalType.Real:  # oms_signal_type_real
        return self._setReal(value_path, value)
      case SignalType.Integer:  # oms_signal_type_integer
        return self._setInteger(value_path, value)
      case SignalType.Boolean:  # oms_signal_type_boolean
        return self._setBoolean(value_path, value)
      case SignalType.String:  # oms_signal_type_string
        return self._setString(value_path, value)
      case SignalType.Enumeration:  # oms_signal_type_enumeration
        return self._setInteger(value_path, value)  # Treat enumeration as integer
      case _:
        raise TypeError(f"Unsupported type: {type}")

  def _setReal(self, mapped_cref: str, value: float):
    self.apiCall.append(f'oms_setReal("{mapped_cref}, {value})')
    status = Capi.setReal(mapped_cref, value) # Get the value from the CAPI
    if status != Status.ok:
      raise RuntimeError(f"Failed to set value for {mapped_cref}: {status}")

  def _setInteger(self, mapped_cref: str, value: int):
    self.apiCall.append(f'oms_setInteger("{mapped_cref}, {value})')
    status = Capi.setInteger(mapped_cref, value) # Get the value from the CAPI
    if status != Status.ok:
      raise RuntimeError(f"Failed to set value for {mapped_cref}: {status}")

  def _setBoolean(self, mapped_cref: str, value: bool):
    self.apiCall.append(f'oms_setBoolean("{mapped_cref}, {value})')
    status = Capi.setBoolean(mapped_cref, value) # Get the value from the CAPI
    if status != Status.ok:
      raise RuntimeError(f"Failed to set value for {mapped_cref}: {status}")

  def _setString(self, mapped_cref: str, value: str):
    self.apiCall.append(f'oms_setString("{mapped_cref}, {value})')
    status = Capi.setString(mapped_cref, value) # Get the value from the CAPI
    if status != Status.ok:
      raise RuntimeError(f"Failed to set value for {mapped_cref}: {status}")

  def getValue(self, cref: CRef):
    name = ".".join(cref.names[:-1])
    if name not in self.mappedCrefs:
      raise KeyError(f"Missing required key: '{name}'")

    value_path = ".".join([self.mappedCrefs[name], cref.names[-1]])

    # Determine the variable type
    type, status = Capi.getVariableType(value_path)
    if status != Status.ok:
      raise RuntimeError(f"Failed to get variable type for {cref}: {status}")

    match SignalType(type):
      case SignalType.Real:  # oms_signal_type_real
        return self._getReal(value_path)
      case SignalType.Integer:  # oms_signal_type_integer
        return self._getInteger(value_path)
      case SignalType.Boolean:  # oms_signal_type_boolean
        return self._getBoolean(value_path)
      case SignalType.String:  # oms_signal_type_string
        return self._getString(value_path)
      case SignalType.Enumeration:  # oms_signal_type_enumeration
        return self._getInteger(value_path)  # Treat enumeration as integer
      case _:
        raise TypeError(f"Unsupported type: {type}")

  def _getReal(self, cref: CRef):
    value, status = Capi.getReal(cref) # Get the value from the CAPI
    if status != Status.ok:
      raise RuntimeError(f"Failed to get value for {cref}: {status}")
    return value

  def _getInteger(self, cref: CRef):
    value, status = Capi.getInteger(cref)
    if status != Status.ok:
      raise RuntimeError(f"Failed to get value for {cref}: {status}")
    return value

  def _getBoolean(self, cref: CRef):
    value, status = Capi.getBoolean(cref)
    if status != Status.ok:
      raise RuntimeError(f"Failed to get value for {cref}: {status}")
    return value

  def _getString(self, cref: CRef):
    value, status = Capi.getString(cref)
    if status != Status.ok:
      raise RuntimeError(f"Failed to get value for {cref}: {status}")
    return value

  def initialize(self):
    status = Capi.initialize(self.modelName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to initialize model: {status}")

  def simulate(self):
    status = Capi.simulate(self.modelName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to simulate model: {status}")

  def stepUntil(self, stopTime):
    status = Capi.stepUntil(self.modelName, stopTime)
    if status != Status.ok:
      raise RuntimeError(f"Failed to step until {stopTime}: {status}")

  def setResultFile(self, filename: str):
    status = Capi.setResultFile("model", filename)
    if status !=Status.ok:
      raise RuntimeError(f"Failed to setResultFile {filename}: {status}")

  def terminate(self):
    status = Capi.terminate(self.modelName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to terminate model: {status}")

  def delete(self):
    status = Capi.delete(self.modelName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to delete model: {status}")
    self.modelName = None  # Clear the model name after deletion
    self.apiCall.clear()  # Clear the API call history
