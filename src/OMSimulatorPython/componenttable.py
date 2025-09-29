from pathlib import Path
from lxml import etree as ET
from OMSimulator.cref import CRef
from OMSimulator.connector import Connector
from OMSimulator.variable import Causality, SignalType
from OMSimulator import namespace
from OMSimulator.capi import Capi

class ResultReader:
  def __init__(self, filePath: str):
    self.filePath = Path(filePath)
    self.headers = []
    self.connectors = []
    self.getSignals()

  def getSignals(self):
    """Read only the header row from the CSV file."""
    (signal, status) = Capi.getResultFileSignals(str(self.filePath))
    self.headers = signal.split(",")
    self.makeConnector()

  def makeConnector(self):
    for header in self.headers:
      if header != "time":
        self.connectors.append(Connector(header, Causality.output, SignalType.Real))

class ComponentTable:
  def __init__(self, name: CRef, filePath: Path | str, connectors : list | None = None):
    self.name = name
    self.filePath = filePath
    self.connectors = connectors
    self._solver = None

  @property
  def solver(self):
    return self._solver

  @solver.setter
  def solver(self, cref: str):
    self._solver = cref

  def list(self, prefix=""):
    print(f"{prefix} Table: {self.name}")
    prefix += ' |--'
    print(f"{prefix} path: {self.filePath}")

    if len(self.connectors) > 0:
      print(f"{prefix} Connectors:")
      for connector in self.connectors:
        connector.list(prefix=prefix + " |--")

  def exportToSSD(self, node):
    component_node = ET.SubElement(node, namespace.tag("ssd", "Component"))
    component_node.set("name", str(self.name))
    component_node.set("type", "text/csv")
    component_node.set("source", str(self.filePath))

    if len(self.connectors) > 0:
      connectors_node = ET.SubElement(component_node, namespace.tag("ssd", "Connectors"))
      ## export component connectors
      for connector in self.connectors:
        connector.exportToSSD(connectors_node)
