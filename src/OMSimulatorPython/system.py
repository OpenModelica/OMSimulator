import logging

from lxml import etree as ET
from OMSimulator.component import Component
from OMSimulator.fmu import FMU
from OMSimulator.values import Values

from OMSimulator import CRef, namespace, utils

logger = logging.getLogger(__name__)


class System:
  def __init__(self, name : str, model=None):
    from OMSimulator.ssp import SSP
    self._name = name
    self.connectors = list()
    self.elements = dict()
    self.connections = list()
    self.value = Values()
    self.parameterResources = dict()
    self.model = model

  @property
  def name(self):
    return self._name

  @staticmethod
  def importFromNode(node, ssd):
    '''Imports a ssd:System'''
    try:
      temp_dir = ssd._filename.parent
      system = System(node.get("name"))
      system.connectors = utils.parseConnectors(node)
      utils.parseParameterBindings(node, ssd, temp_dir)
      system.elements = utils.parseElements(node)
      utils.parseConnection(node, system)
      return system

    except ET.ParseError as e:
      logger.error(f"Error parsing System: {e}")
      raise

  def list(self, prefix=""):
    print(f"{prefix} System: {self.name}")
    print(f"{prefix} Connectors:")
    for connector in self.connectors:
      connector.list(prefix=prefix + " |--")

    ## list parameters inline
    if not self.value.empty():
      print(f"{prefix} Inline Parameter Bindings:")
      self.value.list(prefix=prefix + " |--")

    ## list parameteres in ssv files
    if len(self.parameterResources) > 0:
      for key, resources in self.parameterResources.items():
        print(f"{prefix} Parameter Bindings: {resources.filename}")
        resources.list(prefix=prefix + " |--")

    ## list elements
    if len(self.elements) > 0:
      print(f"{prefix} Elements:")
      for element in self.elements.values():
        element.list(prefix=prefix + " |--")

    ## list connections
    if len(self.connections) > 0:
      print(f"{prefix} Connections:")
      for connection in self.connections:
        print(f"{prefix} |-- {connection[0]}.{connection[1]} -> {connection[2]}.{connection[3]}")
        pass

  def addSystem(self, cref: CRef):
    first = cref.first()
    if not cref.is_root():
      if first not in self.elements:
        raise ValueError(f"System '{first}' not found in {self.name}")
      self.elements[first].addSystem(cref.pop_first())
    else:
      if first in self.elements:
        raise ValueError(f"System '{first}' already exists in {self.name}")
      self.elements[first] = System(first, model=self.model)

  def addComponent(self, cref: CRef, resource: str, inst = None | FMU):
    first = cref.first()
    if not cref.is_root():
      if first not in self.elements:
        raise ValueError(f"System '{first}' not found in '{self.name}'")
      return self.elements[first].addComponent(cref.pop_first(), resource, inst)
    else:
      if first in self.elements:
        raise ValueError(f"Component '{first}' already exists in {self.name}")
      connectors = inst.makeConnectors() if inst else list()
      component = Component(first, resource, connectors)
      self.elements[first] = component
      return component

  def addConnection(self, startElement : str, startConnector : str, endElement : str, endConnector : str):
    if (startElement, startConnector, endElement, endConnector) in self.connections:
      raise ValueError(f"Connection '{startElement}.{startConnector}' to '{endElement}.{endConnector}' already exists")

    if (endElement, endConnector, startElement, startConnector) in self.connections:
      raise ValueError(f"Connection '{startElement}.{startConnector}' to '{endElement}.{endConnector}' already exists")

    self.connections.append((startElement, startConnector, endElement, endConnector))

  def _getComponentResourcePath(self, cref):
    first = cref.first()
    if not cref.is_root():
      if first not in self.elements:
        raise ValueError(f"System '{first}' not found in '{self.name}'")

    return self.elements[first].fmuPath

  def setValue(self, cref: CRef, value, unit = None):
    first = cref.first()
    if not cref.is_root():
      if first not in self.elements:
        raise ValueError(f"System '{first}' not found in '{self.name}'")

    self.elements[first].setValue(cref.last(), value, unit)

  def export(self, root):
    node = ET.SubElement(root, namespace.tag("ssd", "System"), attrib={"name": self.name})

    if len(self.connectors) > 0 :
      connectors_node = ET.SubElement(node, namespace.tag("ssd", "Connectors"))
      for connector in self.connectors:
        connector.exportToSSD(connectors_node)
      ## export parameter bindings
      self.value.exportToSSD(node)

      ## export parameters binding to ssd file with reference to ssv file
      if len(self.parameterResources) > 0:
        for key, resources in self.parameterResources.items():
          resources.exportToSSD(node)

    ## export elements
    if len(self.elements) > 0:
      element_node = ET.SubElement(node, namespace.tag("ssd", "Elements"))
      for key, element in self.elements.items():
        element.exportToSSD(element_node)
        #TODO
        ## export parameter resources e.g ssv

    ## export connections
    if len(self.connections) > 0:
      connections_node = ET.SubElement(node, namespace.tag("ssd", "Connections"))
      for connection in self.connections:
        connection.exportToSSD(connections_node)

    return node
