from lxml import etree as ET

from OMSimulator import namespace


class Unit:
  def __init__(self, name: str = None, baseUnits: dict = None):
    self.name = name
    self.baseUnits = baseUnits or {}

  def list(self, prefix=""):
    '''Lists the unit and its base units.'''
    print(f"{prefix} Unit: {self.name}")
    base_units_str = ", ".join(f"{key}: {value}" for key, value in self.baseUnits.items())
    print(f"{prefix}{prefix} BaseUnit: {base_units_str}")

  def exportToSSD(self, node):
    '''Exports unit definitions to an XML node.'''
    unit_node = ET.SubElement(node, namespace.tag('ssc', 'Unit'), name=self.name)
    base_unit_node = ET.SubElement(unit_node, namespace.tag('ssc', 'BaseUnit'))
    for key, value in self.baseUnits.items():
      base_unit_node.set(key, value)
