from lxml import etree as ET
from OMSimulator.values import Values

from OMSimulator import namespace, utils
from pathlib import Path

class SSV:
  def __init__(self, ssv_path : str | None = None):
    self.filename = None
    self.value = Values()
    if ssv_path:
      self.filename = Path(ssv_path)
      self.importFromSSV(self.filename)

  def setValue(self, cref:str, value, unit = None):
    self.value.setValue(cref, value, unit)

  def exportToSSD(self, node):
    parameter_bindings_node = ET.SubElement(node, namespace.tag("ssd", "ParameterBindings"))
    parameter_binding_node = ET.SubElement(parameter_bindings_node, namespace.tag("ssd", "ParameterBinding"))
    parameter_binding_node.set("source", "resources/"+ self.filename.name)

  def list(self, prefix = ""):
    self.value.list(prefix)

  def export(self, filename = str):
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
    for key, (value, unit) in parameterValues.items():
      self.setValue(key, value, unit)
