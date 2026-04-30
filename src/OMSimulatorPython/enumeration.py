# This file is part of OpenModelica.
#
# Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
# c/o Linköpings universitet, Department of Computer and Information Science,
# SE-58183 Linköping, Sweden.
#
# All rights reserved.
#
# THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
# THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
# VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
#
# The OpenModelica software and the OSMC (Open Source Modelica Consortium)
# Public License (OSMC-PL) are obtained from OSMC, either from the above
# address, from the URLs:
# http://www.openmodelica.org or
# https://github.com/OpenModelica/ or
# http://www.ida.liu.se/projects/OpenModelica,
# and in the OpenModelica distribution.
#
# GNU AGPL version 3 is obtained from:
# https://www.gnu.org/licenses/licenses.html#GPL
#
# This program is distributed WITHOUT ANY WARRANTY; without
# even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
# IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
#
# See the full OSMC Public License conditions for more details.

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