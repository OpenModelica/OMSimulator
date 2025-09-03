import logging
from pathlib import Path

from lxml import etree as ET
from OMSimulator import capi
from OMSimulator.cref import CRef
from OMSimulator.fmu import FMU
from OMSimulator.settings import suppress_path_to_str
from OMSimulator.system import System
from OMSimulator.ssv import SSV
from OMSimulator.unit import Unit
from OMSimulator.instantiated_model import InstantiatedModel
from OMSimulator import namespace, utils
from datetime import datetime

logger = logging.getLogger(__name__)


class SSD:
  def __init__(self, name: str):
    '''Initialize an SSD object.'''
    from OMSimulator.ssp import SSP

    self._name = name
    self._filename = None
    ## TODO change the System name to "root" or "default" or "main" or "top" or something
    self.system = System(name)
    self.startTime = 0.0
    self.stopTime = 1.0
    self.unitDefinitions = list()

  @staticmethod
  def importFromFile(filename: Path, resources = None):
    '''Imports an SSD file and parses its contents.'''
    try:
      # Determine input type
      tree = ET.parse(filename)
      root = tree.getroot()
      filename = Path(filename).resolve()
      return SSD.importFromNode(root, filename, resources)
    except ET.ParseError as e:
      logger.error(f"Error parsing SSD file '{filename}': {e}")
      return None

  @staticmethod
  def importFromString(xml_code: str, resources = None):
    '''Imports an SSD from a string.'''
    try:
      root = ET.fromstring(xml_code.encode("utf-8"))
      return SSD.importFromNode(root, None, resources)
    except ET.ParseError as e:
      logger.error(f"Error parsing SSD string: {e}")
      return None

  @staticmethod
  def importFromNode(root, filename, resources = None):
    '''Imports an SSD from an XML node.'''
    utils.validateSSP(root, filename, "SystemStructureDescription.xsd")
    variant_name = root.get("name")
    ssd = SSD(variant_name)
    ssd._filename = filename
    system = root.find("ssd:System", namespaces=namespace.ns)
    if system is None:
      raise ValueError(f"Invalid SSD file: Missing <ssd:System> in {filename}")
    ssd.system = System.importFromNode(system, ssd, resources)
    utils.parseDefaultExperiment(root, ssd)
    Unit.importFromNode(root, ssd)
    logger.debug(f"SSD '{variant_name}' successfully imported from {filename}")
    return ssd

  @property
  def name(self):
    return self._name

  @name.setter
  def name(self, name: str):
    self._name = name

  @property
  def dirty(self):
    '''Returns True if the SSD has been modified since the last export.'''
    return True

  def _validateCref(self, cref: CRef):
    if self.system is None:
      raise ValueError("Variant doesn’t contain a system")

    first = cref.first()
    if str(first) != self.system.name:
        raise ValueError(f"System '{first}' not found in active variant")
    return cref.pop_first()

  def duplicate(self, variant_name: str | None = None):
    '''Duplicates the SSD and returns a new SSD object.'''
    xml_code = self.export()
    ssd = SSD.importFromString(xml_code)
    if variant_name:
      ssd.name = variant_name
    return ssd

  def addComponent(self, cref: CRef, resource: str, inst : FMU | None = None):
    subcref = self._validateCref(cref)
    return self.system.addComponent(subcref, resource, inst)

  def delete(self, cref: CRef):
    subcref = self._validateCref(cref)
    if subcref is None:
      del self.system # Delete the entire system
      self.system = None
      return
    return self.system.delete(subcref)

  def addSSVReference(self, cref: CRef, resource1: str, resource2: str | None = None):
    subcref = self._validateCref(cref)
    self.system.addSSVReference(subcref, resource1, resource2)

  def exportSSVTemplate(self, cref: CRef, node):
    subcref = self._validateCref(cref)

    if subcref is None:
      return self.system.exportSSVTemplateHelper(node)

    self.system.exportSSVTemplate(subcref, node)

  def exportSSMTemplate(self, cref: CRef, node):
    subcref = self._validateCref(cref)

    if subcref is None:
      return self.system.exportSSMTemplateHelper(node)

    self.system.exportSSMTemplate(subcref, node)

  def swapSSVReference(self, cref: CRef, resource1, resource2):
    subcref = self._validateCref(cref)
    self.system.swapSSVReference(subcref, resource1, resource2)

  def listSSVReference(self, cref: CRef):
    subcref = self._validateCref(cref)
    return self.system.listSSVReference(subcref)

  def deleteResource(self, resource: str):
    '''Removes a resource from the SSP.'''
    self.system.deleteResource(resource)

  def removeSSVReference(self, cref: CRef, resource):
    subcref = self._validateCref(cref)
    self.system.removeSSVReference(subcref, resource)

  def addConnection(self, cref1: CRef, cref2: CRef):
    subcref1 = self._validateCref(cref1)
    subcref2 = self._validateCref(cref2)
    #logger.debug(f"Adding connection from {subcref1} to {subcref2}")
    self.system._addConnection(subcref1, subcref2)

  def newSolver(self, options: dict):
    self.system.solvers.append(options)

  def setSolver(self, cref: CRef, name: str):
    subcref = self._validateCref(cref)
    self.system.setSolver(subcref, name)

  def _getComponentResourcePath(self, cref: CRef):
    subcref = self._validateCref(cref)
    return self.system._getComponentResourcePath(subcref)

  def setValue(self, cref: CRef, value, unit = None, description = None):
    subcref = self._validateCref(cref)
    self.system.setValue(subcref, value, unit, description)

  def getValue(self, cref: CRef):
    subcref = self._validateCref(cref)
    self.system.getValue(subcref)

  def mapParameter(self, cref: CRef, source: str, target: str):
    '''Maps a parameter from source to target in the system.'''
    subcref = self._validateCref(cref)
    self.system.mapParameter(subcref, source, target)

  def addSystem(self, cref: CRef):
    if self.system is None:
      raise ValueError("Variant doesn't contain a system")

    self.system.addSystem(cref.pop_first(first=self._name))

  def instantiate(self, resources: dict | None = None, tempdir: str | None = None ) -> InstantiatedModel:
    if self.system is None:
      raise ValueError("Variant doesn't contain a system")
    json_desc = self.system.generateJson(resources, tempdir)
    return InstantiatedModel(json_desc, self.system, resources)

  def list(self, prefix=""):
    '''Prints the SSD contents.'''
    print(f"{prefix} {type(self)}")
    print(f'{prefix} Variant "{self._name}": {suppress_path_to_str(self._filename)}')

    if self.system:
      self.system.list(prefix=prefix + " |--")

    if self.unitDefinitions:
      print(f"{prefix} UnitDefinitions:")
      for unit in self.unitDefinitions:
        unit.list(prefix=prefix + " |--")

    print(f"{prefix} DefaultExperiment")
    print(f"{prefix} |-- startTime: {self.startTime}")
    print(f"{prefix} |-- stopTime: {self.stopTime}")

  def export(self, filename: str | None = None):
    '''Exports the SSD as an XML file.'''
    root = ET.Element(
      namespace.tag("ssd", "SystemStructureDescription"),
      nsmap=namespace.ns,
      name=self._name,
      version="2.0",
      generationTool= capi.capi().getVersion(),
      generationDateAndTime=datetime.now().isoformat()
    )

    self.system.export(root)

    self._exportUnitDefinitions(root)
    self._exportDefaultExperiment(root)

    xml_content = ET.tostring(root, encoding="utf-8", xml_declaration=True, pretty_print=True).decode("utf-8")

    if filename is None:
      return xml_content
    else:
      with open(filename, "w", encoding="utf-8") as file:
        xml_content = ET.tostring(
          root, encoding="utf-8", xml_declaration=True, pretty_print=True
        ).decode("utf-8")
        file.write(xml_content)

      logger.info(f"SSD '{self._name}' exported to {suppress_path_to_str(filename)}")

  def _exportUnitDefinitions(self, node):
    '''Exports unit definitions to the given XML node.'''
    if self.unitDefinitions:
      unit_definitions_node = ET.SubElement(node, namespace.tag("ssd", "Units"))
      for unit in self.unitDefinitions:
        unit.exportToSSD(unit_definitions_node)

  def _exportDefaultExperiment(self, node):
    '''Exports default experiment settings.'''
    default_experiment = ET.SubElement(node, namespace.tag("ssd", "DefaultExperiment"))
    default_experiment.set("startTime", str(self.startTime))
    default_experiment.set("stopTime", str(self.stopTime))
