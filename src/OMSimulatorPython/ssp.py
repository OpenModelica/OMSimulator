import logging
import os
import shutil
import tempfile
import zipfile
from collections.abc import Iterable
from pathlib import Path

from OMSimulator.fmu import FMU
from OMSimulator.settings import suppress_path_to_str
from OMSimulator.ssv import SSV

from OMSimulator import SSD, CRef

logger = logging.getLogger(__name__)

class SSP:
  def __init__(self, path: str | None = None, temp_dir: str | None = None):
    self._activeVariantName = None
    self.variants = {}
    self.resources = {}

    if temp_dir:
      os.makedirs(temp_dir, exist_ok=True)

    self.temp_dir = Path(tempfile.mkdtemp(dir=temp_dir)).resolve()
    logger.info(f"Temporary directory created: {suppress_path_to_str(self.temp_dir)}")

    if path:
      self._extract_ssp(path)

    # Ensure at least one variant exists
    if self.activeVariantName is None:
      self.add(SSD('default'))

  def __del__(self):
    '''Cleans up temporary files upon deletion'''
    try:
      if self.temp_dir.exists():
        shutil.rmtree(self.temp_dir)
        logger.info(f"Temporary directory removed: {suppress_path_to_str(self.temp_dir)}")
    except Exception as e:
      logger.error(f"Error removing temporary directory {self.temp_dir}: {e}")

  def _extract_ssp(self, path: str):
    '''Extracts SSP file contents and loads SSD files.'''
    path = Path(path)
    if not path.is_file():
      raise FileNotFoundError(f"SSP file '{path}' not found")

    with zipfile.ZipFile(path, 'r') as ssp_zip:
      ssp_zip.extractall(self.temp_dir)
    logger.debug(f"Extracted SSP file: {path} -> {self.temp_dir}")

    ssd_files = [f for f in os.listdir(self.temp_dir) if f.endswith('.ssd') and f != 'SystemStructure.ssd']
    ssd_files.insert(0, 'SystemStructure.ssd')

    for path in self.temp_dir.rglob('*'):
      if path.is_file() and not path.suffix == '.ssd':
        self._addResource(path, path.relative_to(self.temp_dir), copy=False)

    logger.debug(f"SSD files: {ssd_files}")
    logger.debug(f"Resources: {self.resources}")

    for ssd_file in ssd_files:
      ssd_path = self.temp_dir / ssd_file
      ssd = SSD.importFromFile(ssd_path, self.resources)
      self.add(ssd)

  @property
  def activeVariantName(self):
    return self._activeVariantName

  @activeVariantName.setter
  def activeVariantName(self, variant_name: str):
    if variant_name not in self.variants:
      raise ValueError(f"Variant '{variant_name}' does not exist in the SSP.")
    self._activeVariantName = variant_name

  @property
  def activeVariant(self):
    return self.variants.get(self.activeVariantName)

  def addResource(self, filename: str, new_name: str | None = None, validate=True):
    self._addResource(filename, new_name, validate)

  def _addResource(self, filename: str, new_name: str | None = None, validate=True, copy=True):
    '''Adds a resource file to the SSP.'''

    new_name = Path(new_name) if new_name else Path('resources') / Path(filename).name

    if new_name in self.resources:
      raise ValueError(f"Resource '{new_name}' already exists in the SSP.")

    if validate:
      # TODO: Implement validation logic
      pass

    if copy:
      os.makedirs((self.temp_dir / new_name).parent, exist_ok=True)
      shutil.copy(filename, self.temp_dir / new_name)

    filePath = (self.temp_dir / new_name).resolve()
    if Path(filename).suffix == ".fmu":
      self.resources[str(new_name)] = FMU(fmu_path = filePath)
    elif Path(filename).suffix == ".ssv":
      self.resources[str(new_name)] = SSV(ssv_path = filePath)
    ##TODO check for .ssv file and if ssv instances provided
    else:
      self.resources[Path(filename).name] = new_name

  def getVariant(self, name=None):
    '''Returns the specified variant or the active variant.'''
    if name is None and self.activeVariantName is None:
      raise ValueError("No active variant set in the SSP.")
    return self.variants.get(name or self.activeVariantName)

  def getAllVariantNames(self):
    return list(self.variants.keys())

  def addComponent(self, cref: CRef, resource: str):
    if self.activeVariant is None:
      raise ValueError("No active variant set in the SSP.")

    ## look up in the resource if exist and then use that instance
    fmu_inst = None
    if resource in self.resources:
      fmu_inst = self.resources[resource]

    return self.activeVariant.addComponent(cref, resource, inst=fmu_inst)

  def newSolver(self, options: dict):
    if self.activeVariant is None:
      raise ValueError("No active variant set in the SSP.")
    self.activeVariant.newSolver(options)

  def addSSV(self, cref: CRef, resource: str):
    if self.activeVariant is None:
      raise ValueError("No active variant set in the SSP.")

    if resource not in self.resources:
      logger.warning(f"Resource '{resource}' not found in the SSP resources. Add the resource using the addResource API")

    self.activeVariant.addSSV(cref, resource)

  def addConnection(self, cref1: CRef, cref2: CRef):
    if self.activeVariant is None:
      raise ValueError("No active variant set in the SSP.")

    self.activeVariant.addConnection(cref1, cref2)

  def _getComponentResourcePath(self, cref: CRef):
    return self.activeVariant._getComponentResourcePath(cref)

  def setValue(self, cref: CRef, value, unit = None):
    if self.activeVariant is None:
      raise ValueError("No active variant set in the SSP.")

    ## look up in the resource and get the component path
    resource = self._getComponentResourcePath(cref)

    # Check if the resource exists and validate the variable
    fmu_inst = self.resources.get(resource)
    if fmu_inst and not fmu_inst.varExist(cref.last()):
        raise KeyError(f"Variable '{cref.last()}' does not exist in the variables list of component '{resource}'")

    self.activeVariant.setValue(cref, value, unit)

  def setSolver(self, cref: CRef, name: str):
    if self.activeVariant is None:
      raise ValueError("No active variant set in the SSP.")

    self.activeVariant.setSolver(cref, name)


  def addSystem(self, cref: CRef):
    if self.activeVariant is None:
      raise ValueError("No active variant set in the SSP.")

    self.activeVariant.addSystem(cref)

  def add(self, element):
    '''Adds an SSD or a list/iterable of SSDs to the SSP.'''
    if isinstance(element, SSD):
      self._addSSD(element)
    elif isinstance(element, Iterable) and not isinstance(element, (str, bytes)):
      for item in element:
        self.add(item)
    else:
      raise TypeError(f"SSP class does not support adding instance type of: {type(element)}")

  def _addSSD(self, ssd: SSD):
    '''Handles adding an SSD to the SSP'''
    if ssd.name in self.variants:
      if self.variants[ssd.name] is ssd:
        raise ValueError(f"Variant '{ssd.name}' is already part of the SSP")
      else:
        raise ValueError(f"Another variant with name '{ssd.name}' already exists in the SSP.")

    if ssd._model is not None:
      raise ValueError(f"SSD '{ssd.name}' already belongs to another SSP.")

    ssd._model = self
    self.variants[ssd.name] = ssd

    if self.activeVariantName is None:
      self.activeVariantName = ssd.name
      logger.debug(f"Active variant set to '{ssd.name}'")

  def list(self):
    '''Lists SSP contents'''
    print(type(self))
    print("|-- Resources:")
    for resource in sorted(self.resources):
      print(f"|--   {resource}")
      if isinstance(self.resources[resource], SSV):
        print(f"|--   |-- Parameter Bindings:")
        self.resources[resource].list("|--   |-- |--")
    for ssd in self.variants.values():
      ssd.list("|--")

  def export(self, filename: str):
    '''Exports the SSP to file'''
    logger.debug(f"Exporting SSP to {filename} using temp directory: {self.temp_dir}")

    exported_count = 0
    for ssd in self.variants.values():
      if ssd.dirty:
        exported_count += 1
        logger.debug(f"Dirty. Exporting SSD '{ssd.name}'")
        ssd_file_path = self.temp_dir / f'{ssd.name}.ssd'
        if ssd.name == self.activeVariantName:
          ssd_file_path = self.temp_dir / 'SystemStructure.ssd'

        ssd.export(ssd_file_path)

    if exported_count == 0:
      logger.debug("No SSDs needed exporting (all were up to date).")

    with zipfile.ZipFile(filename, 'w', zipfile.ZIP_DEFLATED) as ssp_zip:
      for file in self.temp_dir.rglob('*'):
        if file.is_file():
          archive_name = file.relative_to(self.temp_dir)
          ssp_zip.write(file, archive_name)

    logger.info(f"SSP file '{filename}' successfully exported!")
