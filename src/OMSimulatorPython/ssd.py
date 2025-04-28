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
from OMSimulator import namespace, utils
from datetime import datetime

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
    self.unitDefinitions = list()
    if model:
      model.add(self)

  @staticmethod
  def importFromFile(filename: Path, resources = None):
    '''Imports an SSD file and parses its contents.'''
    try:
      tree = ET.parse(filename)
      root = tree.getroot()
      utils.validateSSP(root, filename, "SystemStructureDescription.xsd")
      variant_name = root.get("name")
      ssd = SSD(variant_name)
      ssd._filename = Path(filename).resolve()

      system = root.find("ssd:System", namespaces=namespace.ns)
      if system is None:
        raise ValueError(f"Invalid SSD file: Missing <ssd:System> in {filename}")

      ssd.system = System.importFromNode(system, ssd, resources)
      utils.parseDefaultExperiment(root, ssd)
      Unit.importFromNode(root, ssd)
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

  def _validateCref(self, cref: CRef):
    if self.system is None:
      raise ValueError("Variant doesn’t contain a system")

    first = cref.first()
    if str(first) != self.system.name:
        raise ValueError(f"System '{first}' not found in active variant")
    return cref.pop_first()

  def addComponent(self, cref: CRef, resource: str, inst = None | FMU):
    subcref = self._validateCref(cref)
    return self.system.addComponent(subcref, resource, inst)

  def addSSV(self, cref: CRef, resource):
    subcref = self._validateCref(cref)
    self.system.addSSV(subcref, resource)

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

  def setValue(self, cref: CRef, value, unit = None):
    subcref = self._validateCref(cref)
    self.system.setValue(subcref, value, unit)

  def addSystem(self, cref: CRef):
    if self.system is None:
      raise ValueError("Variant doesn't contain a system")

    self.system.addSystem(cref.pop_first(first=self._name))

  def instantiate(self):
    if self.system is None:
      raise ValueError("Variant doesn't contain a system")
    self.system.instantiate()

  def list(self, prefix=""):
    '''Prints the SSD contents.'''
    print(f"{prefix} {type(self)}")
    if self._model and self._model._activeVariantName == self._name:
      print(f'{prefix} Active variant "{self._name}": {suppress_path_to_str(self._filename)}')
    else:
      print(f'{prefix} Inactive variant "{self._name}": {suppress_path_to_str(self._filename)}')

    if self.system:
      self.system.list(prefix=prefix + " |--")

    if self.unitDefinitions:
      print(f"{prefix} UnitDefinitions:")
      for unit in self.unitDefinitions:
        unit.list(prefix=prefix + " |--")

    print(f"{prefix} DefaultExperiment")
    print(f"{prefix} |-- startTime: {self.startTime}")
    print(f"{prefix} |-- stopTime: {self.stopTime}")

  def export(self, filename: str):
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
