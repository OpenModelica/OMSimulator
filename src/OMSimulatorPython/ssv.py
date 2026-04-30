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

from pathlib import Path

from lxml import etree as ET
from OMSimulator.values import Values
from OMSimulator.variable import SignalType
from OMSimulator import namespace, utils


class SSV:
  def __init__(self, ssv_path : str | None = None):
    self.filename = None
    self.value = Values()
    if ssv_path:
      self.filename = Path(ssv_path)
      self.importFromSSV(self.filename)

  def setValue(self, cref:str, value, type : SignalType | None = None, unit = None, description = None):
    self.value.setValue(cref, value, type, unit, description)

  def list(self, prefix = ""):
    self.value.list(prefix)

  def export(self, filename : str):
    if not filename:
      raise ValueError("Filename cannot be empty")

    ssv_node = ET.Element(namespace.tag("ssv", "ParameterSet"),
                                   nsmap={"ssc": "http://ssp-standard.org/SSP1/SystemStructureCommon",
                                          "ssv": "http://ssp-standard.org/SSP1/SystemStructureParameterValues"},
                                   version = "2.0",
                                   name = "parameters")

    self.value.exportToSSV(ssv_node)
    xml = ET.tostring(ssv_node, encoding='utf-8', xml_declaration=True, pretty_print=True).decode('utf-8')

    ## write to filesystem
    self.filename = Path(filename).resolve()
    with open(self.filename, "w", encoding="utf-8") as file:
      file.write(xml)

  def importFromSSV(self, filename):
    parameterValues = utils.parseSSV(filename)
    for key, (value, type, unit, description) in parameterValues.items():
      self.setValue(key, value, type, unit, description)
