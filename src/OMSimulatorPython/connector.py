from typing import Union

from lxml import etree as ET
from OMSimulator.cref import CRef
from OMSimulator.variable import Causality, SignalType

from OMSimulator import namespace


class Connector:
  def __init__(self, name : Union[str, CRef], causality : Causality, signal_type : SignalType):
    self.name = CRef(name)
    self.causality = causality if isinstance(causality, Causality) else Causality[causality]
    self.signal_type = signal_type if isinstance(signal_type, SignalType) else SignalType[signal_type]
    self.unit = None

  def getCref(self):
    return self.name

  def getSignalType(self):
     return self.signal_type

  def getCausality(self):
     return self.causality

  def setUnit(self, unit: str):
    self.unit = unit

  def list(self, prefix=""):
    print(f"{prefix} ({self.name}, {self.causality}, {self.signal_type})")
    pass

  def exportToSSD(self, node):
    connector_node = ET.SubElement(node, namespace.tag("ssd", "Connector"))
    connector_node.set("name", str(self.name))
    connector_node.set("kind", self.causality.name)
    connectors_type = ET.SubElement(connector_node, namespace.tag("ssc", self.signal_type.name))
    if self.unit is not None:
      connectors_type.set("unit", self.unit)
