from typing import Union

from lxml import etree as ET
from OMSimulator.cref import CRef
from OMSimulator.variable import Causality, SignalType

from OMSimulator import namespace

class ConnectorGeometry:
  def __init__(self, x: float, y: float):
    self._x = x
    self._y = y

  @property
  def x(self):
    return self._x

  @x.setter
  def x(self, value: float):
    self._x = value

  @property
  def y(self):
    return self._y

  @y.setter
  def y(self, value: float):
    self._y = value

  def list(self, prefix=""):
    print(f"{prefix} |-- ConnectorGeometry: (x:{self.x}, y:{self.y})")

  def exportToSSD(self, node):
    geometry_node = ET.SubElement(node, namespace.tag("ssd", "ConnectorGeometry"))
    geometry_node.set("x", str(self.x))
    geometry_node.set("y", str(self.y))

class Connector:
  def __init__(self, name : Union[str, CRef], causality : Causality, signal_type : SignalType):
    self.name = CRef(name)
    self.causality = causality if isinstance(causality, Causality) else Causality[causality]
    self.signal_type = signal_type if isinstance(signal_type, SignalType) else SignalType[signal_type]
    self.unit = None
    self.connectorGeometry = None
    self.description = None
    self.enumName = None

  def getCref(self):
    return self.name

  def getSignalType(self):
     return self.signal_type

  def getCausality(self):
     return self.causality

  def setUnit(self, unit: str):
    self.unit = unit

  def setEnumerationName(self, name: str):
    self.enumName = name

  def list(self, prefix=""):
    print(f"{prefix} ({self.name}, {self.causality}, {self.signal_type}, {self.unit}, '{self.description}')")
    if self.connectorGeometry:
      self.connectorGeometry.list(prefix)

  def exportToSSD(self, node):
    connector_node = ET.SubElement(node, namespace.tag("ssd", "Connector"))
    connector_node.set("name", str(self.name))
    connector_node.set("kind", self.causality.name)
    if self.description:
      connector_node.set("description", self.description)
    connectors_type = ET.SubElement(connector_node, namespace.tag("ssc", self.signal_type.name))
    if self.unit is not None:
      connectors_type.set("unit", self.unit)
    if self.enumName is not None:
      connectors_type.set("name", self.enumName)
    if self.connectorGeometry is not None:
      self.connectorGeometry.exportToSSD(connector_node)

  @staticmethod
  def importFromNode(node):
    """Extract and print system connectors"""
    connectors = []
    connectors_node = node.find("ssd:Connectors", namespaces=namespace.ns)

    # No connectors found
    if connectors_node is None:
      return connectors

    for connector in connectors_node.findall("ssd:Connector", namespaces=namespace.ns):
      name = connector.get("name")
      kind = connector.get("kind")
      description = connector.get("description")
      # Convert kind string to enum type
      kind = Causality[kind]

      # Find the connector type (Real, Integer, Boolean)
      con = None
      for connectortype in ["ssc:Real", "ssc:Integer", "ssc:Boolean", "ssc:Enumeration"]:  # Expected connector types
        type_element = connector.find(connectortype, namespaces=namespace.ns)
        if type_element is not None:
          signal_type = connectortype.split(":")[-1]  # Extracts 'Real', 'Integer', or 'Boolean'
          con = Connector(name, kind, SignalType[signal_type])
          unit = type_element.get("unit")
          enumName = type_element.get("name")
          if description:
            con.description = description
          # Set unit if it exists
          if unit:
            con.setUnit(unit)
          if enumName:
            con.setEnumerationName(enumName)
          # print(f"Connector: {name}, Kind: {kind}, SignalType: {signal_type}, Unit: {unit}, Enum: {enumName}")
          break  # Stop after the first valid type is found

      # Check if connector has geometry information
      connector_geometry = connector.find("ssd:ConnectorGeometry", namespaces=namespace.ns)
      if connector_geometry is not None:
        x = connector_geometry.get("x")
        y = connector_geometry.get("y")
        if x and y:
          connectorGeometry = ConnectorGeometry(float(x), float(y))
          connectorGeometry.x = float(x)  # Set x coordinate
          connectorGeometry.y = float(y)  # Set y coordinate
          con.connectorGeometry = connectorGeometry

      if con:
        connectors.append(con)

    return connectors