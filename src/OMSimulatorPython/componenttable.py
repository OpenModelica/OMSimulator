from pathlib import Path
from lxml import etree as ET
import csv
from OMSimulator.connector import Connector
from OMSimulator.variable import Causality, SignalType
from OMSimulator import namespace

class ComponentTable:
  def __init__(self, filePath : str ):
    self.filePath = Path(filePath)
    self._name =  None
    self._resourcePath = None
    self.headers = []
    self.connectors = []
    self.solver = None
    self.parse_csv()
    self.makeConnector()

  @property
  def name(self):
    return self._name

  @name.setter
  def name(self, cref: str):
    self._name = cref

  @property
  def resourcePath(self):
    return self._resourcePath

  @resourcePath.setter
  def resourcePath(self, cref: str):
    self._resourcePath = cref

  def parse_csv(self):
    """Read only the header row from the CSV file."""
    with open(self.filePath, newline='', encoding="utf-8") as csvfile:
      reader = csv.reader(csvfile)
      self.headers = next(reader)  # first row only

  def makeConnector(self):
    for header in self.headers:
      if header != "time":
        self.connectors.append(Connector(header, Causality.output, SignalType.Real))

  def setSolver(self, name: str):
    self.solver = name

  def list(self, prefix=""):
    print(f"{prefix} Table: {self.name}")
    prefix += ' |--'
    print(f"{prefix} path: {self.resourcePath}")

    if len(self.connectors) > 0:
      print(f"{prefix} Connectors:")
      for connector in self.connectors:
        connector.list(prefix=prefix + " |--")

  def exportToSSD(self, node):
    component_node = ET.SubElement(node, namespace.tag("ssd", "Component"))
    component_node.set("name", str(self.name))
    component_node.set("type", "text/csv")
    component_node.set("source", str(self.resourcePath))

    if len(self.connectors) > 0:
      connectors_node = ET.SubElement(component_node, namespace.tag("ssd", "Connectors"))
      ## export component connectors
      for connector in self.connectors:
        connector.exportToSSD(connectors_node)
