import xml.etree.ElementTree as ET
from pathlib import Path

from OMSimulator.component import Component
from OMSimulator.connector import Connector
from OMSimulator.ssv import SSV
from OMSimulator.variable import SignalType

from OMSimulator import namespace


def _setParameters(parameterValues: dict, obj):
  if len(parameterValues) > 0:
    for key, value in parameterValues.items():
      obj.setValue(key, value)

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
  ##TODO parse unit definitions

def parseElements(node):
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
    parseParameterBindings(component, elements[name])
  for system in elements_node.findall("ssd:System", namespaces=namespace.ns):
    name = component.get("name")
    elements[name] = System(name)
    elements[name].connectors = parseConnectors(system)
    parseParameterBindings(system, elements[name])
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

def parseParameterBindings(node, obj = None, temp_dir: Path = None):
  """Extract and print system parameters"""
  parameterValues={}
  parameter_bindings = node.find("ssd:ParameterBindings", namespaces=namespace.ns)
  if parameter_bindings is not None:
    for binding in parameter_bindings.findall("ssd:ParameterBinding", namespaces=namespace.ns):
      source = binding.get("source")
      if binding.get("source"):
        ssv_file = temp_dir / source
        tree = ET.parse(ssv_file)
        root = tree.getroot()
        parameters = root.find("ssv:Parameters", namespaces=namespace.ns)
        parseParameterBindingHelper(parameters, parameterValues)
        resources = SSV(ssv_file.name)
        _setParameters(parameterValues, resources)
        obj.add(resources)
      else:
        values = binding.find("ssd:ParameterValues", namespaces=namespace.ns)
        if values is not None:
          param_set = values.find("ssv:ParameterSet", namespaces=namespace.ns)
          if param_set is not None:
            parameters = param_set.find("ssv:Parameters", namespaces=namespace.ns)
            parseParameterBindingHelper(parameters, parameterValues)
            _setParameters(parameterValues, obj)

def parseParameterBindingHelper(parameters, parameterValues):
  if parameters is not None:
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
          parameterValues[name] = cast_func(value)  # Convert to correct type
          break  # Stop after first found type

def validateSSP(root, schema_file):
  module_dir = Path(__file__).parent
  schema = ET.XMLSchema(file=module_dir / 'schema' / schema_file)
  if not schema.validate(root):
      message = "Failed to validate SystemStructure.ssd:"
      for entry in schema.error_log:
          message += "\n%s (line %d, column %d): %s" % (entry.level_name, entry.line, entry.column, entry.message)
      raise Exception(message)
