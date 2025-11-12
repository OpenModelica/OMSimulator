from lxml import etree as ET

from OMSimulator import namespace
from OMSimulator.variable import Causality

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
    connection_node.set("startElement", str(self.startElement))
    connection_node.set("startConnector", str(self.startConnector))
    connection_node.set("endElement", str(self.endElement))
    connection_node.set("endConnector", str(self.endConnector))
    if self.description:
      connection_node.set("description", self.description)
    if self.connectionGeometry:
      self.connectionGeometry.exportToSSD(connection_node)

  @staticmethod
  def importFromNode(node, root):
    connections_node = node.find("ssd:Connections", namespaces=namespace.ns)
    if connections_node is None:
      return
    for connection in connections_node.findall("ssd:Connection", namespaces=namespace.ns):
      startElement = connection.get("startElement", '')
      startConnector = connection.get("startConnector")
      endElement = connection.get("endElement", '')
      endConnector = connection.get("endConnector")
      description = connection.get("description")
      root.addConnection(startElement, startConnector, endElement, endConnector)
      if description:
        root.connections[-1].description = description
      connection_geometry = connection.find("ssd:ConnectionGeometry", namespaces=namespace.ns)
      if connection_geometry is not None:
        pointsX = connection_geometry.get("pointsX")
        pointsY = connection_geometry.get("pointsY")
        # print (f"ConnectionGeometry: pointsX: {pointsX}, pointsY: {pointsY}")
        if pointsX and pointsY:
          connectionGeometry = ConnectionGeometry([float(x) for x in pointsX.split()], [float(y) for y in pointsY.split()])
          root.connections[-1].connectionGeometry = connectionGeometry

  @staticmethod
  def is_validConnection(source_owner: str, source_kind: Causality, dest_owner: str, dest_kind: Causality) -> bool:
    """Return True if the connection is allowed according to SSP 2.0 table."""

    # Allowed causality by owner type
    allowed_source_by_owner = {
      "System":  {Causality.input, Causality.parameter},
      "Element": {Causality.output, Causality.calculatedParameter},
    }

    allowed_dest_by_owner = {
      "System":  {Causality.output, Causality.calculatedParameter},
      "Element": {Causality.input, Causality.parameter},
    }

    result = (source_kind in allowed_source_by_owner.get(source_owner, set()) and dest_kind in allowed_dest_by_owner.get(dest_owner, set()))

    return result
