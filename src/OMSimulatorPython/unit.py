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
