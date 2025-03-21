import os
import shutil
import tempfile
import zipfile

from OMSimulator.ssd import SSD

import logging

class SSP:
  def __init__(self, path: str | None = None, temp_dir: str | None = None):
    self._activeVariantName = None
    self.variants = dict()
    self.resources = list()

    if temp_dir:
      os.makedirs(temp_dir, exist_ok=True)

    self.tempSSPDirectory = tempfile.mkdtemp(dir=temp_dir)

    logging.debug(f"DEBUG Temp: {self.tempSSPDirectory}")

    # If a ssp file is provided, import it
    if path:
      if not os.path.isfile(path):
        raise FileNotFoundError(f"SSP file '{path}' not found")

      with zipfile.ZipFile(path, 'r') as ssp_zip:
        ssp_zip.extractall(self.tempSSPDirectory)
      logging.debug(f"DEBUG UNZIP: {path} -> {self.tempSSPDirectory}")

      # All variants
      ssd_files = [f for f in os.listdir(self.tempSSPDirectory) if f.endswith('.ssd') and f != 'SystemStructure.ssd']
      ssd_files.insert(0, 'SystemStructure.ssd')

      # All resources
      self.resources = [os.path.relpath(os.path.join(root, file), self.tempSSPDirectory)
                        for root, _, files in os.walk(self.tempSSPDirectory)
                        for file in files if not file.endswith('.ssd')]
      logging.debug(f"DEBUG ssd_files: {ssd_files}")
      logging.debug(f"DEBUG resources: {self.resources}")

      for ssd_file in ssd_files:
        ssd_file_path = os.path.join(self.tempSSPDirectory, ssd_file)
        if not os.path.exists(ssd_file_path):
          raise FileNotFoundError(f"SSD file '{ssd_file_path}' not found!")

        ssd = SSD.importFromFile(ssd_file_path)
        self.add(ssd)

    # If no variants are present, add a default one
    if not self._activeVariantName:
      self.add(SSD('default'))

  def __del__(self):
    if self.tempSSPDirectory and os.path.exists(self.tempSSPDirectory):
      shutil.rmtree(self.tempSSPDirectory)
    logging.debug(f"DEBUG Temp Removed: {self.tempSSPDirectory}")

  @property
  def activeVariantName(self):
    return self._activeVariantName

  @activeVariantName.setter
  def activeVariantName(self, variant_name: str):
    if variant_name not in self.variants:
      raise ValueError(f"Variant '{variant_name}' does not exist in the SSP.")

    self._activeVariantName = variant_name

  def getActiveVariantName(self):
    return self._activeVariantName

  def getActiveVariant(self):
    return self.variants[self._activeVariantName]

  def getAllVariantNames(self):
    return list(self.variants.keys())

  def add(self, element):
    if isinstance(element, list):
      for item in element:
        self.add(item)
        return

    if isinstance(element, SSD):
      self._addSSD(element)
    else:
      raise TypeError(f"SSP class does not support adding instance type of : {type(element)}")

  def _addSSD(self, ssd: SSD):
    if ssd.name in self.variants:
      raise ValueError(f"Variant '{ssd.name}' already exists in the SSP.")

    if not self.variants:
      self._activeVariantName = ssd.name
      logging.debug(f"DEBUG: Active variant set to '{ssd.name}'")
    self.variants[ssd.name] = ssd

  def list(self):
    print("List SSP file:")
    for name, ssd in self.variants.items():
      if name == self._activeVariantName:
        print(f"├── Variant: {name} (Active)")
      else:
        print(f"├── Variant: {name}")
      ssd.list()

  def export(self, filename: str):
    temp_dir = 'temp_ssp'
    os.makedirs(temp_dir, exist_ok=True)

    resources_dir = os.path.join(temp_dir, 'resources')
    os.makedirs(resources_dir, exist_ok=True)

    for name, ssd in self.variants.items():
      ssd_file_path = os.path.join(temp_dir, name + '.ssd')
      if name == self._activeVariantName:
        ssd_file_path = os.path.join(temp_dir, 'SystemStructure.ssd')

      ssd.export(ssd_file_path)

      ssd.exportParameterResources(resources_dir)

      self.copyResourse(resourcedir=resources_dir)

    with zipfile.ZipFile(filename, 'w', zipfile.ZIP_DEFLATED) as ssp_zip:
      for root, _, files in os.walk(temp_dir):
        for file in files:
          file_path = os.path.join(root, file)
          archive_name = os.path.relpath(file_path, temp_dir)
          ssp_zip.write(file_path, archive_name)

    logging.debug(f"SSP file '{os.path.abspath(filename)}' successfully exported!")

    # Delete temp directory and all its contents
    if os.path.exists(temp_dir):
      shutil.rmtree(temp_dir)

  def copyResourse(self, resourcedir):
    ## copy the fmu's to resource folder
    for ssd in self.variants.values():
      for cref, fmu in ssd.getComponents().items():
        shutil.copy(os.path.abspath(fmu.getFmuPath()), os.path.join(resourcedir, cref+".fmu"))
