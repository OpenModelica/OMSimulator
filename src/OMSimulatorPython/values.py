from lxml import etree as ET
from OMSimulator.unit import Unit

from OMSimulator import namespace


class Values:
  def __init__(self):
    self.start_values = {}

  def setValue(self, name, value, unit=None):
    if unit is not None and not isinstance(value, float):
      raise TypeError("Unit can only be set for Real values.")
    self.start_values[name] = (value, unit)

  def empty(self) -> bool:
    return not self.start_values

  def getAllParameters(self):
    return self.start_values

  def list(self, prefix = ""):
    if self.empty():
      return

    for key, (value, unit) in self.start_values.items():
      if isinstance(value, float):
        type_tag = "Real"
      elif isinstance(value, int):
        type_tag = "Integer"
      elif isinstance(value, bool):
        type_tag = "Boolean"
      elif isinstance(value, str):
        type_tag = "String"
      else:
        raise TypeError(f"Unsupported type: {type(value)}")
      print(f"{prefix} ({type_tag} {key}, {value}, {unit})")

  def exportToSSD(self, node):
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
    #self.exportUnitDefintions(parameter_set_node)

  def exportToSSV(self, node):
    if self.empty():
      return

    parameters_node = ET.SubElement(node, namespace.tag("ssv", "Parameters"))
    self.add_parameters(parameters_node)
    #self.exportUnitDefintions(node)

  def add_parameters(self, parameters_node):
    """Generic function to add XML parameters based on the value type."""
    for key, (value, unit) in self.start_values.items():
      parameter_node = ET.SubElement(parameters_node, namespace.tag("ssv", "Parameter"))
      parameter_node.set("name", str(key))
      if isinstance(value, float):
        type_tag = "Real"
      elif isinstance(value, bool): ## check for boolean first, as is a subclass of int, so check for boolean first
        type_tag = "Boolean"
      elif isinstance(value, int):
        type_tag = "Integer"
      elif isinstance(value, str):
        type_tag = "String"
      else:
        raise TypeError(f"Unsupported type: {type(value)}")
      parameter_type = ET.SubElement(parameter_node, namespace.tag("ssv", type_tag))
      parameter_type.set("value", str(value))
      if unit is not None:
        parameter_type.set("unit", unit)

  def exportUnitDefintions(self, node):
    unitsToExport = {}
    for key, var in self.realStartValues.items():
      for item in var.unitDefinition:
        if item.name not in unitsToExport:
          unitsToExport[item.name] = item.baseUnits

    if len(unitsToExport) > 0 :
      ssv_unit_node = ET.SubElement(node, namespace.tag("ssv", "Units"))
      unit = Unit()
      unit.exportToSSD(unitsToExport, ssv_unit_node)
