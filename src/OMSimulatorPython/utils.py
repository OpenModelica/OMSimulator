import logging
import xml.etree.ElementTree as ET
from pathlib import Path

from OMSimulator.component import Component
from OMSimulator.connection import ConnectionGeometry
from OMSimulator.connector import Connector, ConnectorGeometry
from OMSimulator.unit import Unit
from OMSimulator.variable import Causality, SignalType
from OMSimulator.elementgeometry import ElementGeometry
from OMSimulator import namespace

logger = logging.getLogger(__name__)

def _setParameters(parameterValues: dict, obj):
  if len(parameterValues) > 0:
    for key, (value, unit, description) in parameterValues.items():
      obj.setValue(key, value, unit, description)

def parseConnection(node, root):
  connections_node = node.find("ssd:Connections", namespaces=namespace.ns)
  if connections_node is None:
    return
  for connection in connections_node.findall("ssd:Connection", namespaces=namespace.ns):
    startElement = connection.get("startElement", '')
    startConnector = connection.get("startConnector")
    endElement = connection.get("endElement", '')
    endConnector = connection.get("endConnector")
    description = connection.get("description")
    root.addConnection(startElement, startConnector, endElement, endConnector)
    if description:
      root.connections[-1].description = description
    connection_geometry = connection.find("ssd:ConnectionGeometry", namespaces=namespace.ns)
    if connection_geometry is not None:
      pointsX = connection_geometry.get("pointsX")
      pointsY = connection_geometry.get("pointsY")
      # print (f"ConnectionGeometry: pointsX: {pointsX}, pointsY: {pointsY}")
      if pointsX and pointsY:
        connectionGeometry = ConnectionGeometry([float(x) for x in pointsX.split()], [float(y) for y in pointsY.split()])
        root.connections[-1].connectionGeometry = connectionGeometry

def parseDefaultExperiment(node, root):
  default_experiment = node.find("ssd:DefaultExperiment", namespaces=namespace.ns)
  if default_experiment is None:
    return
  root.startTime = default_experiment.get("startTime")
  root.stopTime = default_experiment.get("stopTime")
  ##TODO parse ssd:annotation

def parseUnitDefinitions(node, root, tagname = "ssd:Units"):
  units_element = node.find(tagname, namespaces=namespace.ns)
  if units_element is None:
    return
  for unit in units_element.findall("ssc:Unit", namespaces=namespace.ns):
    name = unit.get("name")
    description = unit.get("description")
    base_unit = unit.find("ssc:BaseUnit", namespaces=namespace.ns)
    if base_unit is not None:
      attributes = {key: base_unit.get(key) for key in base_unit.keys()}
      # Create a Unit object and add it to the root or system
      unit_obj = Unit(name, attributes, description)
      root.unitDefinitions.append(unit_obj)
      # print(f"Unit: {name}, Attributes: {attributes}")

def parseElements(node, resources = None):
  """Extract components from <ssd:Elements> section"""
  from OMSimulator.system import System, SystemGeometry

  elements = {}
  elements_node = node.find("ssd:Elements", namespaces=namespace.ns)
  if elements_node is None:
    return elements

  # Parse the sub system <ssd:system> in the <ssd:Elements> section
  for system in elements_node.findall("ssd:System", namespaces=namespace.ns):
    name = system.get("name")
    description = system.get("description")
    elements[name] = System(name)
    elements[name].description = description
    elements[name].connectors = parseConnectors(system)
    elements[name].elementgeometry = ElementGeometry.importFromNode(system)
    elements[name].systemgeometry = SystemGeometry.importFromNode(system)
    parseParameterBindings(system, elements[name], resources)
    elements[name].elements = parseElements(system, resources)  # recursively parse nested elements in the sub-system
    parseConnection(system, elements[name]) # parse connections for the sub-system

  for component in elements_node.findall("ssd:Component", namespaces=namespace.ns):
    name = component.get("name")
    comp_type = component.get("type")
    source = component.get("source")
    description = component.get("description")
    elements[name] = Component(name, source)
    elements[name].description = description
    elements[name].connectors = parseConnectors(component)
    elements[name].elementgeometry = ElementGeometry.importFromNode(component)
    parseParameterBindings(component, elements[name], resources)

  return elements

