from pathlib import Path

from lxml import etree as ET

from OMSimulator import namespace, utils, CRef
from collections import defaultdict


class SSM:
  def __init__(self, ssm_path : str | None = None):
    self.filename = None
    self.mappingEntry = defaultdict(list)
    if ssm_path:
      self.filename = Path(ssm_path)
      ##TODO import from ssm
      self.importFromSSM(self.filename)

  def map(self, source: str, target: str):
    self.mappingEntry[source].append(target)

  def empty(self) -> bool:
    return not self.mappingEntry

  def list(self, prefix = ""):
    if not self.mappingEntry:
      return

    for source, targets in self.mappingEntry.items():
      print(f"{prefix} source: {source}")
      print(f"{prefix} |-- targets: {targets}")

  def export(self, filename : str):
    if not filename:
      raise ValueError("Filename cannot be empty")

    ssm_node = ET.Element(namespace.tag("ssm", "ParameterMapping"),
                                   nsmap={"ssc": "http://ssp-standard.org/SSP1/SystemStructureCommon",
                                          "ssm": "http://ssp-standard.org/SSP1/SystemStructureParameterMapping"},
                                   version = "2.0")

    for source, targets in self.mappingEntry.items():
      for target in targets:
        mapping_node = ET.SubElement(ssm_node, namespace.tag("ssm", "MappingEntry"))
        mapping_node.set("source", str(source))
        mapping_node.set("target", str(target))

    xml = ET.tostring(ssm_node, encoding='utf-8', xml_declaration=True, pretty_print=True).decode('utf-8')

    ## write to filesystem
    self.filename = Path(filename).resolve()
    with open(self.filename, "w", encoding="utf-8") as file:
      file.write(xml)

  def importFromSSM(self, filename):
    mappingentry = utils.parseSSM(filename)
    for source, targets in mappingentry.items():
      for target in targets:
        self.map(source, target)
