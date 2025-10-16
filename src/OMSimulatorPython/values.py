from lxml import etree as ET
from OMSimulator.unit import Unit
from OMSimulator.ssm import SSM
from OMSimulator import namespace
from OMSimulator.variable import SignalType

class Values:
  def __init__(self):
    self.start_values = {}

  def setValue(self, name, value, type, unit=None, description=None):
    if unit is not None and not type in ["Real", "Float32", "Float64"]:
      raise TypeError("Unit can only be set for Real values.")
    self.start_values[name] = (value, type, unit, description)

  def getValue(self, name):
    return self.start_values.get(name)

  def empty(self) -> bool:
    return not self.start_values

  def getAllParameters(self):
    return self.start_values

  def list(self, prefix = ""):
    if self.empty():
      return

    for key, (value, type, unit, description) in self.start_values.items():
      print(f"{prefix} ({key}, {type.name}, {value}, {unit}, '{description}')")

  def exportToSSD(self, node, parameterMapping : SSM | None = None, unitDefinitions = None):
    if self.empty():
      return

    parameter_bindings_node = ET.SubElement(node, namespace.tag("ssd", "ParameterBindings"))
    parameter_binding_node = ET.SubElement(parameter_bindings_node, namespace.tag("ssd", "ParameterBinding"))
    parameter_values_node = ET.SubElement(parameter_binding_node, namespace.tag("ssd", "ParameterValues"))
    parameter_set_node = ET.SubElement(parameter_values_node, namespace.tag("ssv", "ParameterSet"))
    parameter_set_node.set("version", "2.0")
    parameter_set_node.set("name", "parameters")
    parameters_node = ET.SubElement(parameter_set_node, namespace.tag("ssv", "Parameters"))

    self.add_parameters(parameters_node)

    ## export parameter mapping inline
    if parameterMapping:
      parameterMapping.exportToSSD(parameter_binding_node)

    ## export unit definitions
    if unitDefinitions:
      unit_definitions_node = ET.SubElement(parameter_set_node, namespace.tag("ssv", "Units"))
      for unit in unitDefinitions:
        unit.exportToSSD(unit_definitions_node)


  def exportToSSV(self, node):
    if self.empty():
      return

    parameters_node = ET.SubElement(node, namespace.tag("ssv", "Parameters"))
    self.add_parameters(parameters_node)

  def add_parameters(self, parameters_node, prefix = None):
    """Generic function to add XML parameters based on the value type."""
    for key, (value, type, unit, description) in self.start_values.items():
      parameter_node = ET.SubElement(parameters_node, namespace.tag("ssv", "Parameter"))
      if prefix:
        parameter_node.set("name", str(prefix) + "." + str(key))
      else:
        parameter_node.set("name", str(key))
      if description:
        parameter_node.set("description", description)

      parameter_type = ET.SubElement(parameter_node, namespace.tag("ssv", type.name))
      parameter_type.set("value", str(value))
      if unit is not None:
        parameter_type.set("unit", unit)
