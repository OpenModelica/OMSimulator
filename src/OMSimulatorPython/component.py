from pathlib import Path

from lxml import etree as ET
from OMSimulator.cref import CRef
from OMSimulator.values import Values
from OMSimulator.elementgeometry import ElementGeometry
from OMSimulator import namespace

class Component:
  def __init__(self, name: CRef, fmuType : str, fmuPath: Path | str, connectors=None, unitDefinitions=None):
    self.name = CRef(name)
    self.fmuType = fmuType
    self.fmuPath = Path(fmuPath)
    self.connectors = connectors or list()
    self.unitDefinitions = unitDefinitions or list()
    self.elementgeometry = None
    self.description = None
    self.value = Values()
    self.solver = None
    self.parameterResources = []

  def addConnector(self, connector):
    if connector in self.connectors:
      raise ValueError(f"Connector '{connector.name}' already exists in {self.name}")
    self.connectors.append(connector)

  def addSSVReference(self, resource: str):
    self.parameterResources.append(resource)

  def swapSSVReference(self, resource1: str, resource2: str):
    self.removeSSVReference(resource1)
    self.addSSVReference(resource2)

  def listSSVReference(self):
    return self.parameterResources

  def removeSSVReference(self, resource: str):
    if resource in self.parameterResources:
      self.parameterResources.remove(resource)
    else:
      raise ValueError(f"Resource '{resource}' not found in {self.name}")

  def list(self, prefix=""):
    print(f"{prefix} FMU: {self.name} '{self.description}'")
    prefix += ' |--'
    print(f"{prefix} path: {self.fmuPath}")

    if len(self.connectors) > 0:
      print(f"{prefix} Connectors:")
      for connector in self.connectors:
        connector.list(prefix=prefix + " |--")

    ## list component element geometry
    if self.elementgeometry:
      print(f"{prefix} ElementGeometry:")
      self.elementgeometry.list(prefix=prefix + " |--")

    ## list parameters inline
    if not self.value.empty():
      print(f"{prefix} Inline Parameter Bindings:")
      self.value.list(prefix=prefix + " |--")

    ## list unit definitions
    if len(self.unitDefinitions) > 0:
      print(f"{prefix} |-- UnitDefinitions:")
      for unit in self.unitDefinitions:
        unit.list(prefix=prefix + " |-- |--")

    ## list parameteres in ssv files
    if len(self.parameterResources) > 0:
      for resource in self.parameterResources:
        print(f"{prefix} Parameter Bindings: {resource}")

    ## list solver settings
    if self.solver:
      print(f"{prefix} Solver Settings:")
      print(f"{prefix} |-- name: {self.solver}")

  def exportToSSD(self, node):
    component_node = ET.SubElement(node, namespace.tag("ssd", "Component"))
    component_node.set("name", str(self.name))
    component_node.set("type", "application/x-fmu-sharedlibrary")
    component_node.set("source", str(self.fmuPath))
    if self.description:
      component_node.set("description", self.description)

    if len(self.connectors) > 0:
      connectors_node = ET.SubElement(component_node, namespace.tag("ssd", "Connectors"))
      ## export component connectors
      for connector in self.connectors:
        connector.exportToSSD(connectors_node)

    ## export component element geometry
    if self.elementgeometry:
      self.elementgeometry.exportToSSD(component_node)

    ## export parameter bindings
    self.value.exportToSSD(component_node, self.unitDefinitions)

    ## export parameters binding to ssd file with reference to ssv file
    if len(self.parameterResources) > 0:
      parameter_bindings_node = ET.SubElement(component_node, namespace.tag("ssd", "ParameterBindings"))
      for resource in self.parameterResources:
        parameter_binding_node = ET.SubElement(parameter_bindings_node, namespace.tag("ssd", "ParameterBinding"))
        parameter_binding_node.set("source", resource)

    ## export Annotations
    if self.solver:
      from OMSimulator import utils
      utils.exportAnnotations(component_node, self.solver)

  def setValue(self, cref:str, value, unit=None, description = None):
    self.value.setValue(cref, value, unit, description)

  def setSolver(self, name: str):
    self.solver = name