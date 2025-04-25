import logging

from lxml import etree as ET
from OMSimulator.component import Component
from OMSimulator.connector import Connector
from OMSimulator.connection import Connection
from OMSimulator.fmu import FMU
from OMSimulator.values import Values
from OMSimulator.ssv import SSV
from OMSimulator.elementgeometry import ElementGeometry

from OMSimulator import CRef, namespace, utils

logger = logging.getLogger(__name__)

class SystemGeometry:
  def __init__(self, x1 : float | None = None, y1 : float | None = None, x2 : float | None = None, y2 : float | None = None):
    self._x1 = x1
    self._y1 = y1
    self._x2 = x2
    self._y2 = y2

  @property
  def x1(self):
    return self._x1

  @x1.setter
  def x1(self, value: float):
    self._x1 = value

  @property
  def y1(self):
    return self._y1

  @y1.setter
  def y1(self, value: float):
    self._y1 = value

  @property
  def x2(self):
    return self._x2

  @x2.setter
  def x2(self, value: float):
    self._x2 = value

  @property
  def y2(self):
    return self._y2

  @y2.setter
  def y2(self, value: float):
    self._y2 = value

  def list(self, prefix=""):
    print(f"{prefix} (x1:{self.x1}, y1:{self.y1}, x2:{self.x2}, y2:{self.y2})")

  def exportToSSD(self, node):
    """Exports the system geometry to an XML node."""
    system_geometry_node = ET.SubElement(node, namespace.tag("ssd", "SystemGeometry"))
    system_geometry_node.set("x1", str(self.x1))
    system_geometry_node.set("y1", str(self.y1))
    system_geometry_node.set("x2", str(self.x2))
    system_geometry_node.set("y2", str(self.y2))

  @staticmethod
  def importFromNode(node):
    """Imports system geometry from an XML node."""
    system_geometry_node = node.find("ssd:SystemGeometry", namespaces=namespace.ns)
    if system_geometry_node is None:
      return None
    x1 = float(system_geometry_node.get("x1", 0))
    y1 = float(system_geometry_node.get("y1", 0))
    x2 = float(system_geometry_node.get("x2", 0))
    y2 = float(system_geometry_node.get("y2", 0))

    return SystemGeometry(x1, y1, x2, y2)

