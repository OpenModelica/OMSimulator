from lxml import etree as ET
from OMSimulator.unit import Unit
from OMSimulator.ssm import SSM
from OMSimulator import namespace
from OMSimulator.variable import Float32, Float64, Int8, Int16, Int32, Int64, UInt8, UInt16, UInt32, UInt64

class Values:
  def __init__(self):
    self.start_values = {}

  def setValue(self, name, value, unit=None, description=None):
    if unit is not None and not isinstance(value, float):
      raise TypeError("Unit can only be set for Real values.")
    self.start_values[name] = (value, unit, description)

  def getValue(self, name):
    return self.start_values.get(name)

  def empty(self) -> bool:
    return not self.start_values

  def getAllParameters(self):
    return self.start_values

  def list(self, prefix = ""):
    if self.empty():
      return

    for key, (value, unit, description) in self.start_values.items():
      type_tag = self._getVariableType(value)
      print(f"{prefix} ({type_tag} {key}, {str(value.value if hasattr(value, 'value') else value)}, {unit}, '{description}')")

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
    for key, (value, unit, description) in self.start_values.items():
      parameter_node = ET.SubElement(parameters_node, namespace.tag("ssv", "Parameter"))
      if prefix:
        parameter_node.set("name", str(prefix) + "." + str(key))
      else:
        parameter_node.set("name", str(key))
      if description:
        parameter_node.set("description", description)

      type_tag = self._getVariableType(value)

      parameter_type = ET.SubElement(parameter_node, namespace.tag("ssv", type_tag))
      parameter_type.set("value", str(value.value if hasattr(value, 'value') else value))
      if unit is not None:
        parameter_type.set("unit", unit)

  def _getVariableType(self, value):
    match value:
      case float():
        return "Real"
      case bool():  # Check for boolean first, because it is a subclass of int
        return "Boolean"
      case int():
        return "Integer"
      case str():
        return "String"
      ## handle fmi3 types
      case Float32():
        return "Float32"
      case Float64():
        return "Float64"
      case Int8():
        return "Int8"
      case Int16():
        return "Int16"
      case Int32():
        return "Int32"
      case Int64():
        return "Int64"
      case UInt8():
        return "UInt8"
      case UInt16():
        return "UInt16"
      case UInt32():
        return "UInt32"
      case UInt64():
        return "UInt64"
      case _:
          raise TypeError(f"Unsupported type: {type(value)}")