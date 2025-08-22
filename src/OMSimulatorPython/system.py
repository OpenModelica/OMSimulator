import json
import logging
from collections import defaultdict
from pathlib import Path

from lxml import etree as ET
from OMSimulator.component import Component
from OMSimulator.connection import Connection
from OMSimulator.connector import Connector
from OMSimulator.elementgeometry import ElementGeometry
from OMSimulator.ssm import SSM
from OMSimulator.values import Values

from OMSimulator import Capi, CRef, namespace, utils

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
    self.parameterMapping = SSM()
    self.parameterResources = []
    self.model = model
    self.elementgeometry = None
    self.systemgeometry = None
    self.solvers = list()

  @property
  def name(self):
    return self._name

  @name.setter
  def name(self, name: str):
    self._name = name

  @staticmethod
  def importFromNode(node, ssd, resources: dict | None = None):
    '''Imports a ssd:System'''
    try:
      system = System(node.get("name"))
      system.description = node.get("description")
      system.connectors = Connector.importFromNode(node)
      system.elementgeometry = ElementGeometry.importFromNode(node)
      system.systemgeometry = SystemGeometry.importFromNode(node)
      utils.parseParameterBindings(node, system, resources)
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
      if not self.parameterMapping.empty():
        print(f"{prefix} |-- Inline Parameter Mapping:")
        self.parameterMapping.list(prefix=prefix + " |-- |--")

    ## list parameteres in ssv files
    if len(self.parameterResources) > 0:
      for resource in self.parameterResources:
        for key, value in resource.items():
          print(f"{prefix} Parameter Bindings: {key}")
          if value:
            print(f"{prefix} |-- Parameter Mapping: {value}")

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
      component.fmuType = inst.fmuType if inst else None
      self.elements[first] = component
      return component

  def addSSVReference(self, cref: CRef, resource1: str, resource2: str | None = None):
    ## top level system
    if cref is None:
      self.parameterResources.append({resource1: resource2})
      return

    first = cref.first()

    match self.elements.get(first):
      case System():
        self.elements[first].addSSVReference(cref.pop_first(), resource1, resource2)
      case Component():
        self.elements[first].addSSVReference(resource1, resource2)
      case _:
        raise ValueError(f"Element '{first}' in system '{self.name}' is neither a System nor a Component")

  def swapSSVReference(self, cref: CRef, resource1: str, resource2: str):
    ## top level system
    if cref is None:
      self.removeSSVReference(cref, resource1)
      self.addSSVReference(cref, resource2)
      return

    first = cref.first()

    match self.elements.get(first):
      case System():
        self.elements[first].swapSSVReference(cref.pop_first(), resource1, resource2)
      case Component():
        self.elements[first].swapSSVReference(resource1, resource2)
      case _:
        raise ValueError(f"Element '{first}' in system '{self.name}' is neither a System nor a Component")

  def listSSVReference(self, cref: CRef):
    ## top level system
    if cref is None:
      return self.parameterResources

    first = cref.first()

    match self.elements.get(first):
      case System():
        return self.elements[first].listSSVReference(cref.pop_first())
      case Component():
        return self.elements[first].listSSVReference()
      case _:
        raise ValueError(f"Element '{first}' in system '{self.name}' is neither a System nor a Component")

  def _remove(self, resource: str, raise_error: bool = True):
    for entry in self.parameterResources:
      for key, _ in entry.items():
        if key == resource:
          del entry[key]
          return
    if raise_error:
      raise ValueError(f"Resource '{resource}' not found in {self.name}")

  def removeSSVReference(self, cref: CRef, resource: str):
    ## top level system
    if cref is None:
      return self._remove(resource)

    first = cref.first()

    match self.elements.get(first):
      case System():
        self.elements[first].removeSSVReference(cref.pop_first(), resource)
      case Component():
        self.elements[first].removeSSVReference(resource)
      case _:
        raise ValueError(f"Element '{first}' in system '{self.name}' is neither a System nor a Component")

  def deleteAllConnection(self, cref: CRef):
    """Deletes a connection from the system."""
    for connection in self.connections[:]:
      # Check if the connection is associated with the connector
      if str(cref) in {connection.startElement, connection.endElement, connection.startConnector, connection.endConnector}:
        self.connections.remove(connection)
    # Also delete connections in elements
    for key, element in self.elements.items():
      if isinstance(element, System):
        element.deleteAllConnection(cref)

  def delete(self, cref: CRef):
    """Removes the system and all its elements."""

    first = cref.first()

    ## Check if the cref is a top level system connector
    if self._connectorExists(first, delete = True):
      return

    match self.elements.get(first):
      case System():
        # If cref is root, delete the whole system
        if cref.is_root():
          del self.elements[first]
          self.deleteAllConnection(first)
          return
        # Otherwise, delete connector
        self.elements[first].delete(cref.pop_first())
        self.deleteAllConnection(cref.pop_first())
      case Component():
        if cref.is_root():
          del self.elements[first]
          self.deleteAllConnection(first)
          return
        self.elements[first].deleteConnector(cref.last())
        self.deleteAllConnection(cref.last())
      case _:
        raise ValueError(f"Element '{first}' in system '{self.name}' is neither a System nor a Component or a Connector")

  def deleteComponent(self, resource: str):
    """Removes an element from the system by matching FMU path."""
    keys_to_delete = [
        key for key, element in self.elements.items()
        if isinstance(element, Component) and str(element.fmuPath) == str(resource)
    ]

    for key in keys_to_delete:
      del self.elements[key]

  def deleteResource(self, resource: str):
    """Removes a resource from the SSP and its associated SSV references"""

    if Path(resource).suffix == '.fmu':
      self.deleteComponent(resource)
      return

    # Remove the ssv resource from the parameter resources
    if Path(resource).suffix == '.ssv':
      self._remove(resource, raise_error = False)
      for key, element in self.elements.items():
        if isinstance(element, System):
          element.removeSSVReference(resource)
        elif isinstance(element, Component):
          element.removeSSVReference(resource, raise_error = False)
        else:
          # Handle other types of elements if needed
          logger.error(f"Unknown element type '{type(element)}' for element '{key}'. Skipping deletion.")
    else:
      logger.error(f"Unsupported resource type '{Path(resource).suffix}' for deletion in system '{self.name}'")

  def exportSSVTemplateHelper(self, node, prefix = None):
    """Exports all parameters in ssp to an XML node."""
    self.value.add_parameters(node, prefix)
    for key, element in self.elements.items():
      if isinstance(element, System):
        element.exportSSVTemplateHelper(node, key)
      elif isinstance(element, Component):
        element.exportSSVTemplate(node, key)
      else:
        # Handle other types of elements if needed
        logger.error(f"Unknown element type '{type(element)}' for element '{key}'. Skipping export.")

  def exportSSVTemplate(self, cref: CRef, node):
    ## top level system
    if cref is None:
      self.value.add_parameters(node)
      return

    first = cref.first()

    match self.elements.get(first):
      case System():
        self.elements[first].exportSSVTemplate(cref.pop_first(), node)
      case Component():
        self.elements[first].exportSSVTemplate(node)
      case _:
        raise ValueError(f"Element '{first}' in system '{self.name}' is neither a System nor a Component")

  def exportSSMTemplateHelper(self, node, prefix = None):
    """Exports all connectors in ssp to an XML node."""
    self.parameterMapping.exportSSMTemplate(node, self.connectors, prefix)
    for key, element in self.elements.items():
      if isinstance(element, System):
        element.exportSSMTemplateHelper(node, key)
      elif isinstance(element, Component):
        element.exportSSMTemplate(node, key)
      else:
        # Handle other types of elements if needed
        logger.error(f"Unknown element type '{type(element)}' for element '{key}'. Skipping export.")


  def exportSSMTemplate(self, cref: CRef, node):
    ## top level system
    if cref is None:
      self.parameterMapping.exportSSMTemplate(node, self.connectors)
      return

    first = cref.first()

    match self.elements.get(first):
      case System():
        self.elements[first].exportSSMTemplate(cref.pop_first(), node)
      case Component():
        self.elements[first].exportSSMTemplate(node)
      case _:
        raise ValueError(f"Element '{first}' in system '{self.name}' is neither a System nor a Component")

  def _addConnection(self, cref1: CRef, cref2: CRef) -> None:
    first1 = cref1.first()
    first2 = cref2.first()

    # Both CRefs start in the same subsystem
    if first1 == first2:
      if first1 not in self.elements:
        raise ValueError(f"System '{first1}' not found in '{self.name}'")

      # Add the connection inside the subsystem
      self.elements[first1]._addConnection(cref1.pop_first(), cref2.pop_first())
    # Top level system connections
    else:
      (start_element, start_connector) = cref1.split()
      (end_element, end_connector) = cref2.split()

      # Add the connections to top level system
      self.addConnection(start_element, start_connector, end_element, end_connector)

  def addConnection(self, startElement : str, startConnector : str, endElement : str, endConnector : str):
    #TODO: Fix this check for Connection class
    #if (startElement, startConnector, endElement, endConnector) in self.connections:
    #  raise ValueError(f"Connection '{startElement}.{startConnector}' to '{endElement}.{endConnector}' already exists")

    #if (endElement, endConnector, startElement, startConnector) in self.connections:
    #  raise ValueError(f"Connection '{startElement}.{startConnector}' to '{endElement}.{endConnector}' already exists")

    self.connections.append(Connection(startElement, startConnector, endElement, endConnector))

  def _connectorExists(self, cref: CRef, delete = False) -> bool:
    """Check if a connector exists in the system."""
    for i, connector in enumerate(self.connectors):
      if connector.name == cref:
        if delete:
          del self.connectors[i]
          # Remove connections associated with this connector
          self.deleteAllConnection(cref)
        return True
    return False

  def _getComponentResourcePath(self, cref):
    element_name = cref.first()
    element = self.elements.get(element_name, None)

    ## check if element is a top level system connectors
    ## or allow non existing connectors to support parameter mapping throgh SSM inline or ssm file by checking if cref
    if self._connectorExists(cref) or cref.is_root():
      return

    if element is None:
      raise ValueError(f"Element '{element_name}' not found in System '{self.name}'")

    match element:
      case Component():
        return element.fmuPath
      case System():
        return element._getComponentResourcePath(cref.pop_first())
      case _:
        raise TypeError(f"Element '{element_name}' is not a Component or System, but {type(element)}")

  def setValue(self, cref: CRef, value, unit = None, description = None):
    first = cref.first()
    ## Check if the cref is a top level system connector
    ## or allow non existing connectors to support parameter mapping throgh SSM inline or ssm file by checking if cref
    if self._connectorExists(first) or cref.is_root():
      self.value.setValue(cref, value, unit)
      return

    match self.elements.get(first):
      case System():
        self.elements[first].setValue(cref.pop_first(), value, unit, description)
      case Component():
        self.elements[first].setValue(cref.last(), value, unit, description)
      case _:
        raise ValueError(f"Element '{first}' in system '{self.name}' is neither a System nor a Component or a Connector")

  def getValue(self, cref: CRef):
    first = cref.first()
    ## Check if the cref is a top level system connector
    ## or allow non existing connectors to support parameter mapping throgh SSM inline or ssm file by checking if cref
    if self._connectorExists(first) or cref.is_root():
      self.value.getValue(cref)
      return

    match self.elements.get(first):
      case System():
        self.elements[first].getValue(cref.pop_first())
      case Component():
        self.elements[first].getValue(cref.last())
      case _:
        raise ValueError(f"Element '{first}' in system '{self.name}' is neither a System nor a Component or a Connector")

  def mapParameter(self, cref: CRef, source: str, target: str):
    if cref is None:
      self.parameterMapping.mapParameter(source, target)
      return

    first = cref.first()

    match self.elements.get(first):
      case System():
        self.elements[first].mapParameter(cref.pop_first(), source, target)
      case Component():
        self.elements[first].mapParameter(source, target)
      case _:
        raise ValueError(f"Element '{first}' in system '{self.name}' is neither a System nor a Component or a Connector")

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

  def generateJson(self, resources: dict | None = None, tempdir : str | None = None) -> str:
    """Instantiates the system and its components."""
    data = {
        "simulation units": []
    }
    # Dict to group components by solver
    solver_groups = defaultdict(list)
    componentSolver = {}
    # dict to group connections by solver unit
    solver_connections = defaultdict(list)

    # process the elements
    self.processElements(self.elements, self.connections, data, solver_groups, componentSolver, solver_connections, resources, tempdir)

    ## group the simulation units
    for solver, components in solver_groups.items():
      #print(f"Processing solver: {solver} with components: {components}")
      unit = {
          "components": components,
          "connections": solver_connections.get(solver, [])
      }
      if solver is not None:
         unit["solver"] = {
            "name": solver,
            "method": "cvode",
            "tolerance": 1e-6
        }
      data["simulation units"].append(unit)

    # Add top-level simulation metadata
    data["result file"] = "simulation_result.csv"
    data["simulation settings"] = {
        "start time": 0,
        "stop time": 10, ## TODO get the stop time from the solver settings
        "tolerance": 1e-6
    }

    # Dump JSON
    json_string = json.dumps(data, indent=2)
    return json_string

  def processElements(self, elements_dict: dict, connections: list, data: dict, solver_groups : defaultdict, componentSolver : dict, solver_connections : defaultdict, resources :dict, tempdir : str, systemName = None):
    """Processes the elements and connections in the system."""
    for key, element in elements_dict.items():
      if isinstance(element, Component):
        if element.implementation is None:
          # get the fmuType from the resources
          fmu = resources.get(str(element.fmuPath))
          if fmu is None:
            raise ValueError(f"Resource '{element.fmuPath}' not found in component {element.name}")
          fmuType = fmu.fmuType
        else:
          fmuType = element.implementation
        solver_groups[element.solver].append({
            "name": [self.name] + ([systemName] if systemName else []) + [str(element.name)],
            "type": fmuType,
            "path": str(Path(tempdir, str(element.fmuPath))) if tempdir is not None else str(element.fmuPath)
        })
        componentSolver[str(element.name)] = element.solver
      elif isinstance(element, System):
        # recurse into subsystems
        self.processElements(element.elements, element.connections, data, solver_groups, componentSolver, solver_connections, resources, tempdir, systemName=str(element.name))

    for connection in connections:
      startElement = connection.startElement
      endElement = connection.endElement
      startSolver = componentSolver.get(startElement, None)
      endSolver = componentSolver.get(endElement, None)
      # print(f"{startElement},{endElement},{startSolver},{endSolver}")

      solver = None
      if startSolver == endSolver and startSolver is not None:
        solver = startSolver
      elif startSolver is None and endSolver is not None:
        solver = endSolver
      elif endSolver is None and startSolver is not None:
        solver = startSolver
      ##TODO group components and connection without solver information, right now they are grouped under NONE category
      solver_connections[solver].append({
            "start element": [self.name] + ([systemName] if systemName else []) + ([startElement] if startElement else []),
            "start connector": connection.startConnector,
            "end element": [self.name] + ([systemName] if systemName else []) + ([endElement] if endElement else []),
            "end connector": connection.endConnector
        })

  def export(self, root):
    node = ET.SubElement(root, namespace.tag("ssd", "System"), attrib={"name": str(self.name)})
    if self.description:
      node.set("description", self.description)
    if len(self.connectors) > 0 :
      connectors_node = ET.SubElement(node, namespace.tag("ssd", "Connectors"))
      for connector in self.connectors:
        connector.exportToSSD(connectors_node)

      if self.elementgeometry:
        self.elementgeometry.exportToSSD(node)

    ## export top level parameter bindings
    self.value.exportToSSD(node, self.parameterMapping)

    ## export parameters binding to ssd file with reference to ssv file
    if len(self.parameterResources) > 0:
      parameter_bindings_node = ET.SubElement(node, namespace.tag("ssd", "ParameterBindings"))
      for resource in self.parameterResources:
        for key, value in resource.items():
          parameter_binding_node = ET.SubElement(parameter_bindings_node, namespace.tag("ssd", "ParameterBinding"))
          parameter_binding_node.set("source", key)
          if value:
            parameter_mapping_node = ET.SubElement(parameter_binding_node, namespace.tag("ssd", "ParameterMapping"))
            parameter_mapping_node.set("source", value)

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
