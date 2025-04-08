from lxml import etree as ET

from OMSimulator import namespace


class Unit:
  def __init__(self, name: str = None, baseUnits: dict = None, description: str = None):
    self.name = name
    self.baseUnits = baseUnits or {}
    self.description = description

  def list(self, prefix=""):
    '''Lists the unit and its base units.'''
    print(f"{prefix} Unit: {self.name}")
    base_units_str = ", ".join(f"{key}: {value}" for key, value in self.baseUnits.items())
    print(f"{prefix} |-- BaseUnit: {base_units_str}")

  def exportToSSD(self, node):
    '''Exports unit definitions to an XML node.'''
    unit_node = ET.SubElement(node, namespace.tag('ssc', 'Unit'), name=self.name)
    if self.description:
      unit_node.set('description', self.description)
    base_unit_node = ET.SubElement(unit_node, namespace.tag('ssc', 'BaseUnit'))
    for key, value in self.baseUnits.items():
      base_unit_node.set(key, value)

  @staticmethod
  def importFromNode(node, root, tagname = "ssd:Units"):
    units_element = node.find(tagname, namespaces=namespace.ns)
    if units_element is None:
      return
    for unit in units_element.findall("ssc:Unit", namespaces=namespace.ns):
      name = unit.get("name")
      description = unit.get("description")
      base_unit = unit.find("ssc:BaseUnit", namespaces=namespace.ns)
      if base_unit is not None:
        attributes = {key: base_unit.get(key) for key in base_unit.keys()}
        # Create a Unit object and add it to the root or system
        unit_obj = Unit(name, attributes, description)
        root.unitDefinitions.append(unit_obj)
        # print(f"Unit: {name}, Attributes: {attributes}")
