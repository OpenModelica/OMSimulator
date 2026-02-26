import zipfile
import logging
import tempfile
from pathlib import Path
from typing import Union

from lxml import etree as ET
from OMSimulator.connector import Connector
from OMSimulator.unit import Unit
from OMSimulator.variable import Variable, SignalType
from OMSimulator import namespace
from OMSimulator.capi import Capi, Status
from OMSimulator.cref import CRef
from OMSimulator.enumeration import Enumeration

logger = logging.getLogger(__name__)

class FMU:
  def __init__(self, fmu_path: Union[str, Path], instanceName: str = None):
    '''Initialize the FMU by loading modelDescription.xml from the FMU archive.'''
    self._fmu_path = Path(fmu_path)
    self._fmiVersion = None
    self._modelName = None
    self._fmuType = None
    self._guid = None
    self._description = None
    self._generationTool = None
    self._generationDateAndTime = None
    self._variableNamingConvention = None
    self._variables = []
    self._states = []
    self._unitDefinitions = []
    self._enumerationDefinitions = []
    self.defaultExperiment = {}
    self.apiCall = []
    self.instanceName = instanceName
    self.fmuInstantitated = False
    self._load_model_description()

  @property
  def fmiVersion(self):
    return self._fmiVersion

  @property
  def modelName(self):
    return self._modelName

  @property
  def fmuType(self):
    return self._fmuType

  @property
  def guid(self):
    return self._guid

  @property
  def description(self):
    return self._description

  @property
  def generationTool(self):
    return self._generationTool

  @property
  def generationDateAndTime(self):
    return self._generationDateAndTime

  @property
  def variableNamingConvention(self):
    return self._variableNamingConvention


  @property
  def variables(self):
    return self._variables

  @property
  def states(self):
    return self._states

  def _load_model_description(self):
    '''Extract and parse the modelDescription.xml from the FMU zip archive.'''
    if not self._fmu_path.exists():
      raise FileNotFoundError(f'FMU file not found: {self._fmu_path}')

    try:
      with zipfile.ZipFile(self._fmu_path, 'r') as fmu_zip:
        # Find modelDescription.xml in the FMU archive
        model_desc_name = 'modelDescription.xml'
        if model_desc_name not in fmu_zip.namelist():
          raise FileNotFoundError(f'Missing {model_desc_name} in {self._fmu_path}')

        # Read and parse modelDescription.xml
        with fmu_zip.open(model_desc_name) as model_desc_file:
          xml_content = model_desc_file.read()
          model_description = ET.fromstring(xml_content)  # May raise XMLSyntaxError

          # Parse modelName, guid, ...
          self._fmiVersion = model_description.get('fmiVersion')
          self._modelName = model_description.get('modelName')
          self._guid = model_description.get('guid')
          self._description = model_description.get('description')
          self._generationTool = model_description.get('generationTool')
          self._generationDateAndTime = model_description.get('generationDateAndTime')
          self._variableNamingConvention = model_description.get('variableNamingConvention')

          has_me = model_description.find('.//{*}ModelExchange') is not None
          has_cs = model_description.find('.//{*}CoSimulation') is not None

          if has_me and has_cs:
            self._fmuType = 'me_cs'
          elif has_me:
            self._fmuType = 'me'
          elif has_cs:
            self._fmuType = 'cs'

          # Parse default experiment settings
          self._parse_default_experiment(model_description)

          # Parse UnitDefinitions
          self._parse_units(model_description)

          # Dispatch version-specific handling
          if self._fmiVersion == "2.0":
            # Parse typeDefinitions
            self._parse_typeDefinitions_fmi2(model_description)
            # Parse ModelVariables
            self._parse_variables_fmi2(model_description)
          elif self._fmiVersion == "3.0":
            # Parse typeDefinitions
            self._parse_typeDefinitions_fmi3(model_description)
            # Parse ModelVariables
            self._parse_variables_fmi3(model_description)
    except ET.XMLSyntaxError as e:
      raise ValueError(f'Error parsing {model_desc_name}: {e}')

  def _parse_default_experiment(self, model_description):
    """Extract default experiment settings from modelDescription.xml."""
    default_experiment = model_description.find('.//DefaultExperiment')

    def _get(attr, default):
      val = default_experiment.get(attr) if default_experiment is not None else None
      return float(val) if val is not None else default

    self.defaultExperiment = {
      'startTime': _get('startTime', 0.0),
      'stopTime': _get('stopTime', 1.0),
      'tolerance': _get('tolerance', 1e-6),
      'stepSize': _get('stepSize', 1e-3),
    }

  def _parse_variables_fmi2(self, model_description):
    '''Parses variables from the ModelVariables section of modelDescription.xml'''
    scalar_variables = model_description.xpath('//ModelVariables/ScalarVariable')
    for scalar_var in scalar_variables:
      name = scalar_var.get('name')
      description = scalar_var.get('description')
      value_reference = scalar_var.get('valueReference')
      causality = scalar_var.get('causality', 'local')
      variability = scalar_var.get('variability', 'continuous')

      var_type = None
      unit = None
      start = None
      derivative_index = -1

      # Find the first child (type node) and extract attributes
      type_element = scalar_var.xpath('./*')  # Selects the first child element
      if type_element:
        type_element = type_element[0]  # Get first match
        var_type = type_element.tag
        unit = type_element.get('unit')
        start = type_element.get('start')
        derivative_index = int(type_element.get('derivative', '-1'))
        declaredType = type_element.get('declaredType')

        # Handle state variables if it's a derivative
        if derivative_index > 0 and variability == 'continuous':
          self._states.append(self._variables[derivative_index - 1])

      # Create and store the variable
      variable = Variable(name, description, value_reference, causality, variability, var_type, unit, start, declaredType)

      # Assign unit definitions if applicable
      if unit:
        for defined_unit in self._unitDefinitions:
          if defined_unit.name == variable.unit:
            variable.unitDefinition.append(defined_unit)

      # assign enumeration definitions if applicable
      if declaredType:
        for enumeration in self._enumerationDefinitions:
          if declaredType == enumeration.name:
            variable.enumerationDefinition.append(enumeration)

      self._variables.append(variable)

    # Raise an error if ModelVariables section is missing
    if not scalar_variables:
      raise Exception('ModelVariables section not found in modelDescription.xml')

  def _parse_variables_fmi3(self, model_description):
    '''Parses variables from the ModelVariables section of modelDescription.xml'''
    model_variables = model_description.xpath('//ModelVariables')
    ## iterate over all ModelVariables sections (fmi3 may have multiple)
    for model_var_section in model_variables:
      for element in model_var_section:
        # Element tag (e.g., Float64, Int32, Boolean, String, etc.)
        var_type = element.tag
        # Extract standard FMI 3.0 attributes
        name = element.get("name")
        description = element.get("description")
        value_reference = element.get("valueReference")
        causality = element.get("causality", "local")
        variability = element.get("variability", "continuous")
        initial = element.get("initial")
        start = element.get("start")
        derivative_index = int(element.get('derivative', '-1'))
        declaredType = element.get('declaredType')
        unit = element.get("unit")

        # Handle state variables if it's a derivative
        if derivative_index > 0 and variability == 'continuous':
          self._states.append(self._variables[derivative_index - 1])

        # Handle nested <Start> element for String/Binary/Enumeration types
        if start is None:
          start_elem = element.find("Start")
          if start_elem is not None:
            start = start_elem.get("value")

        # print(f"var_name: {name}, var_type: {var_type}, causality: {causality}, variability: {variability}, initial: {initial}, start: {start}")

        # Create and store the variable
        variable = Variable(name, description, value_reference, causality, variability, var_type, unit, start, declaredType)

        # Assign unit definitions if applicable
        if unit:
          for defined_unit in self._unitDefinitions:
            if defined_unit.name == variable.unit:
              variable.unitDefinition.append(defined_unit)

        # assign enumeration definitions if applicable
        if declaredType:
          for enumeration in self._enumerationDefinitions:
            if declaredType == enumeration.name:
              variable.enumerationDefinition.append(enumeration)

        self._variables.append(variable)

    # Raise an error if ModelVariables section is missing
    if not model_variables:
      raise Exception('ModelVariables section not found in modelDescription.xml')

  def _parse_units(self, model_description):
    '''Extracts unit definitions from modelDescription.xml.'''

    unit_elements = model_description.xpath('//UnitDefinitions/Unit')
    for unit_elem in unit_elements:
      name = unit_elem.get('name')

      base_units = {
        key: value
        for base_unit in unit_elem.xpath('./BaseUnit')
        for key, value in base_unit.attrib.items()
      }

      self._unitDefinitions.append(Unit(name, base_units))

  def _parse_typeDefinitions_fmi2(self, model_description):
    '''Extracts only enumeration type definitions from modelDescription.xml.'''
    for simple_type in model_description.xpath('//TypeDefinitions/SimpleType'):
      type_name = simple_type.get('name')
      # Check if the SimpleType has an Enumeration child
      enum_elem = simple_type.find('./Enumeration')
      if enum_elem is not None:
        # Collect enumeration items
        items = {}
        for item in enum_elem.xpath('./Item'):
          items[item.get("name")] = int(item.get("value"))
        self._enumerationDefinitions.append(Enumeration(type_name, items))

  def _parse_typeDefinitions_fmi3(self, model_description):
    '''Extracts only enumeration type definitions from modelDescription.xml.'''
    for simple_type in model_description.xpath('//TypeDefinitions/EnumerationType'):
      type_name = simple_type.get('name')
      # Collect enumeration items
      items = {}
      for item in simple_type.xpath('./Item'):
        items[item.get("name")] = int(item.get("value"))
      self._enumerationDefinitions.append(Enumeration(type_name, items))

  def makeConnectors(self):
    connectors = []
    for var in self.variables:
      if var.isInput() or var.isOutput() or var.isParameter() or var.isCalculatedParameter():
        connector = Connector(var.name, var.causality, var.signal_type)
        connector.setUnit(var.unit)
        connector.description = var.description
        if var.declaredType:
          for enum in var.enumerationDefinition:
            if enum.name == var.declaredType:
              connector.setEnumerationName(var.declaredType)
        connectors.append(connector)
    return connectors

  def varExist(self, cref: str) -> bool:
    return any(var.name == cref for var in self.variables)

  def export_units_to_ssd(self, node):
    '''Exports all unit definitions to an SSD XML node.'''
    for unit in self._unitDefinitions:
      unit.exportToSSD(node)

  def exportSSVTemplate(self, filename: Path | None = None):
    if filename is None:
      filename = self.modelName + '.ssv'

    ssv_node = ET.Element(namespace.tag("ssv", "ParameterSet"),
                                   nsmap={"ssc": "http://ssp-standard.org/SSP1/SystemStructureCommon",
                                          "ssv": "http://ssp-standard.org/SSP1/SystemStructureParameterValues"},
                                   version = "2.0",
                                   name = "parameters")
    parameters_node = ET.SubElement(ssv_node, namespace.tag("ssv", "Parameters"))
    ## extract variables with start values
    for var in self.variables:
      if var.isInput() or var.isParameter() or var.isCalculatedParameter():
        parameter_node = ET.SubElement(parameters_node, namespace.tag("ssv", "Parameter"))
        parameter_node.set("name", str(var.name))
        if var.description:
          parameter_node.set("description", var.description)

        match var.signal_type:
          case SignalType.Real:
            type_tag = "Real"
          case SignalType.Boolean:  # Check for boolean first, because it is a subclass of int
            type_tag = "Boolean"
          case SignalType.Integer:
            type_tag = "Integer"
          case SignalType.String:
            type_tag = "String"
          case _:
            raise TypeError(f"Unsupported type: {type(var.signal_type)}")
        parameter_type = ET.SubElement(parameter_node, namespace.tag("ssv", type_tag))
        parameter_type.set("value", str(var.modelDescriptionStartValue))
        if var.unit is not None:
          parameter_type.set("unit", str(var.unit))


    xml = ET.tostring(ssv_node, encoding='utf-8', xml_declaration=True, pretty_print=True).decode('utf-8')

    ## write to filesystem
    with open(Path(filename).resolve(), "w", encoding="utf-8") as file:
      file.write(xml)
    logger.info(f"SSV template '{filename}' successfully exported!")

  def exportSSMTemplate(self, filename : Path | None = None):
    if filename is None:
      filename = self.modelName + '.ssm'

    ssm_node = ET.Element(namespace.tag("ssm", "ParameterMapping"),
                                   nsmap={"ssc": "http://ssp-standard.org/SSP1/SystemStructureCommon",
                                          "ssm": "http://ssp-standard.org/SSP1/SystemStructureParameterMapping"},
                                   version = "2.0")

    for var in self.variables:
      if var.isInput() or var.isParameter() or var.isCalculatedParameter():
        ssm_mapping_node = ET.SubElement(ssm_node, namespace.tag("ssm", "MappingEntry"))
        ssm_mapping_node.set("source", "")
        ssm_mapping_node.set("target", str(var.name))



    xml = ET.tostring(ssm_node, encoding='utf-8', xml_declaration=True, pretty_print=True).decode('utf-8')

    ## write to filesystem
    with open(Path(filename).resolve(), "w", encoding="utf-8") as file:
      file.write(xml)
    logger.info(f"SSM template '{filename}' successfully exported!")

  def dumpApiCalls(self):
    """Returns the generated API calls as a string."""
    return "\n".join(self.apiCall)

  def splitModelName(self):
    '''Splits the model name into its hierarchical components.'''
    if self.modelName is None:
      raise ValueError("modelName is not set")
    parts = self.modelName.split('.')
    if len(parts) == 1:
      return parts[0]
    else:
      return parts[-1]


  def instantiate(self):
    '''Instantiate the FMU for simulation.'''
    status = Capi.setCommandLineOption("--suppressPath=true")
    if status != Status.ok:
      raise RuntimeError(f"Failed to set command line option: {status}")
    status = Capi.setTempDirectory(tempfile.mkdtemp())
    if status != Status.ok:
      raise RuntimeError(f"Failed to set temp directory: {status}")

    if self.instanceName is None:
      self.instanceName = self.splitModelName()

    # Create a new model
    status = Capi.newModel(self.instanceName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to create new model: {status}")
    self.apiCall.append(f'oms.newModel({self.instanceName})')

    ## add system type: wc (weakly coupled) or sc (strongly coupled)
    match self.fmuType:
      case 'me_cs' | 'cs':
        system_type = 1  # wc
      case 'me':
        system_type = 2  # sc
      case _:
        raise ValueError(f"Unsupported fmuType: {self.fmuType}")
    root_name = f"{self.instanceName}.root"
    status = Capi.addSystem(root_name, system_type)
    if status != Status.ok:
      raise RuntimeError(f"Failed to add system: {status}")

    self.apiCall.append(f'oms.addSystem({root_name}, {system_type})')

    # Add the FMU to the model
    status = Capi.addSubModel(f"{root_name}.{self.instanceName}", str(self._fmu_path))
    if status != Status.ok:
      raise RuntimeError(f"Failed to add submodel: {status}")

    self.apiCall.append(f'oms.addSubModel({root_name}.{self.instanceName}, {self._fmu_path})')

    # set export name
    status = Capi.setExportName(f"{root_name}.{self.instanceName}", self.instanceName)  # Set export name if provided
    if status != Status.ok:
      raise RuntimeError(f"Failed to set export name: {status}")

    status = Capi.instantiate(self.instanceName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to instantiate model: {status}")
    self.fmuInstantitated = True

    self.setDefaultExperiment()

    self.apiCall.append(f'oms.instantiate("{self.instanceName}")')

  def setDefaultExperiment(self):
    self.setStartTime(self.defaultExperiment.get('startTime'))
    self.setStopTime(self.defaultExperiment.get('stopTime'))
    self.setTolerance(self.defaultExperiment.get('tolerance'))
    self.setStepSize(self.defaultExperiment.get('stepSize'))

  def setStartTime(self, startTime: float):
    if self.fmuInstantitated is False:
      raise RuntimeError("FMU must be instantiated before setting start time")

    status = Capi.setStartTime(self.instanceName, startTime)
    if status != Status.ok:
      raise RuntimeError(f"Failed to set start time: {status}")

  def setStopTime(self, stopTime: float):
    if self.fmuInstantitated is False:
      raise RuntimeError("FMU must be instantiated before setting stop time")

    status = Capi.setStopTime(self.instanceName, stopTime)
    if status != Status.ok:
      raise RuntimeError(f"Failed to set stop time: {status}")

  def setTolerance(self, tolerance: float):
    if self.fmuInstantitated is False:
      raise RuntimeError("FMU must be instantiated before setting tolerance")

    status = Capi.setTolerance(self.instanceName, tolerance)
    if status != Status.ok:
      raise RuntimeError(f"Failed to set tolerance: {status}")

  def setStepSize(self, stepSize: float):
    if self.fmuInstantitated is False:
      raise RuntimeError("FMU must be instantiated before setting variable step size")

    status = Capi.setVariableStepSize(self.instanceName, 1e-6, 1e-12, stepSize)
    if status != Status.ok:
      raise RuntimeError(f"Failed to set variable step size: {status}")

  def getValue(self, cref: str):
    if self.fmuInstantitated is False:
      raise RuntimeError("FMU must be instantiated before getting values")

    '''Get the value of a variable by its name.'''
    if not self.varExist(CRef(cref)):
      raise KeyError(f"Variable '{cref}' does not exist in the FMU")

    mappedCrefs = f"{self.instanceName}.root.{self.instanceName}.{cref}"

    # Determine the variable type
    type, status = Capi.getVariableType(mappedCrefs)
    if status != Status.ok:
      raise RuntimeError(f"Failed to get variable type for {cref}: {status}")

    match SignalType(type):
      case SignalType.Real:  # oms_signal_type_real
        return self._getReal(mappedCrefs)
      case SignalType.Integer:  # oms_signal_type_integer
        return self._getInteger(mappedCrefs)
      case SignalType.Boolean:  # oms_signal_type_boolean
        return self._getBoolean(mappedCrefs)
      case SignalType.String:  # oms_signal_type_string
        return self._getString(mappedCrefs)
      case SignalType.Enumeration:  # oms_signal_type_enumeration
        return self._getInteger(mappedCrefs)  # Treat enumeration as integer
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

  def setValue(self, cref: str, value):
    """Sets a value for a specific CRef in the model."""

    if self.fmuInstantitated is False:
      raise RuntimeError("FMU must be instantiated before setting values")

    if not self.varExist(CRef(cref)):
      raise KeyError(f"Variable '{cref}' does not exist in the FMU")

    mappedCrefs = f"{self.instanceName}.root.{self.instanceName}.{cref}"

    # Determine the variable type
    type, status = Capi.getVariableType(mappedCrefs)
    if status != Status.ok:
      raise RuntimeError(f"Failed to get variable type for {cref}: {status}")

    match SignalType(type):
      case SignalType.Real:  # oms_signal_type_real
        return self._setReal(mappedCrefs, value)
      case SignalType.Integer:  # oms_signal_type_integer
        return self._setInteger(mappedCrefs, value)
      case SignalType.Boolean:  # oms_signal_type_boolean
        return self._setBoolean(mappedCrefs, value)
      case SignalType.String:  # oms_signal_type_string
        return self._setString(mappedCrefs, value)
      case SignalType.Enumeration:  # oms_signal_type_enumeration
        return self._setInteger(mappedCrefs, value)  # Treat enumeration as integer
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

  def setResultFile(self, filename: str):
    if self.fmuInstantitated is False:
      raise RuntimeError("FMU must be instantiated before setting result file")

    status = Capi.setResultFile(self.instanceName, filename)
    if status != Status.ok:
      raise RuntimeError(f"Failed to setResultFile {filename}: {status}")

  def initialize(self):
    if self.fmuInstantitated is False:
      raise RuntimeError("FMU must be instantiated before initialization")

    status = Capi.initialize(self.instanceName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to initialize model: {status}")

  def simulate(self):
    if self.fmuInstantitated is False:
      raise RuntimeError("FMU must be instantiated before simulation")

    status = Capi.simulate(self.instanceName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to simulate model: {status}")

  def stepUntil(self, stopTime):
    status = Capi.stepUntil(self.instanceName, stopTime)
    if status != Status.ok:
      raise RuntimeError(f"Failed to step until {stopTime}: {status}")

  def setLoggingLevel(self, level: int):
    status = Capi.setLoggingLevel(level)
    if status != Status.ok:
      raise RuntimeError(f"Failed to set logging level: {status}")

  def setLogFile(self, filename: str):
    status = Capi.setLogFile(filename)
    if status != Status.ok:
      raise RuntimeError(f"Failed to set log file: {status}")

  def setLoggingInterval(self, interval: float):
    status = Capi.setLoggingInterval(self.instanceName, interval)
    if status != Status.ok:
      raise RuntimeError(f"Failed to set logging interval: {status}")

  def terminate(self):
    status = Capi.terminate(self.instanceName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to terminate model: {status}")

  def delete(self):
    status = Capi.delete(self.instanceName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to delete model: {status}")
    self.instanceName = None  # Clear the model name after deletion
    self.apiCall.clear()  # Clear the API call history
