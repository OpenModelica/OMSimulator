import xml.etree.ElementTree as ET
from pathlib import Path

from OMSimulator.component import Component
from OMSimulator.connector import Connector
from OMSimulator.variable import SignalType
from OMSimulator.unit import Unit
from OMSimulator import namespace


def _setParameters(parameterValues: dict, obj):
  if len(parameterValues) > 0:
    for key, (value, unit) in parameterValues.items():
      obj.setValue(key, value, unit)

def parseConnection(node, root):
  connections_node = node.find("ssd:Connections", namespaces=namespace.ns)
  if connections_node is None:
    return
  for connection in connections_node.findall("ssd:Connection", namespaces=namespace.ns):
    startElement = connection.get("startElement", '')
    startConnector = connection.get("startConnector")
    endElement = connection.get("endElement", '')
    endConnector = connection.get("endConnector")
    root.addConnection(startElement, startConnector, endElement, endConnector)

def parseDefaultExperiment(node, root):
  default_experiment = node.find("ssd:DefaultExperiment", namespaces=namespace.ns)
  if default_experiment is None:
    return
  root.startTime = default_experiment.get("startTime")
  root.stopTime = default_experiment.get("stopTime")
  ##TODO parse ssd:annotation

def parseUnitDefinitions(node, root):
  units_element = node.find("ssd:Units", namespaces=namespace.ns)
  if units_element is None:
    return
  for unit in units_element.findall("ssc:Unit", namespaces=namespace.ns):
    name = unit.get("name")
    base_unit = unit.find("ssc:BaseUnit", namespaces=namespace.ns)
    if base_unit is not None:
      attributes = {key: base_unit.get(key) for key in base_unit.keys()}
      # Create a Unit object and add it to the root or system
      unit_obj = Unit(name, attributes)
      root.unitDefinitions.append(unit_obj)
      # print(f"Unit: {name}, Attributes: {attributes}")

def parseElements(node, resources = None):
  """Extract components from <ssd:Elements> section"""
  from OMSimulator.system import System

  elements = {}
  elements_node = node.find("ssd:Elements", namespaces=namespace.ns)
  if elements_node is None:
    return elements
  for component in elements_node.findall("ssd:Component", namespaces=namespace.ns):
    name = component.get("name")
    comp_type = component.get("type")
    source = component.get("source")
    elements[name] = Component(name, source)
    elements[name].connectors = parseConnectors(component)
    parseParameterBindings(component, elements[name], resources)
  for system in elements_node.findall("ssd:System", namespaces=namespace.ns):
    name = component.get("name")
    elements[name] = System(name)
    elements[name].connectors = parseConnectors(system)
    parseParameterBindings(system, elements[name], resources)
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
    # Convert kind string to enum type
    kind = Connector.getCausalityFromString(kind)
    # Find the connector type (Real, Integer, Boolean)
    con = None
    for connectortype in ["ssc:Real", "ssc:Integer", "ssc:Boolean"]:  #expected connector types
      if connector.find(connectortype, namespaces=namespace.ns) is not None:
        signal_type = connectortype.split(":")[-1]  # Extracts 'Real, Integer, Boolean'
        con = Connector(name, kind, SignalType[signal_type])
        unit = connector.get("unit")
        if unit:
          con.setUnit(unit)
        connectors.append(con)
        break  # Stop after the first valid type is found
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
          print("warning: SSV file not found: ", source)
        obj.addSSV(source)
      else:
        values = binding.find("ssd:ParameterValues", namespaces=namespace.ns)
        if values is not None:
          param_set = values.find("ssv:ParameterSet", namespaces=namespace.ns)
          if param_set is not None:
            parameters = param_set.find("ssv:Parameters", namespaces=namespace.ns)
            parameterValues = parseParameterBindingHelper(parameters)
            _setParameters(parameterValues, obj)

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
          parameterValues[name] = (cast_func(value), unit)  # Convert to correct type
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
