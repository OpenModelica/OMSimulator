from lxml import etree as ET

from OMSimulator import namespace
from OMSimulator.values import Values
from OMSimulator.variables import Variable


class SSV:
  def __init__(self, filename:str):
    self.filename = filename
    self.value = Values()

  def setValue(self, cref:str, value):
    var = Variable(cref)
    var.startValue = value

    if isinstance(value, float):
      self.value.setReal(var)
    elif isinstance(value, int):
      self.value.setInteger(var)
    elif isinstance(value, bool):
      self.value.setBoolean(var)
    elif isinstance(value, str):
      self.value.setString(var)

  def exportToSSD(self, node):
    parameter_bindings_node = ET.SubElement(node, namespace.tag("ssd", "ParameterBindings"))
    parameter_binding_node = ET.SubElement(parameter_bindings_node, namespace.tag("ssd", "ParameterBinding"))
    parameter_binding_node.set("source", "resources/"+ self.filename)

  def list(self, prefix = ""):
    self.value.list(prefix)

  def exportToSSV(self, node):
    self.value.exportToSSV(node)
