from lxml import etree as ET

from OMSimulator import namespace


class UnitDefinitions:
  def __init__(self, name=None, baseUnits=None):
    self.name = name
    self.baseUnits = baseUnits

  def exportToSSD(self, unitsToExport, node):
    for key, baseunits in unitsToExport.items():
      unit_node = ET.SubElement(node, namespace.tag("ssc", "Unit"))
      unit_node.set("name", key)
      base_unitnode = ET.SubElement(unit_node, namespace.tag("ssc", "BaseUnit"))
      for key, value in baseunits.items():
        base_unitnode.set(key, value)
