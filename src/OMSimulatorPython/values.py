from lxml import etree as ET

from OMSimulator import namespace
from OMSimulator.unit import Unit


class Values:
  def __init__(self):
    # self.cref = cref
    # self.value = value
    self.allResources = {}
    self.realStartValues = {}
    self.integerStartValues = {}
    self.booleanStartValues = {}
    self.stringStartValues = {}

  def setReal(self, var):
    self.realStartValues[var.name] = var

  def setInteger(self, var):
    self.integerStartValues[var.name] = var

  def setBoolean(self, var):
    self.booleanStartValues[var.name] = var

  def setString(self, var):
    self.stringStartValues[var.name] = var

  def empty(self) -> bool:
    return not self.realStartValues and not self.integerStartValues and not self.booleanStartValues and not self.stringStartValues

  def getAllParameters(self):
    parameters = {}
    for store in [self.realStartValues, self.integerStartValues, self.booleanStartValues]:
      for key, value in store.items():
        parameters[key] = value.startValue
    return parameters

  def list(self, prefix = ""):
    if self.empty():
      return

    for key, var in self.realStartValues.items():
      print(f"|{prefix}   | * ({key}, {var.startValue}, Real)")
    for key, var in self.integerStartValues.items():
      print(f"|{prefix}   | * ({key}, {var.startValue}, Integer)")
    for key, var in self.booleanStartValues.items():
      print(f"|{prefix}   | * ({key}, {var.startValue}, Boolean)")

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

    self.add_parameters(parameters_node, self.realStartValues, "Real")
    self.add_parameters(parameters_node, self.integerStartValues, "Integer")
    self.add_parameters(parameters_node, self.booleanStartValues, "Boolean")
    self.exportUnitDefintions(parameter_set_node)

  def exportToSSV(self, node):
    if self.empty():
      return

    parameters_node = ET.SubElement(node, namespace.tag("ssv", "Parameters"))
    self.add_parameters(parameters_node, self.realStartValues, "Real")
    self.add_parameters(parameters_node, self.integerStartValues, "Integer")
    self.add_parameters(parameters_node, self.booleanStartValues, "Boolean")
    self.exportUnitDefintions(node)

  def add_parameters(self, parameters_node, values_dict, type_tag):
    """Generic function to add XML parameters based on the value type."""
    for key, var in values_dict.items():
      parameter_node = ET.SubElement(parameters_node, namespace.tag("ssv", "Parameter"))
      parameter_node.set("name", key)
      parameter_type = ET.SubElement(parameter_node, namespace.tag("ssv", type_tag))
      parameter_type.set("value", str(var.startValue))
      if var.unit:
        parameter_type.set("unit", var.unit)

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
