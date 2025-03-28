from pathlib import Path

from lxml import etree as ET
from OMSimulator.cref import CRef
from OMSimulator.values import Values
from OMSimulator.variable import Variable
from OMSimulator import namespace


class Component:
  def __init__(self, name: CRef, fmuPath: Path | str, connectors=None, unitDefinitions=None):
    self.name = CRef(name)
    self.fmuPath = Path(fmuPath)
    self.connectors = connectors or list()
    self.unitDefinitions = unitDefinitions or list()
    self.value = Values() ## TODO propogate Values
    self.parameterResources = {} ## TODO handle ssv resources

  def list(self, prefix=""):
    print(f"{prefix} FMU: ({self.name})")
    print(f"{prefix}   path: {self.fmuPath}")

    if len(self.connectors) > 0:
      print(f"{prefix} Connectors:")
      for connector in self.connectors:
        connector.list(prefix=prefix + " |--")

    ## list parameters inline
    if not self.value.empty():
      print(f"{prefix} Inline Parameter Bindings:")
      self.value.list(prefix=prefix + " |--")

    ## list parameteres in ssv files
    if len(self.parameterResources) > 0:
      for resources in self.parameterResources.values():
        print(f"{prefix} Parameter Bindings: {resources.filename}")
        resources.list(prefix=prefix + " |--")

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

  def setValue(self, cref:str, value, unit : str = None):
    if isinstance(value, float):
      self.value.setReal(cref, value, unit)
    elif isinstance(value, int):
      self.value.setInteger(cref, value)
    elif isinstance(value, bool):
      self.value.setBoolean(cref, value)
    elif isinstance(value, str):
      self.value.setString(cref, value)
