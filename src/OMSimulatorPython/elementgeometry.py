from OMSimulator import namespace
from lxml import etree as ET

class ElementGeometry:
  """
  A class to represent the geometry of an element in a simulation model.
  """
  def __init__(
    self,
    x1 : float | None = None,
    y1 : float | None = None,
    x2 : float | None = None,
    y2 : float | None = None,
    rotation: float = 0.0,
    icon_source: str | None = None,
    icon_rotation: float = 0.0,
    icon_flip: bool = False,
    icon_fixed_aspect_ratio: bool = False):

    self._x1 = x1
    self._y1 = y1
    self._x2 = x2
    self._y2 = y2
    self._rotation = rotation
    self._icon_source = icon_source
    self._icon_rotation = icon_rotation
    self._icon_flip = icon_flip
    self._icon_fixed_aspect_ratio = icon_fixed_aspect_ratio


  @staticmethod
  def importFromNode(node):
    """Imports the geometry from an XML node."""
    element_geometry_node = node.find("ssd:ElementGeometry", namespaces=namespace.ns)

    if element_geometry_node is None:
      return None

    x1 = float(element_geometry_node.get("x1", 0))
    y1 = float(element_geometry_node.get("y1", 0))
    x2 = float(element_geometry_node.get("x2", 0))
    y2 = float(element_geometry_node.get("y2", 0))
    rotation = float(element_geometry_node.get("rotation", 0))
    icon_source = element_geometry_node.get("iconSource")
    icon_rotation = float(element_geometry_node.get("iconRotation", 0))
    icon_flip = element_geometry_node.get("iconFlip") == "true"
    icon_fixed_aspect_ratio = element_geometry_node.get("iconFixedAspectRatio") == "true"
    elementgeometry = ElementGeometry(
            x1=x1,
            y1=y1,
            x2=x2,
            y2=y2,
            rotation=rotation,
            icon_source=icon_source,
            icon_rotation=icon_rotation,
            icon_flip=icon_flip,
            icon_fixed_aspect_ratio=icon_fixed_aspect_ratio,
        )
    return elementgeometry

  @property
  def x1(self):
    return self._x1

  @x1.setter
  def x1(self, value):
    self._x1 = value

  @property
  def y1(self):
    return self._y1

  @y1.setter
  def y1(self, value):
    self._y1 = value

  @property
  def x2(self):
    return self._x2

  @x2.setter
  def x2(self, value):
    self._x2 = value

  @property
  def y2(self):
    return self._y2

  @y2.setter
  def y2(self, value):
    self._y2 = value

  @property
  def rotation(self):
    return self._rotation

  @rotation.setter
  def rotation(self, value):
    self._rotation = value

  @property
  def icon_source(self):
    return self._icon_source

  @icon_source.setter
  def icon_source(self, value):
    self._icon_source = value

  @property
  def icon_rotation(self):
    return self._icon_rotation

  @icon_rotation.setter
  def icon_rotation(self, value):
    self._icon_rotation = value

  @property
  def icon_flip(self):
    return self._icon_flip

  @icon_flip.setter
  def icon_flip(self, value):
    self._icon_flip = value

  @property
  def icon_fixed_aspect_ratio(self):
    return self._icon_fixed_aspect_ratio

  @icon_fixed_aspect_ratio.setter
  def icon_fixed_aspect_ratio(self, value):
    self._icon_fixed_aspect_ratio = value

  def list(self, prefix=""):
    """Prints the geometry details with variable names."""
    print(f"{prefix} (x1:{self.x1}, y1:{self.y1}, x2:{self.x2}, y2:{self.y2}, "
                f"rotation:{self.rotation}, icon_source:{self.icon_source}, "
                f"icon_rotation:{self.icon_rotation}, icon_flip:{self.icon_flip}, "
                f"icon_fixed_aspect_ratio:{self.icon_fixed_aspect_ratio})")

  def exportToSSD(self, node):
    """Exports the geometry to an XML node."""
    element_geometry_node = ET.SubElement(node, namespace.tag("ssd", "ElementGeometry"))
    element_geometry_node.set("x1", str(self.x1))
    element_geometry_node.set("y1", str(self.y1))
    element_geometry_node.set("x2", str(self.x2))
    element_geometry_node.set("y2", str(self.y2))

    if self.rotation:
      element_geometry_node.set("rotation", str(self.rotation))
    if self.icon_source:
      element_geometry_node.set("iconSource", self.icon_source)
    if self.icon_rotation:
      element_geometry_node.set("iconRotation", str(self.icon_rotation))
    if self.icon_flip:
      element_geometry_node.set("iconFlip", str(self.icon_flip).lower())
    if self.icon_fixed_aspect_ratio:
      element_geometry_node.set("iconFixedAspectRatio", str(self.icon_fixed_aspect_ratio).lower())