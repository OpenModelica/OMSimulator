import logging
import os

from lxml import etree as ET

from OMSimulator import namespace, utils
from OMSimulator.system import System
from OMSimulator.cref import CRef
from OMSimulator.settings import Settings
from OMSimulator.fmu import FMU

logger = logging.getLogger(__name__)


class SSD:
  def __init__(self, name: str, model=None):
    '''Initialize an SSD object.'''
    from OMSimulator.ssp import SSP

    self._name = name
    self._model: SSP = None
    self._filename = None
    self.system = System(name, model=model)
    self.startTime = 0.0
    self.stopTime = 1.0

    if model:
      model.add(self)

  @staticmethod
  def importFromFile(filename: str):
    '''Imports an SSD file and parses its contents.'''
    try:
      temp_dir = os.path.dirname(filename)
      tree = ET.parse(filename)
      root = tree.getroot()
      variant_name = root.get("name")
      ssd = SSD(variant_name)
      ssd._filename = filename

      system = root.find("ssd:System", namespaces=namespace.ns)
      if system is None:
        raise ValueError(f"Invalid SSD file: Missing <ssd:System> in {filename}")

      ssd.system = System.importFromNode(system, ssd)

      utils.parseDefaultExperiment(root, ssd)

      logger.debug(f"SSD '{variant_name}' successfully imported from {filename}")
      return ssd

    except ET.ParseError as e:
      logger.error(f"Error parsing SSD file '{filename}': {e}")
      return None

  @property
  def name(self):
    return self._name

  @property
  def dirty(self):
    '''Returns True if the SSD has been modified since the last export.'''
    return True

  def addComponent(self, cref: CRef, resource: str, inst = None | FMU):
    if self.system is None:
      raise ValueError("Variant doesn#t contain a system")
    self.system.addComponent(cref, resource, inst)

  def list(self):
    '''Prints the SSD contents.'''
    print(f"|-- {type(self)}")
    if self._model and self._model._activeVariantName == self._name:
      print(f'|-- Active variant "{self._name}": {self._filename if not Settings.suppressPath else "<hidden>"}')
    else:
      print(f'|-- Inactive variant "{self._name}": {self._filename if not Settings.suppressPath else "<hidden>"}')

    if self.system:
      self.system.list()

    print("|--   DefaultExperiment")
    print(f"|--     startTime: {self.startTime}")
    print(f"|--     stopTime: {self.stopTime}")

  def export(self, filename: str):
    '''Exports the SSD as an XML file.'''
    root = ET.Element(
      namespace.tag("ssd", "SystemStructureDescription"),
      nsmap=namespace.ns,
      name=self._name,
      version="2.0",
    )

    self.system.export(root)
    self._exportDefaultExperiment(root)

    with open(filename, "w", encoding="utf-8") as file:
      xml_content = ET.tostring(
        root, encoding="utf-8", xml_declaration=True, pretty_print=True
      ).decode("utf-8")
      file.write(xml_content)

    logger.info(f"SSD '{self._name}' exported to {filename}")

  def _exportDefaultExperiment(self, node):
    '''Exports default experiment settings.'''
    default_experiment = ET.SubElement(node, namespace.tag("ssd", "DefaultExperiment"))
    default_experiment.set("startTime", str(self.startTime))
    default_experiment.set("stopTime", str(self.stopTime))
