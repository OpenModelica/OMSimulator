from lxml import etree as ET

from OMSimulator import namespace


class Enumeration:
  def __init__(self, name: str = None, enumItems: dict = None, description: str = None):
    self.name = name
    self.enumItems = enumItems or {}
    self.description = description

  def list(self, prefix=""):
    '''Lists the unit and its base units.'''
    print(f"{prefix} EnumName: {self.name}")
    enum_items_str = ", ".join(f"{key}: {value}" for key, value in self.enumItems.items())
    print(f"{prefix} |-- EnumItems: {enum_items_str}")

  def exportToSSD(self, node):
    '''Exports unit definitions to an XML node.'''
    enumeration_node = ET.SubElement(node, namespace.tag('ssc', 'Enumeration'), name=self.name)
    if self.description:
      enumeration_node.set('description', self.description)
    for key, value in self.enumItems.items():
      enumeration_item_node = ET.SubElement(enumeration_node, namespace.tag('ssc', 'Item'))
      enumeration_item_node.set("name", key)
      enumeration_item_node.set("value", str(value))


  @staticmethod
  def importFromNode(node, root, tagname="ssd:Enumerations"):
    """Imports enumeration definitions from an XML node."""
    enums_element = node.find(tagname, namespaces=namespace.ns)
    if enums_element is None:
        return

    for enum in enums_element.findall("ssc:Enumeration", namespaces=namespace.ns):
      name = enum.get("name")
      description = enum.get("description")
      # Collect all items under this enumeration
      items = {}
      for item in enum.findall("ssc:Item", namespaces=namespace.ns):
        key = item.get("name")
        value = item.get("value")
        items[key] = value
      # Create an Enumeration object
      enum_obj = Enumeration(name, items, description)
      root.enumerationDefinitions.append(enum_obj)