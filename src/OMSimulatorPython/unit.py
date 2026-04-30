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
