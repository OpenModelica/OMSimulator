import zipfile
import logging
from pathlib import Path
from typing import Union

from lxml import etree as ET
from OMSimulator.connector import Connector
from OMSimulator.unit import Unit
from OMSimulator.variable import Variable, SignalType
from OMSimulator import namespace

logger = logging.getLogger(__name__)

class FMU:
  def __init__(self, fmu_path: Union[str, Path]):
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

          # Parse UnitDefinitions
          self._parse_units(model_description)

          # Parse variables
          self._parse_variables(model_description)

    except ET.XMLSyntaxError as e:
      raise ValueError(f'Error parsing {model_desc_name}: {e}')

  def _parse_variables(self, model_description):
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

        # Handle state variables if it's a derivative
        if derivative_index > 0 and variability == 'continuous':
          self._states.append(self._variables[derivative_index - 1])

      # Create and store the variable
      variable = Variable(name, description, value_reference, causality, variability, var_type, unit, start)

      # Assign unit definitions if applicable
      if unit:
        for defined_unit in self._unitDefinitions:
          if defined_unit.name == variable.unit:
            variable.unitDefinition.append(defined_unit)

      self._variables.append(variable)

    # Raise an error if ModelVariables section is missing
    if not scalar_variables:
      raise Exception('ModelVariables section not found in modelDescription.xml')

  def _parse_units(self, model_description):
    '''Extracts unit definitions from modelDescription.xml.'''
    self._unitDefinitions = []

    unit_elements = model_description.xpath('//UnitDefinitions/Unit')
    for unit_elem in unit_elements:
      name = unit_elem.get('name')

      base_units = {
        key: value
        for base_unit in unit_elem.xpath('./BaseUnit')
        for key, value in base_unit.attrib.items()
      }

      self._unitDefinitions.append(Unit(name, base_units))

  def makeConnectors(self):
    connectors = []
    for var in self.variables:
      if var.isInput() or var.isOutput() or var.isParameter() or var.isCalculatedParameter():
        connector = Connector(var.name, var.causality, var.signal_type)
        connector.setUnit(var.unit)
        connector.description = var.description
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
