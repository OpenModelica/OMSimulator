import logging
import os

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
    self.components = dict()
    self.connections = list()
    self.value = Values()
    self.parameterResources = dict()
    self.model = model

  @staticmethod
  def importFromNode(node, ssd):
    '''Imports a ssd:System'''
    try:
      temp_dir, _ = os.path.split(ssd._filename)
      system = System(node.get("name"))
      system.connectors = utils.parseConnectors(node)
      utils.parseParameterBindings(node, ssd, temp_dir)
      system.components = utils.parseElements(node)
      utils.parseConnection(node, system)
      return system

    except ET.ParseError as e:
      logger.error(f"Error parsing System: {e}")
      raise

  def list(self, prefix=""):
    print(f"{prefix}|--   System: {self._name}")
    print(f"{prefix}|--   Connectors:")
    last_prefix = prefix + "     |"  # This is the prefix for nested elements
    for connector in self.connectors:
      connector.list(prefix=last_prefix)

    ## list parameters inline
    if not self.value.empty():
      print(f"{prefix}|--   ParameterBindings:")
      print(f"{prefix}|     |-- inline:")
      last_prefix = prefix + "     |"  # This is the prefix for nested elements
      self.value.list(prefix= last_prefix)

    ## list parameteres in ssv files
    if len(self.parameterResources) > 0:
      for key, resources in self.parameterResources.items():
        print(f"{prefix}|--   ParameterBindings:")
        print(f"{prefix}|     |-- {resources.filename}:")
        last_prefix = prefix + "     |"  # This is the prefix for nested elements
        resources.list(prefix = last_prefix)

    ## list elements
    if len(self.components) > 0:
      print(f"{prefix}|--   Components:")
      last_prefix = prefix + "     |"  # This is the prefix for nested elements
      for key, component in self.components.items():
        # print(key, values)
        component.list(last_prefix)

    ## list connections
    if len(self.connections) > 0:
      print(f"{prefix}|--   Connections:")
      last_prefix = prefix + "     |"  # This is the prefix for nested elements
      for connection in self.connections:
        connection.list(prefix=last_prefix)
        pass

  def addComponent(self, cref: CRef, resource: str, inst = None | FMU):
    name = cref.pop_first(first=self._name)

    if not name.is_root():
      raise ValueError(f"Invalid component reference: {cref}")

    connectors = inst.makeConnectors() if inst else list()
    component = Component(name, resource, connectors)
    self.components[name] = component

  def export(self, root):
    node = ET.SubElement(root, namespace.tag("ssd", "System"), attrib={"name": self._name})

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
    if len(self.components) > 0:
      element_node = ET.SubElement(node, namespace.tag("ssd", "Elements"))
      for key, component in self.components.items():
        component.exportToSSD(element_node)
        #TODO
        ## export parameter resources e.g ssv

    ## export connections
    if len(self.connections) > 0:
      connections_node = ET.SubElement(node, namespace.tag("ssd", "Connections"))
      for connection in self.connections:
        connection.exportToSSD(connections_node)

    return node