def parseConnectors(node):
  """Extract and print system connectors"""
  connectors = []
  connectors_node = node.find("ssd:Connectors", namespaces=namespace.ns)

  # No connectors found
  if connectors_node is None:
    return connectors

  for connector in connectors_node.findall("ssd:Connector", namespaces=namespace.ns):
    name = connector.get("name")
    kind = connector.get("kind")
    description = connector.get("description")
    # Convert kind string to enum type
    kind = Causality[kind]

    # Find the connector type (Real, Integer, Boolean)
    con = None
    for connectortype in ["ssc:Real", "ssc:Integer", "ssc:Boolean"]:  # Expected connector types
      type_element = connector.find(connectortype, namespaces=namespace.ns)
      if type_element is not None:
        signal_type = connectortype.split(":")[-1]  # Extracts 'Real', 'Integer', or 'Boolean'
        con = Connector(name, kind, SignalType[signal_type])
        unit = type_element.get("unit")
        if description:
          con.description = description
        # Set unit if it exists
        if unit:
            con.setUnit(unit)
        # print(f"Connector: {name}, Kind: {kind}, SignalType: {signal_type}, Unit: {unit}")
        break  # Stop after the first valid type is found

    # Check if connector has geometry information
    connector_geometry = connector.find("ssd:ConnectorGeometry", namespaces=namespace.ns)
    if connector_geometry is not None:
      x = connector_geometry.get("x")
      y = connector_geometry.get("y")
      if x and y:
        connectorGeometry = ConnectorGeometry(float(x), float(y))
        connectorGeometry.x = float(x)  # Set x coordinate
        connectorGeometry.y = float(y)  # Set y coordinate
        con.connectorGeometry = connectorGeometry

    if con:
      connectors.append(con)

  return connectors

def parseParameterBindings(node, obj, resources):
  """Extract and print system parameters"""
  parameter_bindings = node.find("ssd:ParameterBindings", namespaces=namespace.ns)
  if parameter_bindings is not None:
    for binding in parameter_bindings.findall("ssd:ParameterBinding", namespaces=namespace.ns):
      source = binding.get("source")
      if binding.get("source"):
        ## use the instantiated ssv class to set the parameter Resources
        if source not in resources:
          logger.warning(f"SSV file not found: {source}")
        obj.addSSV(source)
      else:
        values = binding.find("ssd:ParameterValues", namespaces=namespace.ns)
        if values is not None:
          param_set = values.find("ssv:ParameterSet", namespaces=namespace.ns)
          if param_set is not None:
            parameters = param_set.find("ssv:Parameters", namespaces=namespace.ns)
            parameterValues = parseParameterBindingHelper(parameters)
            _setParameters(parameterValues, obj)

          units = param_set.find("ssv:Units", namespaces=namespace.ns)
          parseUnitDefinitions(param_set, obj, tagname="ssv:Units")
          # if units:
          #   print("Units found:")

          # Parse units
          # units = param_set.find("ssv:Units", namespaces=namespace.ns)
          # if units is not None:
          #   for unit in units.findall("ssv:Unit", namespaces=namespace.ns):
          #     name = unit.get("name")
          #     base_unit = unit.find("ssv:BaseUnit", namespaces=namespace.ns)
          #     if base_unit is not None:
          #       attributes = {key: base_unit.get(key) for key in base_unit.keys()}
          #       # Create a Unit object and add it to the root or system
          #       unit_obj = Unit(name, attributes)
          #       obj.unitDefinitions.append(unit_obj)
          #       print(f"Unit PArameters: {name}, Attributes: {attributes}")

def parseSSV(filename):
  tree = ET.parse(filename)
  root = tree.getroot()
  parameters = root.find("ssv:Parameters", namespaces=namespace.ns)
  return parseParameterBindingHelper(parameters)

def parseParameterBindingHelper(parameters):
  if parameters is not None:
    parameterValues={}
    for param in parameters.findall("ssv:Parameter", namespaces=namespace.ns):
      name = param.get("name")
      description = param.get("description")
      value_types = {
                      "ssv:Real": float,
                      "ssv:Integer": int,
                      "ssv:Boolean": lambda v: v.lower() == "true",  # Convert "true"/"false" to bool
                      "ssv:String": str
                    }
      for value_type, cast_func in value_types.items():
        value_element = param.find(value_type, namespaces=namespace.ns)
        if value_element is not None:
          value = value_element.get("value")
          unit = value_element.get("unit")
          parameterValues[name] = (cast_func(value), unit, description)  # Convert to correct type
          break  # Stop after first found type
    return parameterValues

def validateSSP(root, schema_file):
  module_dir = Path(__file__).parent
  schema = ET.XMLSchema(file=module_dir / 'schema' / schema_file)
  if not schema.validate(root):
      message = "Failed to validate SystemStructure.ssd:"
      for entry in schema.error_log:
          message += "\n%s (line %d, column %d): %s" % (entry.level_name, entry.line, entry.column, entry.message)
      raise Exception(message)
