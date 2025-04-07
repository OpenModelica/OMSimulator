from lxml import etree as ET

from OMSimulator import namespace

class ConnectionGeometry:
  def __init__(self, pointsX: list , pointsY: list):
    self.pointsX = pointsX
    self.pointsY = pointsY

  def list(self, prefix=""):
    print(f"{prefix} ConnectionGeometry: (pointsX: {self.pointsX}, pointsY: {self.pointsY})")

  def exportToSSD(self, node):
    connection_geometry_node = ET.SubElement(node, namespace.tag("ssd", "ConnectionGeometry"))
    connection_geometry_node.set("pointsX", " ".join(map(str, self.pointsX)))
    connection_geometry_node.set("pointsY", " ".join(map(str, self.pointsY)))


class Connection:
  def __init__(self, startElement : str, startConnector : str, endElement : str, endConnector : str):
    self.startElement = startElement
    self.startConnector = startConnector
    self.endElement = endElement
    self.endConnector = endConnector
    self.description = None
    self.connectionGeometry = None

  def list(self, prefix=""):
    print(f"{prefix} {self.startElement}.{self.startConnector} -> {self.endElement}.{self.endConnector}")
    if self.connectionGeometry:
      self.connectionGeometry.list(prefix + " |--")

  def exportToSSD(self, node):
    connection_node = ET.SubElement(node, namespace.tag("ssd", "Connection"))
    connection_node.set("startElement", self.startElement)
    connection_node.set("startConnector", self.startConnector)
    connection_node.set("endElement", self.endElement)
    connection_node.set("endConnector", self.endConnector)
    if self.description:
      connection_node.set("description", self.description)
    if self.connectionGeometry:
      self.connectionGeometry.exportToSSD(connection_node)
