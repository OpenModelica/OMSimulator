# This file is part of OpenModelica.
#
# Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
# c/o Linköpings universitet, Department of Computer and Information Science,
# SE-58183 Linköping, Sweden.
#
# All rights reserved.
#
# THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
# THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
# VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
#
# The OpenModelica software and the OSMC (Open Source Modelica Consortium)
# Public License (OSMC-PL) are obtained from OSMC, either from the above
# address, from the URLs:
# http://www.openmodelica.org or
# https://github.com/OpenModelica/ or
# http://www.ida.liu.se/projects/OpenModelica,
# and in the OpenModelica distribution.
#
# GNU AGPL version 3 is obtained from:
# https://www.gnu.org/licenses/licenses.html#GPL
#
# This program is distributed WITHOUT ANY WARRANTY; without
# even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
# IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
#
# See the full OSMC Public License conditions for more details.

import json
import logging
from collections import defaultdict
from pathlib import Path

from lxml import etree as ET
from OMSimulator.component import Component
from OMSimulator.componenttable import ComponentTable, ResultReader
from OMSimulator.connection import Connection
from OMSimulator.connector import Connector
from OMSimulator.elementgeometry import ElementGeometry
from OMSimulator.fmu import FMU
from OMSimulator.ssm import SSM
from OMSimulator.dcp import DCP
from OMSimulator.values import Values
from OMSimulator.variable import Causality, SignalType

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
    self.metaDataResources = []
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
      utils.parseMetaData(node, system, resources)
      system.elements = utils.parseElements(node, resources)
      system.solvers = utils.parseSimulationInformation(node)
      Connection.importFromNode(node, system)
      return system

    except ET.ParseError as e:
      logger.error(f"Error parsing System: {e}")
      raise

  def addConnectorHelper(self, cref: CRef, connector: Connector):
    if cref is None:
      self.addConnector(connector)
      return

    first = cref.first()

    match self.elements.get(first):
      case System():
        self.elements[first].addConnectorHelper(cref.pop_first(), connector)

  def getConnector(self, cref: CRef):
    first = cref.first()
    ## Check if the cref is a top level system connector
    ## or allow non existing connectors to support parameter mapping throgh SSM inline or ssm file by checking if cref
    connector = self._connectorExists(first)
    if connector is not None:
      return connector

    match self.elements.get(first):
      case System():
        return self.elements[first].getConnector(cref.pop_first())
      case Component():
        return self.elements[first].getConnector(cref.pop_first())

  def split_cref(self, cref: CRef):
    head = cref.first()
    tail = cref.pop_first()

    # connector-only cref like ".input"
    if tail is None:
      return "", str(head)

    return (head, tail)

  def getConnection(self, crefA: CRef, crefB: CRef):
    (headA, tailA) = self.split_cref(crefA)
    (headB, tailB) = self.split_cref(crefB)

    for connection in self.connections:
      if (str(connection.startElement) == str(headA) and str(connection.startConnector) == str(tailA)) and (str(connection.endElement) == str(headB) and str(connection.endConnector) == str(tailB)):
        return connection
      if (str(connection.startElement) == str(headB) and str(connection.startConnector) == str(tailB)) and (str(connection.endElement) == str(headA) and str(connection.endConnector) == str(tailA)):
        return connection

    ## recurse into subsystems to find the connection
    for element in self.elements.values():
      if isinstance(element, System):
        connection = element.getConnection(crefA.pop_first(), crefB.pop_first())
        if connection:
          return connection

    return None

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

    ## list metadata resources
    if len(self.metaDataResources) > 0:
      for resources in self.metaDataResources:
        print(f"{prefix} MetaData: {resources.get('source')}")

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
      if isinstance(inst, FMU) or (inst is None and resource.endswith(".fmu")):
        connectors = inst.makeConnectors() if inst else list()
        unitDefinitions = inst._unitDefinitions if inst else list()
        enumerationDefinitions = inst._enumerationDefinitions if inst else list()
        fmuType = inst._fmuType if inst else None
        component = Component(first, resource, fmuType, connectors, unitDefinitions, enumerationDefinitions)
        component.fmuType = inst.fmuType if inst else None
        component.fmu = inst  # keep FMU reference for modeldescription start value fallback
        self.elements[first] = component
        return component
      elif isinstance(inst, ResultReader) or (inst is None and resource.endswith(".csv")):
        componenttable = ComponentTable(first, resource, inst.connectors)
        self.elements[first] = componenttable
        return componenttable
      elif isinstance(inst, DCP) or (inst is None and resource.endswith(".dcp")):
        connectors = inst.makeConnectors() if inst else list()
        unitDefinitions =  list()
        enumerationDefinitions = list()
        component = Component(first, resource, None, connectors, unitDefinitions, enumerationDefinitions)
        component.fmuType = inst.fmuType if inst else None
        self.elements[first] = component
        return component
      else:
        raise TypeError( f"Unknown component instance for '{first}' in '{self.name}'. "
                 f"Please add the component from the top-level model.")

  def replaceComponent(self, cref: CRef, resource: str, inst=None, dryRun: bool = False):
    first = cref.first()

    # recurse into subsystem before any type checks
    if not cref.is_root():
      if first not in self.elements:
        raise ValueError(f"System '{first}' not found in '{self.name}'")
      return self.elements[first].replaceComponent(cref.pop_first(), resource, inst, dryRun)

    # check component must exist at this level
    old_component = self.elements.get(first)
    if old_component is None:
      raise ValueError(f"Component '{first}' not found in '{self.name}'")

    if not isinstance(old_component, (Component, ComponentTable)):
      raise ValueError(f"Element '{first}' is not a replaceable component")

    import copy
    snapshot = copy.deepcopy(self) if dryRun else None

    warnings = []
    # create replacement component — FMU or CSV/table
    if isinstance(inst, ResultReader) or (inst is None and resource.endswith(".csv")):
      new_component = ComponentTable(first, resource, inst.connectors if inst else [])
    else:
      new_component = Component(first, resource, inst._fmuType, inst.makeConnectors(), inst._unitDefinitions, inst._enumerationDefinitions)
      new_component.fmu = inst
      new_component.fmuType = inst.fmuType

    # preserve OMEdit geometry
    new_component.elementgeometry = copy.deepcopy(old_component.elementgeometry)
    # preserve values and resources
    new_component.value = copy.deepcopy(old_component.value)
    new_component.parameterMapping = copy.deepcopy(old_component.parameterMapping)
    new_component.parameterResources = copy.deepcopy(old_component.parameterResources)
    new_component.metaDataResources = copy.deepcopy(old_component.metaDataResources)
    new_component.solver = old_component.solver

    # connector lookup
    old_connectors = {str(c.name): c for c in old_component.connectors}
    new_connectors = {str(c.name): c for c in new_component.connectors}

    # validate existing connections
    connections_to_remove = []
    for connection in self.connections:
      connector_name = None
      if str(connection.startElement) == str(first):
        connector_name = str(connection.startConnector)
      elif str(connection.endElement) == str(first):
        connector_name = str(connection.endConnector)
      else:
        continue
      old_connector = old_connectors.get(connector_name)
      new_connector = new_connectors.get(connector_name)
      conn_str = f"{connection.startElement}.{connection.startConnector} ==> {connection.endElement}.{connection.endConnector}"

      # connector removed
      if new_connector is None:
        warnings.append(
          f"deleting connection \"{conn_str}\", as signal \"{connector_name}\" couldn't be resolved "
          f"to any signal in the replaced submodel \"{resource}\"")
        connections_to_remove.append(connection)
        continue

      # causality changed
      if (old_connector is not None
        and old_connector.causality is not None
        and new_connector.causality is not None
        and old_connector.causality != new_connector.causality):
        warnings.append(
          f"deleting connection \"{conn_str}\", as signal \"{connector_name}\" causality changed "
          f"in the replaced submodel \"{resource}\"")
        connections_to_remove.append(connection)
        continue

      # signal type changed
      if (old_connector is not None
        and old_connector.signal_type is not None
        and new_connector.signal_type is not None
        and old_connector.signal_type != new_connector.signal_type):
        warnings.append(
          f"deleting connection \"{conn_str}\", as signal \"{connector_name}\" type changed "
          f"in the replaced submodel \"{resource}\"")
        connections_to_remove.append(connection)

    # remove invalid connections
    for connection in connections_to_remove:
      if connection in self.connections:
        self.connections.remove(connection)

    # cleanup start values (FMU only — ComponentTable has no variables)
    if inst is not None and hasattr(inst, "variables"):
      valid_variables = {str(variable.name) for variable in inst.variables}
      for parameter_name in list(new_component.value.start_values.keys()):
        if str(parameter_name) not in valid_variables:
          del new_component.value.start_values[parameter_name]
          warnings.append(
            f"deleting start value \"{first}.{parameter_name}\" in \"inline\" resources, "
            f"because the identifier couldn't be resolved to any system signal in the replacing model"
          )
    # replace component
    self.elements[first] = new_component

    # rollback for dry run
    if dryRun:
      self.__dict__.clear()
      self.__dict__.update(snapshot.__dict__)
      return warnings

    del old_component
    return warnings

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

  def addMetaDataReference(self, cref: CRef, resource: str, kind: str, type: str):
    ## top level system
    if cref is None:
      self.metaDataResources.append({"source":resource, "kind":kind, "type":type})
      return

    first = cref.first()

    match self.elements.get(first):
      case System():
        self.elements[first].addMetaDataReference(cref.pop_first(), resource, kind, type)
      case Component():
        self.elements[first].addMetaDataReference(resource, kind, type)
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
    if self._deleteConnector(first):
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
      # str()-convert: CRef.split() returns CRef objects, but Connection
      # stores/compares startElement/startConnector as plain strings
      # everywhere else (XML import, _deleteConnection) -- leaving these as
      # CRef silently breaks deleteConnection's equality check later.
      self.addConnection(str(start_element), str(start_connector), str(end_element), str(end_connector))

  def _deleteConnection(self, crefA: CRef, crefB: CRef) -> None:
    """Deletes a connection from the system."""
    startElement, startConnector = self.split_cref(crefA)
    endElement, endConnector = self.split_cref(crefB)

    for connection in self.connections[:]:
      if (connection.startElement == str(startElement) and connection.startConnector == str(startConnector) and connection.endElement == str(endElement) and connection.endConnector == str(endConnector)):
        self.connections.remove(connection)
        return

    for key, element in self.elements.items():
      if isinstance(element, System):
        self.elements[key]._deleteConnection(crefA.pop_first(), crefB.pop_first())

  def isConnectorAlreadyConnected(self, startElement : str, startConnector : str, endElement : str, endConnector : str):
    """Check if a connection is valid in the system."""
    for conn in self.connections:
      if (conn.endElement == str(endElement) and conn.endConnector == str(endConnector)):
        raise ValueError(f"Connector '{conn.endElement}.{conn.endConnector}' is already connected to {conn.startElement}.{conn.startConnector}'")

  def _findConnector(self, element_name, connector_name):
    """Returns (owner_string, causality) or (None, None) if not found."""
    if not element_name:
      connectors = self.connectors  # System level
    else:
      key = CRef(element_name)
      if key not in self.elements:
        known = [str(k) for k in self.elements.keys()]
        raise ValueError(
          f"Component '{element_name}' not found in system '{self.name}'. "
          f"Known elements: {known}. "
          f"If you renamed a component, make sure to update all connection "
          f"references (startElement/endElement) in <ssd:Connection> too."
        )
      connectors = self.elements[key].connectors  # Element level
    for con in connectors:
      if str(con.name) == str(connector_name):
        owner_str = "System" if not element_name else "Element"
        return (owner_str, con.causality)

    raise ValueError(f"Connector '{connector_name}' not found in element '{element_name or self.name}'")

  def addConnection(self, startElement : str, startConnector : str, endElement : str, endConnector : str):
    """Adds a connection to the system."""
    # Resolve source connector owner and causality
    (source_owner, source_kind) = self._findConnector(startElement, startConnector)
    # Resolve destination connector owner and causality
    (dest_owner, dest_kind) = self._findConnector(endElement, endConnector)

    if Connection.is_validConnection(source_owner, source_kind, dest_owner, dest_kind):
      self.isConnectorAlreadyConnected(startElement, startConnector, endElement, endConnector)
      self.connections.append(Connection(startElement, startConnector, endElement, endConnector))
    # flipped connection
    elif Connection.is_validConnection(dest_owner, dest_kind, source_owner, source_kind):
      self.isConnectorAlreadyConnected(endElement, endConnector, startElement, startConnector)
      self.connections.append(Connection(endElement, endConnector, startElement, startConnector))
    else:
      raise ValueError(f"info: Invalid connection from '{startElement}.{startConnector}'->'{endElement}.{endConnector}' as causality are violated with '{source_owner}.{source_kind.name}' -> '{dest_owner}.{dest_kind.name}'")

  def _connectorExists(self, cref: CRef) -> Connector | None:
    """Check if a connector exists in the system."""
    for connector in self.connectors:
      if connector.name == cref:
        return connector
    return None

  def rename(self, cref: CRef, new_name: CRef):
    """Renames a component or subsystem identified by cref to new_name."""
    first = cref.first()
    ## Check if the cref is a top level system or subsystemconnector
    connector = self._connectorExists(first)
    if connector is not None:
      return self.renameConnector(connector, new_name)

    match self.elements.get(first):
      case System() | Component():
        if cref.is_root():
          # cref points directly at the element to rename — do it here.
          self.renameComponent(first, new_name)
        else:
          # Recurse into the subsystem.
          self.elements[first].rename(cref.pop_first(), new_name)
      case _:
        raise ValueError(f"Element '{first}' not found in system '{self.name}'")

  def renameConnector(self, connector: Connector, new_name: CRef):
    """Renames a connector identified by cref to new_name."""
    ## Check if the cref is a top level system connector
    old_str = str(connector.name)
    connector.name = new_name
    ## rename all connection references to this connector in the system and subsystems
    for connection in self.connections:
      if str(connection.startConnector) == old_str:
        connection.startConnector = new_name
      if str(connection.endConnector) == old_str:
        connection.endConnector = new_name
    ## rename start values associated with connector
    for key in list(self.value.start_values.keys()):
      if str(key) == old_str:
        self.value.start_values[new_name] = self.value.start_values.pop(key)

  def renameComponent(self, old_name: CRef, new_name: CRef):
    """Renames a component/subsystem and updates all connection references in this system."""
    if old_name not in self.elements:
      raise ValueError(f"Component '{old_name}' not found in system '{self.name}'")
    if new_name in self.elements:
      raise ValueError(f"Component '{new_name}' already exists in system '{self.name}'")
    # Re-key the elements dict
    element = self.elements.pop(old_name)
    element.name = new_name
    self.elements[new_name] = element
    # Update all connection start/end element references
    old_str = str(old_name)
    new_str = str(new_name)
    for connection in self.connections:
      if str(connection.startElement) == old_str:
        connection.startElement = new_str
      if str(connection.endElement) == old_str:
        connection.endElement = new_str

  def _deleteConnector(self, cref: CRef) -> bool:
    """Delete connector if it exists."""
    for i, connector in enumerate(self.connectors):
      if connector.name == cref:
        self.deleteAllConnection(cref)
        ## delete start values associated with connector
        for key in list(self.value.start_values.keys()):
          if str(key) == str(cref):
            del self.value.start_values[key]
        del self.connectors[i]
        return True
    return False

  def _getComponentResourcePath(self, cref):
    element_name = cref.first()
    element = self.elements.get(element_name, None)

    ## check if element is a top level system connectors
    ## or allow non existing connectors to support parameter mapping throgh SSM inline or ssm file by checking if cref
    connector = self._connectorExists(cref)
    if connector or cref.is_root():
      return (None, None)

    if element is None:
      raise ValueError(f"Element '{element_name}' not found in System '{self.name}'")

    match element:
      case Component():
        return (element.fmuPath, element)
      case ComponentTable():
        return (element.filePath, element)
      case System():
        return element._getComponentResourcePath(cref.pop_first())
      case _:
        raise TypeError(f"Element '{element_name}' is not a Component or System, but {type(element)}")

  def setValue(self, cref: CRef, value, unit = None, description = None):
    first = cref.first()
    ## Check if the cref is a top level system connector
    ## or allow non existing connectors to support parameter mapping throgh SSM inline or ssm file by checking if cref
    connector = self._connectorExists(first)
    if connector or cref.is_root():
      signal_type = connector.signal_type if connector else None
      self.value.setValue(cref, value, signal_type, unit, description)
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
    connector = self._connectorExists(first)
    if connector or cref.is_root():
      return self.value.getValue(cref)

    match self.elements.get(first):
      case System():
        return self.elements[first].getValue(cref.pop_first())
      case Component():
        return self.elements[first].getValue(cref.last())
      case _:
        raise ValueError(f"Element '{first}' in system '{self.name}' is neither a System nor a Component or a Connector")

  def getElement(self, cref: CRef):
    if cref is None:
      return self

    first = cref.first()
    element = self.elements.get(first)

    if isinstance(element, System):
      if cref.is_root():
        return element
      return element.getElement(cref.pop_first())

    if isinstance(element, (Component, ComponentTable)):
      return element

    raise ValueError(f"Element '{first}' in system '{self.name}' is neither a System nor a Component")

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

  def getUnitDefinitions(self, unitDefinitions: list):
    """Get unique unit definitions defined in fmu (by name only)."""
    for key, element in self.elements.items():
      if isinstance(element, System):
        element.getUnitDefinitions(unitDefinitions)
      elif isinstance(element, Component):
        for unit in element.unitDefinitions:
          # check if this name is already present
          if all(u.name != unit.name for u in unitDefinitions):
            unitDefinitions.append(unit)

  def getEnumerationDefinitions(self, enumerationDefinitions: list):
    """get enumeration definitions defined in fmu"""
    for key, element in self.elements.items():
      if isinstance(element, System):
        element.getEnumerationDefinitions(enumerationDefinitions)
      elif isinstance(element, Component):
        for enumeration in element.enumerationDefinitions:
          # check if this name is already present
          if all(u.name != enumeration.name for u in enumerationDefinitions):
            enumerationDefinitions.append(enumeration)

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

  def generateJson(self, resources: dict | None = None, tempdir : str | None = None, simulation_info: dict | None = None) -> str:
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
      #print(f"Processing solver: {solver} with components: {components} {self.solvers}")
      unit = {
          "components": components,
          "connections": solver_connections.get(solver, [])
      }
      # Add solver settings if available
      if solver is not None:
        # Find solver configuration by name
        solver_config = next((s for s in self.solvers if s.get("name") == solver), None)
        if solver_config:
          unit["solver"] = {
            "name": solver,
            "method": solver_config.get("method"),
            "relativeTolerance": solver_config.get("relativeTolerance"),
            "initialStepSize": solver_config.get("initialStepSize"),
            "minimumStepSize": solver_config.get("minimumStepSize"),
            "maximumStepSize": solver_config.get("maximumStepSize"),
            "fixedStepSize": solver_config.get("fixedStepSize")
        }
        else:
          raise ValueError(f"Solver '{solver}' not found in solver list.")
      data["simulation units"].append(unit)

    # Validate: at most one WC-method solver unit is allowed (nested WC under WC is not supported)
    _WC_METHODS = {"oms_ma", "oms_mav", "oms_mav2"}
    wc_units = [u for u in data["simulation units"] if u.get("solver", {}).get("method") in _WC_METHODS]
    if len(wc_units) > 1:
      wc_names = [u["solver"]["name"] for u in wc_units]
      raise ValueError(
        f"Multiple WC solver units found: {wc_names}. "
        f"Nested WC systems are not supported. All CS FMUs without an explicit solver "
        f"share one WC unit automatically."
      )

    # Add top-level simulation metadata
    data["simulation settings"] = {
        "start time": simulation_info.get("startTime"),
        "stop time": simulation_info.get("stopTime"),
        "result file": simulation_info.get("resultFile"),
        "logging interval": simulation_info.get("loggingInterval"),
        "buffer size": simulation_info.get("bufferSize")
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
        ## always set solver for ModelExchange only fmu's if solver not present
        if fmuType == "me" and element.solver is None:
          solver_name = "oms_me_solver"
          if not any(s.get("name") == solver_name for s in self.solvers):
            self.solvers.append({
                  "name": solver_name,
                  "method": "cvode",
              })
          element.solver = solver_name
        ## default CS FMUs with no solver to oms_ma
        elif element.solver is None:
          solver_name = "oms_ma_default"
          if not any(s.get("name") == solver_name for s in self.solvers):
            self.solvers.append({
                  "name": solver_name,
                  "method": "oms_ma"
              })
          element.solver = solver_name
        ## CS FMUs must not use SC solvers (cvode/euler)
        elif fmuType == "cs":
          _SC_METHODS = {"cvode", "euler"}
          assigned_solver = next((s for s in self.solvers if s.get("name") == element.solver), None)
          if assigned_solver and assigned_solver.get("method") in _SC_METHODS:
            raise ValueError(
              f"CS FMU '{element.name}' is assigned solver '{element.solver}' "
              f"with method '{assigned_solver.get('method')}', but CS FMUs can only use "
              f"WC master algorithms (oms_ma, oms_mav, oms_mav2)."
            )

        ## add connectors info for the component in the json, this is needed for propagating connector geomtery to capi
        connector_info = []
        for connector in element.connectors:
          connector_info.append({
              "name": str(connector.name),
              "causality": connector.causality.name if connector.causality else None,
              "type": connector.signal_type.name if connector.signal_type else None,
          })
        solver_groups[element.solver].append({
            "name": [self.name] + ([systemName] if systemName else []) + [str(element.name)],
            "type": fmuType,
            "path": str(Path(tempdir, str(element.fmuPath))) if tempdir is not None else str(element.fmuPath),
            "connectors": connector_info
        })
        componentSolver[str(element.name)] = element.solver
      elif isinstance(element, ComponentTable):
        ## default tables with no solver to oms_ma
        if element.solver is None:
          solver_name = "oms_ma_default"
          if not any(s.get("name") == solver_name for s in self.solvers):
            self.solvers.append({
                  "name": solver_name,
                  "method": "oms_ma"
              })
          element.solver = solver_name
        ## add connectors info for the component in the json, this is needed for propagating connector geomtery to capi
        connector_info = []
        for connector in element.connectors:
          connector_info.append({
              "name": str(connector.name),
              "causality": connector.causality.name if connector.causality else None,
              "type": connector.signal_type.name if connector.signal_type else None,
          })
        solver_groups[element.solver].append({
            "name": [self.name] + ([systemName] if systemName else []) + [str(element.name)],
            "type": "table",
            "path": str(Path(tempdir, str(element.filePath))) if tempdir is not None else str(element.filePath),
            "connectors": connector_info
        })
      elif isinstance(element, System):
        # recurse into subsystems
        self.processElements(element.elements, element.connections, data, solver_groups, componentSolver, solver_connections, resources, tempdir, systemName=str(element.name))

    for connection in connections:
      startElement = str(connection.startElement)
      endElement = str(connection.endElement)
      startSolver = componentSolver.get(startElement, None)
      endSolver = componentSolver.get(endElement, None)

      solver = None
      if startSolver == endSolver and startSolver is not None:
        solver = startSolver
      elif startSolver is None and endSolver is not None:
        solver = endSolver
      elif endSolver is None and startSolver is not None:
        solver = startSolver
      ##TODO group components and connection without solver information, right now they are grouped under NONE category
      connection_info = {
            "start element": [self.name] + ([systemName] if systemName else []) + ([startElement] if startElement else []),
            "start connector": str(connection.startConnector),
            "end element": [self.name] + ([systemName] if systemName else []) + ([endElement] if endElement else []),
            "end connector": str(connection.endConnector)
        }
      ## add linear transformation info if available
      if connection.linearTransformation:
        connection_info["linear transformation"] = {
          "factor": connection.linearTransformation.factor,
          "offset": connection.linearTransformation.offset
        }
      solver_connections[solver].append(connection_info)

  def export(self, root):
    node = ET.SubElement(root, namespace.tag("ssd", "System"), attrib={"name": str(self.name)})
    if self.description:
      node.set("description", self.description)
    if len(self.connectors) > 0 :
      connectors_node = ET.SubElement(node, namespace.tag("ssd", "Connectors"))
      for connector in self.connectors:
        connector.exportToSSD(connectors_node)

    ## export element geometry if available for system, subsystem and components
    if self.elementgeometry:
      self.elementgeometry.exportToSSD(node)

    ## export top level parameter bindings
    self.value.exportToSSD(node, self.parameterMapping)

    ## export parameters binding to ssd file with reference to ssv filec
    if len(self.parameterResources) > 0:
      parameter_bindings_node = ET.SubElement(node, namespace.tag("ssd", "ParameterBindings"))
      for resource in self.parameterResources:
        for key, value in resource.items():
          parameter_binding_node = ET.SubElement(parameter_bindings_node, namespace.tag("ssd", "ParameterBinding"))
          parameter_binding_node.set("source", key)
          if value:
            parameter_mapping_node = ET.SubElement(parameter_binding_node, namespace.tag("ssd", "ParameterMapping"))
            parameter_mapping_node.set("source", value)

    ## export MetaData resources to ssd file
    if len(self.metaDataResources) > 0:
      for resource in self.metaDataResources:
        meta_data_node = ET.SubElement(node, namespace.tag("ssc", "MetaData"))
        meta_data_node.set("kind", resource.get("kind"))
        meta_data_node.set("type", resource.get("type"))
        meta_data_node.set("source", resource.get("source"))

    ## export elements
    if len(self.elements) > 0:
      element_node = ET.SubElement(node, namespace.tag("ssd", "Elements"))
      for key, element in self.elements.items():
        if isinstance(element, System):
          element.export(element_node)
        elif isinstance(element, Component):
          element.exportToSSD(element_node)
        elif isinstance(element, ComponentTable):
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
      utils.exportSimulationInformation(node, self.solvers)

    return node
