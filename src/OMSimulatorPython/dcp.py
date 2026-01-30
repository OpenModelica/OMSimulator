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

class DCP:
  def __init__(self, dcp_path: Union[str, Path]):
    '''Initialize the DCP by loading dcpSlaveDescription.xml from the DCP archive.'''
    self._dcp_path = Path(dcp_path)
    self._dcpMajorVersion = None
    self._dcpMinorVersion = None
    self._salveName = None
    self._uuid = None
    self._variableNamingConvention = None
    self._variables = []

    self._load_slave_description()

  @property
  def dcpVersion(self):
    return self._dcpVersion

  @property
  def slaveName(self):
    return self._slaveName

  @property
  def fmuType(self):
    return "dcp"

  @property
  def uuid(self):
    return self._uuid

  @property
  def description(self):
    return self._description

  @property
  def variableNamingConvention(self):
    return self._variableNamingConvention

  @property
  def variables(self):
    return self._variables

  def _load_slave_description(self):
    '''Extract and parse the dcpSlaveDescription.xml from the DCP zip archive.'''
    if not self._dcp_path.exists():
      raise FileNotFoundError(f'DCP file not found: {self._dcp_path}')

    try:
      with zipfile.ZipFile(self._dcp_path, 'r') as dcp_zip:
        # Find dcpSlaveDescription.dcpx in the DCP archive
        slave_desc_name = 'v1.0/dcpSlaveDescription.dcpx'
        if slave_desc_name not in dcp_zip.namelist():
          raise FileNotFoundError(f'Missing {slave_desc_name} in {self._dcp_path}')

        # Read and parse dcpSlaveDescription.dcpx
        with dcp_zip.open(slave_desc_name) as slave_desc_file:
          xml_content = slave_desc_file.read()
          slave_description = ET.fromstring(xml_content)  # May raise XMLSyntaxError

          # Parse slaveName, uuid, ...
          self._dcpMajorVersion = slave_description.get('dcpMajorVersion')
          self._dcpMinorVersion = slave_description.get('dcpMinorVersion')
          self._slaveName = slave_description.get('dcpSlaveName')
          self._uuid = slave_description.get('uuid')
          self._variableNamingConvention = slave_description.get('variableNamingConvention')

          # Parse variables
          self._parse_variables(slave_description)

    except ET.XMLSyntaxError as e:
      raise ValueError(f'Error parsing {slave_desc_name}: {e}')

  def _parse_variables(self, slave_description):
    '''Parses variables from the ModelVariables section of modelDescription.xml'''
    scalar_variables = slave_description.xpath('//Variables/Variable')
    for scalar_var in scalar_variables:
      name = scalar_var.get('name')
      description = scalar_var.get('description')
      value_reference = scalar_var.get('valueReference')
      variability = scalar_var.get('variability', 'continuous')

      causality = None
      var_type = None
      start = None
      unit = None

      causality_element = scalar_var.xpath('./*')
      if causality_element:
        causality_element = causality_element[0]
        causality = causality_element.tag.lower()
        print("Causality: "+causality)

        # Find the first child (type node) and extract attributes
        type_element = causality_element.xpath('./*')  # Selects the first child element
        if type_element:
          type_element = type_element[0]  # Get first match
          var_type = type_element.tag
          if var_type == "Float64":
            var_type = "Real"   #HACK, only FMI2 types are currently supported
          start = type_element.get('start')
          unit = type_element.get('unit')

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

  def _parse_units(self, slave_description):
    '''Extracts unit definitions from modelDescription.xml.'''
    self._unitDefinitions = []

    unit_elements = slave_description.xpath('//UnitDefinitions/Unit')
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
