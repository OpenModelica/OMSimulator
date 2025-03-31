from lxml import etree as ET

from OMSimulator import namespace


class Connection:
  def __init__(self, startElement : str, startConnector : str, endElement : str, endConnector : str):
    self.startElement = startElement
    self.startConnector = startConnector
    self.endElement = endElement
    self.endConnector = endConnector

  def list(self, prefix=""):
    print(f"{prefix} {self.startElement}.{self.startConnector} -> {self.endElement}.{self.endConnector}")

  def exportToSSD(self, node):
    connection_node = ET.SubElement(node, namespace.tag("ssd", "Connection"))
    connection_node.set("startElement", self.startElement)
    connection_node.set("startConnector", self.startConnector)
    connection_node.set("endElement", self.endElement)
    connection_node.set("endConnector", self.endConnector)
