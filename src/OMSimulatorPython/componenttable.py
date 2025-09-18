from pathlib import Path
import csv
from OMSimulator.connector import Connector
from OMSimulator.variable import Causality, SignalType

class ComponentTable:
  def __init__(self, filePath : str ):
    self.filePath = Path(filePath)
    self._name =  None
    self.headers = []
    self.connectors = []
    self.parse_csv()
    self.makeConnector()

  @property
  def name(self):
    return self._name

  @name.setter
  def name(self, cref: str):
    self._name = cref

  def parse_csv(self):
    """Read only the header row from the CSV file."""
    with open(self.filePath, newline='', encoding="utf-8") as csvfile:
      reader = csv.reader(csvfile)
      self.headers = next(reader)  # first row only

  def makeConnector(self):
    for header in self.headers:
      if header != "time":
        self.connectors.append(Connector(header, Causality.output, SignalType.Real))


  def list(self, prefix=""):
    print(f"{prefix} Table: {self.name}")
    prefix += ' |--'
    print(f"{prefix} path: {self.filePath}")

    if len(self.connectors) > 0:
      print(f"{prefix} Connectors:")
      for connector in self.connectors:
        connector.list(prefix=prefix + " |--")
