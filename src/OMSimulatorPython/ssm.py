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

  def mapParameter(self, source: str, target: str):
    self.mappingEntry[source].append(target)

  def empty(self) -> bool:
    return not self.mappingEntry

  def list(self, prefix = ""):
    if not self.mappingEntry:
      return

    for source, targets in self.mappingEntry.items():
      print(f"{prefix} source: {source}")
      print(f"{prefix} |-- targets: {targets}")

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
      for target in targets:
        ssm_mapping_node = ET.SubElement(node, namespace.tag("ssm", "MappingEntry"))
        ssm_mapping_node.set("source", str(source))
        ssm_mapping_node.set("target", str(target))

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
      for target in targets:
        self.mapParameter(source, target)