class System:
  def __init__(self, name : str, model=None):
    from OMSimulator.ssp import SSP
    self._name = name
    self.description = None
    self.connectors = list()
    self.elements = dict()
    self.connections = list()
    self.value = Values()
    self.parameterResources = dict()
    self.model = model
    self.elementgeometry = None
    self.systemgeometry = None
    self.solvers = list()

  @property
  def name(self):
    return self._name

  @staticmethod
  def importFromNode(node, ssd, resources: dict | None = None):
    '''Imports a ssd:System'''
    try:
      temp_dir = ssd._filename.parent
      system = System(node.get("name"))
      system.description = node.get("description")
      system.connectors = Connector.importFromNode(node)
      system.elementgeometry = ElementGeometry.importFromNode(node)
      system.systemgeometry = SystemGeometry.importFromNode(node)
      utils.parseParameterBindings(node, ssd, resources)
      system.elements = utils.parseElements(node, resources)
      system.solvers = utils.parseAnnotations(node)
      Connection.importFromNode(node, system)
      return system

    except ET.ParseError as e:
      logger.error(f"Error parsing System: {e}")
      raise

  def addConnector(self, connector):
    if connector in self.connectors:
      raise ValueError(f"Connector '{connector.name}' already exists in {self.name}")
    self.connectors.append(connector)

  def list(self, prefix=""):
    print(f"{prefix} System: {self.name} '{self.description}'")
    prefix += ' |--'
    print(f"{prefix} Connectors:")
    for connector in self.connectors:
      connector.list(prefix=prefix + " |--")

    if self.elementgeometry:
      print(f"{prefix} ElementGeometry:")
      self.elementgeometry.list(prefix=prefix + " |--")

    ## list parameters inline
    if not self.value.empty():
      print(f"{prefix} Inline Parameter Bindings:")
      self.value.list(prefix=prefix + " |--")

    ## list parameteres in ssv files
    if len(self.parameterResources) > 0:
      for key, resources in self.parameterResources.items():
        print(f"{prefix} Parameter Bindings: {resources.filename.name}")
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
        connection.list(prefix=prefix + " |--")

    ## list system geometry
    if self.systemgeometry:
      print(f"{prefix} SystemGeometry:")
      self.systemgeometry.list(prefix=prefix + " |--")

    ## list solver options
    if self.solvers:
      print(f"{prefix} Solver Settings:")
      for solver in self.solvers:
        kv_list = [f"{k}={v}" for k, v in solver.items()]
        print(f"{prefix} |-- ({', '.join(kv_list)})")

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

  def addComponent(self, cref: CRef, resource: str, inst = None):
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

  def addSSV(self, cref: CRef, resource: str):
    first = cref.first()
    if not cref.is_root():
      if first not in self.elements:
        raise ValueError(f"System '{first}' not found in '{self.name}'")
      self.elements[first].addSSV(cref.pop_first(), resource)
    else:
      if first not in self.elements:
        raise ValueError(f"Component '{first}' not found in {self.name}")
      self.elements[first].addSSV(resource)

  def split_cref(self, cref: CRef):
    if cref.is_root():
      return ("", str(cref))
    else:
      return (cref.first(), cref.last())

  def _addConnection(self, cref1: CRef, cref2: CRef):
    first1 = cref1.first()
    first2 = cref2.first()

    # Both CRefs start in the same subsystem
    if first1 == first2:
      if first1 not in self.elements:
          raise ValueError(f"System '{first1}' not found in '{self.name}'")
      if first2 not in self.elements:
          raise ValueError(f"System '{first2}' not found in '{self.name}'")

      (start_element, start_connector) = self.split_cref(cref1.pop_first())
      (end_element, end_connector) = self.split_cref(cref2.pop_first())
      # Add the connection inside the subsystem
      self.elements[first1].addConnection(start_element, start_connector, end_element, end_connector)
    # Top level system connections
    else:
      (start_element, start_connector) = self.split_cref(cref1)
      (end_element, end_connector) = self.split_cref(cref2)
      # Add the connections to top level system
      self.addConnection(start_element, start_connector, end_element, end_connector)

  def addConnection(self, startElement : str, startConnector : str, endElement : str, endConnector : str):
    #TODO: Fix this check for Connection class
    #if (startElement, startConnector, endElement, endConnector) in self.connections:
    #  raise ValueError(f"Connection '{startElement}.{startConnector}' to '{endElement}.{endConnector}' already exists")

    #if (endElement, endConnector, startElement, startConnector) in self.connections:
    #  raise ValueError(f"Connection '{startElement}.{startConnector}' to '{endElement}.{endConnector}' already exists")

    self.connections.append(Connection(startElement, startConnector, endElement, endConnector))

  def _getComponentResourcePath(self, cref):
    element_name = cref.first()
    element = self.elements.get(element_name, None)

    if element is None:
      raise ValueError(f"Element '{element_name}' not found in System '{self.name}'")

    match element:
      case Component():
        return element.fmuPath
      case System():
        return element._getComponentResourcePath(cref.pop_first())
      case _:
        raise TypeError(f"Element '{element_name}' is not a Component or System, but {type(element)}")

  def setValue(self, cref: CRef, value, unit = None):
    first = cref.first()
    if not cref.is_root():
      if first not in self.elements:
        raise ValueError(f"System '{first}' not found in '{self.name}'")

    self.elements[first].setValue(cref.last(), value, unit)

  def setSolver(self, cref: CRef, name: str):
    first = cref.first()
    if not cref.is_root():
      if first not in self.elements:
        raise ValueError(f"System '{first}' not found in '{self.name}'")
      self.elements[first].setSolver(cref.pop_first(), name)
    else:
      if first not in self.elements:
        raise ValueError(f"Component '{first}' not found in {self.name}")
      self.elements[first].setSolver(name)

  def export(self, root):
    node = ET.SubElement(root, namespace.tag("ssd", "System"), attrib={"name": self.name})
    if self.description:
      node.set("description", self.description)
    if len(self.connectors) > 0 :
      connectors_node = ET.SubElement(node, namespace.tag("ssd", "Connectors"))
      for connector in self.connectors:
        connector.exportToSSD(connectors_node)

      if self.elementgeometry:
        self.elementgeometry.exportToSSD(node)

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
        if isinstance(element, System):
          element.export(element_node)
        elif isinstance(element, Component):
          element.exportToSSD(element_node)
        else:
          # Handle other types of elements if needed
          logger.error(f"Unknown element type '{type(element)}' for element '{key}'. Skipping export.")

    ## export connections
    if len(self.connections) > 0:
      connections_node = ET.SubElement(node, namespace.tag("ssd", "Connections"))
      for connection in self.connections:
        connection.exportToSSD(connections_node)

    ## export ssd annotations
    if self.solvers:
      utils.exportAnnotations(node, self.solvers)

    return node
