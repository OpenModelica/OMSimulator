from pathlib import Path

from lxml import etree as ET
from OMSimulator.cref import CRef
from OMSimulator.values import Values

from OMSimulator import namespace


class Component:
  def __init__(self, name: CRef, fmuPath: Path | str, connectors=None):
    self.name = CRef(name)
    self.fmuPath = Path(fmuPath)
    self.connectors = connectors or list()
    self.value = Values() ## TODO propogate Values
    self.parameterResources = {} ## TODO handle ssv resources

  def list(self, prefix=""):
    print(f"|{prefix}  |-- FMU: ({self.name})")
    print(f"|{prefix}  |   |-- path: {self.fmuPath}")

    if len(self.connectors) > 0:
      print(f"|{prefix}  |   |-- Connectors:")
      ## export component connectors
      last_prefix = prefix + "  |   |  "  # This is the prefix for nested elements
      for connector in self.connectors:
        connector.list(prefix = last_prefix)

    last_prefix = prefix + "   |       "  # This is the prefix for nested elements

    ## list parameters inline
    if not self.value.empty():
      print(f"|{prefix}  |   |-- ParameterBindings:")
      print(f"|{prefix}  |   |   |-- inline:")
      last_prefix = prefix + "  |   |"  # This is the prefix for nested elements

      self.value.list(prefix=last_prefix)

    ## list parameteres in ssv files
    if len(self.parameterResources) > 0:
      for key, resources in self.parameterResources.items():
        print(f"|{prefix}  |   |-- ParameterBindings:")
        print(f"|{prefix}  |   |   |-- {resources.filename}:")
        last_prefix = prefix + "  |   |"  # This is the prefix for nested elements
        resources.list(prefix = last_prefix)

  def exportToSSD(self, node):
    component_node = ET.SubElement(node, namespace.tag("ssd", "Component"))
    component_node.set("name", str(self.name))
    component_node.set("type", "application/x-fmu-sharedlibrary")
    component_node.set("source", str(self.fmuPath))

    if len(self.connectors) > 0:
      connectors_node = ET.SubElement(component_node, namespace.tag("ssd", "Connectors"))
      ## export component connectors
      for connector in self.connectors:
        connector.exportToSSD(connectors_node)

    ## export parameter bindings
    self.value.exportToSSD(component_node)

    ## export parameters binding to ssd file with reference to ssv file
    if len(self.parameterResources) > 0:
      for key, resources in self.parameterResources.items():
        resources.exportToSSD(component_node)
