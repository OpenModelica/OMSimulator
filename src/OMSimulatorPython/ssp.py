import logging
import os
import shutil
import tempfile
import zipfile
from pathlib import Path

from OMSimulator import SSD, CRef
from OMSimulator.settings import Settings
from OMSimulator.fmu import FMU

logger = logging.getLogger(__name__)

class SSP:
  def __init__(self, path: str | None = None, temp_dir: str | None = None):
    self._activeVariantName = None
    self.variants = {}
    self.resources = {}

    if temp_dir:
      os.makedirs(temp_dir, exist_ok=True)

    self.temp_dir = Path(tempfile.mkdtemp(dir=temp_dir)).resolve()
    logger.info(f"Temporary directory created: {self.temp_dir if not Settings.suppressPath else '<hidden>'}")

    if path:
      self._extract_ssp(path)

    # Ensure at least one variant exists
    if self.activeVariantName is None:
      self.add(SSD('default'))

  def __del__(self):
    '''Cleans up temporary files upon deletion'''
    try:
      if os.path.exists(self.temp_dir):
        shutil.rmtree(self.temp_dir)
        logger.info(f"Temporary directory removed: {self.temp_dir if not Settings.suppressPath else '<hidden>'}")
    except Exception as e:
      logger.error(f"Error removing temporary directory {self.temp_dir}: {e}")

  def _extract_ssp(self, path: str):
    '''Extracts SSP file contents and loads SSD files.'''
    if not os.path.isfile(path):
      raise FileNotFoundError(f"SSP file '{path}' not found")

    with zipfile.ZipFile(path, 'r') as ssp_zip:
      ssp_zip.extractall(self.temp_dir)
    logger.debug(f"Extracted SSP file: {path} -> {self.temp_dir}")

    ssd_files = [f for f in os.listdir(self.temp_dir) if f.endswith('.ssd') and f != 'SystemStructure.ssd']
    ssd_files.insert(0, 'SystemStructure.ssd')

    self.resources = [
      os.path.relpath(os.path.join(root, file), self.temp_dir)
      for root, _, files in os.walk(self.temp_dir)
      for file in files if not file.endswith('.ssd')
    ]

    logger.debug(f"SSD files: {ssd_files}")
    logger.debug(f"Resources: {self.resources}")

    for ssd_file in ssd_files:
      ssd_path = os.path.join(self.temp_dir, ssd_file)
      ssd = SSD.importFromFile(ssd_path)
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

  def addResource(self, filename: str, new_name: str | None = None, target_dir='resources/', validate=True):
    '''Adds a resource file to the SSP.'''
    new_name = new_name or os.path.basename(filename)
    resource_path = os.path.join(target_dir, new_name)

    if resource_path in self.resources:
      raise ValueError(f"Resource '{resource_path}' already exists in the SSP.")

    if validate:
      # TODO: Implement validation logic
      pass

    target_path = os.path.join(self.temp_dir, target_dir)
    os.makedirs(target_path, exist_ok=True)
    shutil.copy(filename, os.path.join(target_path, new_name))

    # Append to resources only after a successful copy
    self.resources[new_name] = FMU((Path(target_path) / new_name).resolve())

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

    self.activeVariant.addComponent(cref, resource, inst=fmu_inst)

  def add(self, element):
    '''Adds an SSD or a list of SSDs to the SSP'''
    if isinstance(element, list):
      for item in element:
        self.add(item)
      return

    if isinstance(element, SSD):
      self._addSSD(element)
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
    for resource in self.resources:
      print(f"|--   {resource}")
    for ssd in self.variants.values():
      ssd.list()

  def export(self, filename: str):
    '''Exports the SSP to file'''
    logger.debug(f"Exporting SSP to {filename} using temp directory: {self.temp_dir}")

    exported_count = 0
    for ssd in self.variants.values():
      if ssd.dirty:
        exported_count += 1
        logger.debug(f"Dirty. Exporting SSD '{ssd.name}'")
        ssd_file_path = os.path.join(self.temp_dir, f"{ssd.name}.ssd")
        if ssd.name == self.activeVariantName:
          ssd_file_path = os.path.join(self.temp_dir, 'SystemStructure.ssd')

        ssd.export(ssd_file_path)

    if exported_count == 0:
      logger.debug("No SSDs needed exporting (all were up to date).")

    with zipfile.ZipFile(filename, 'w', zipfile.ZIP_DEFLATED) as ssp_zip:
      for root, _, files in os.walk(self.temp_dir):
        for file in files:
          file_path = os.path.join(root, file)
          archive_name = os.path.relpath(file_path, self.temp_dir)
          ssp_zip.write(file_path, archive_name)

    logger.info(f"SSP file '{os.path.abspath(filename)}' successfully exported!")
