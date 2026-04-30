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

from OMSimulator import namespace, utils, CRef
from OMSimulator.connection import LinearTransformation
from collections import defaultdict


class SSM:
  def __init__(self, ssm_path : str | None = None):
    self.filename = None
    self.mappingEntry = defaultdict(list)
    if ssm_path:
      self.filename = Path(ssm_path)
      ##TODO import from ssm
      self.importFromSSM(self.filename)

  def mapParameter(self, source: str, target: str, linearTransformation: LinearTransformation = None):
    ##TODO add support for other GTTransformation such as BooleanMappingTransformation, IntegerMappingTransformation and EnumerationMappingTransformation
    self.mappingEntry[source].append({
        "target": target,
        "linearTransformation": linearTransformation
    })

  def empty(self) -> bool:
    return not self.mappingEntry

  def list(self, prefix = ""):
    if not self.mappingEntry:
      return

    for source, targets in self.mappingEntry.items():
      print(f"{prefix} source: {source}")
      target_list = []
      for entry in targets:
        target = entry["target"]
        linearTransformation = entry["linearTransformation"]
        if linearTransformation:
          target_list.append((target, linearTransformation))
        else:
          target_list.append(target)
      print(f"{prefix} |-- targets: {target_list}")

  def exportToSSD(self, node):
    """Exports the SSM mapping to an SSD node."""
    if not self.mappingEntry:
      return

    parameter_mapping_node_ssd = ET.SubElement(node, namespace.tag("ssd", "ParameterMapping"))
    parameter_mapping_node_ssm = ET.SubElement(parameter_mapping_node_ssd, namespace.tag("ssm", "ParameterMapping"))

    self.exportMappingEntry(parameter_mapping_node_ssm)

  def export(self, filename : str):
    if not filename:
      raise ValueError("Filename cannot be empty")

    if not self.mappingEntry:
      return

    ssm_node = ET.Element(namespace.tag("ssm", "ParameterMapping"),
                                   nsmap={"ssc": "http://ssp-standard.org/SSP1/SystemStructureCommon",
                                          "ssm": "http://ssp-standard.org/SSP1/SystemStructureParameterMapping"},
                                   version = "2.0")

    self.exportMappingEntry(ssm_node)

    ## export to xml
    xml = ET.tostring(ssm_node, encoding='utf-8', xml_declaration=True, pretty_print=True).decode('utf-8')

    ## write to filesystem
    self.filename = Path(filename).resolve()
    with open(self.filename, "w", encoding="utf-8") as file:
      file.write(xml)

  def exportMappingEntry(self, node):
    """Exports the SSM mapping entries to an SSD node."""
    for source, targets in self.mappingEntry.items():
      for entry in targets:
        ssm_mapping_node = ET.SubElement(node, namespace.tag("ssm", "MappingEntry"))
        target = entry["target"]
        ssm_mapping_node.set("source", str(source))
        ssm_mapping_node.set("target", str(target))
        linearTransformation = entry["linearTransformation"]
        if linearTransformation:
          ssm_linearTransformation_node = ET.SubElement(ssm_mapping_node, namespace.tag("ssc", "LinearTransformation"))
          ssm_linearTransformation_node.set("factor", str(linearTransformation.factor))
          ssm_linearTransformation_node.set("offset", str(linearTransformation.offset))

  def exportSSMTemplate(self, node, connectors : list, prefix = None):
    if not connectors:
      return
    for connector in connectors:
      ssm_mapping_node = ET.SubElement(node, namespace.tag("ssm", "MappingEntry"))
      ssm_mapping_node.set("source", "")
      target_name = f"{prefix}.{str(connector.name)}" if prefix else str(connector.name)
      ssm_mapping_node.set("target", target_name)

  def importFromSSM(self, filename):
    mappingentry = utils.parseSSM(filename)
    for source, targets in mappingentry.items():
      for entry in targets:
        target = entry["target"]
        linearTransformation = entry["linearTransformation"]
        self.mapParameter(source, target, linearTransformation)
