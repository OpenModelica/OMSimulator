from lxml import etree as ET

from OMSimulator import namespace


class Unit:
  def __init__(self, name: str = None, baseUnits: dict = None):
    self.name = name
    self.baseUnits = baseUnits or {}

  def exportToSSD(self, node):
    '''Exports unit definitions to an XML node.'''
    unit_node = ET.SubElement(node, namespace.tag('ssc', 'Unit'), name=self.name)
    base_unit_node = ET.SubElement(unit_node, namespace.tag('ssc', 'BaseUnit'))
    for key, value in self.baseUnits.items():
      base_unit_node.set(key, value)
