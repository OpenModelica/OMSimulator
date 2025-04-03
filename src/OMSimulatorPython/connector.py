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

  def getCref(self):
    return self.name

  def getSignalType(self):
     return self.signal_type

  def getCausality(self):
     return self.causality

  def setUnit(self, unit: str):
    self.unit = unit

  def list(self, prefix=""):
    print(f"{prefix} ({self.name}, {self.causality}, {self.signal_type}, {self.unit})")
    if self.connectorGeometry:
      self.connectorGeometry.list(prefix)

  def exportToSSD(self, node):
    connector_node = ET.SubElement(node, namespace.tag("ssd", "Connector"))
    connector_node.set("name", str(self.name))
    connector_node.set("kind", self.causality.name)
    connectors_type = ET.SubElement(connector_node, namespace.tag("ssc", self.signal_type.name))
    if self.unit is not None:
      connectors_type.set("unit", self.unit)
    if self.connectorGeometry is not None:
      self.connectorGeometry.exportToSSD(connector_node)
