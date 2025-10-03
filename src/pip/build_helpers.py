import os
import platform
import shutil
import sys
import sysconfig
import tempfile
from setuptools.command.build_py import build_py

# check for requests package
try:
  import requests
except ImportError:
  import subprocess
  subprocess.check_call([sys.executable, "-m", "pip", "install", "requests"])
  import requests

class my_build_py(build_py):
  """
  Custom build_py command to download OMSimulator binaries and copy them
  into the package during build.
  """
  def __init__(self, dist):
    # check if directory exist and remove
    topdir = os.path.join(os.getcwd(), 'OMSimulator')
    if os.path.isdir(topdir):
      os.rmdir(topdir)

    # recreate it, so that setuptools creates the package directory
    os.mkdir(topdir)
    build_py.__init__(self, dist)

  def fetch_oms(self):
    target_dir = os.path.join(self.build_lib, "OMSimulator")
    os.makedirs(target_dir, exist_ok=True)

    # Select platform-specific download
    platform_name = sysconfig.get_platform()
    system = platform.system()
    if platform_name == "linux-x86_64":
        url = "https://build.openmodelica.org/omsimulator/nightly/linux-amd64/OMSimulator-linux-amd64-@OMS_VERSION_STRING@.tar.gz"
        dll_path = "lib/x86_64-linux-gnu/libOMSimulator.so"
        source_path = "lib/x86_64-linux-gnu/OMSimulator"
    elif platform_name in ["mingw_x86_64_ucrt", "win-amd64"] or (platform_name == "mingw" and platform.architecture()[0] == "64bit"):
        url = "https://build.openmodelica.org/omsimulator/nightly/win-mingw-ucrt64/OMSimulator-mingw-ucrt64-@OMS_VERSION_STRING@.zip"
        dll_path = "bin/libOMSimulator.dll"
        source_path = "lib/OMSimulator"
    elif system == "Darwin":
        url = "https://build.openmodelica.org/omsimulator/nightly/osx/OMSimulator-osx-@OMS_VERSION_STRING@.zip"
        dll_path = "lib/libOMSimulator.dylib"
        source_path = "lib/OMSimulator"
    else:
        raise Exception(f"Platform not supported: {platform_name}")

    response = requests.get(url, verify=False)
    if response.status_code != 200:
        raise Exception(f"Failed to download {url}")

    # Save temp file
    temp_dir = tempfile.gettempdir()
    zip_file = os.path.join(temp_dir, os.path.basename(url))
    with open(zip_file, "wb") as f:
      f.write(response.content)

    # Extract archive
    extract_dir = os.path.join(temp_dir, "OMSimulator_extract")
    if os.path.exists(extract_dir):
        shutil.rmtree(extract_dir)

    shutil.unpack_archive(zip_file, extract_dir)

    # Copy package
    shutil.copytree(os.path.join(extract_dir, source_path), target_dir, dirs_exist_ok=True)
    # Copy schema if exists
    schema_src = os.path.join(extract_dir, "share/OMSimulator/schema")
    if os.path.exists(schema_src):
        shutil.copytree(schema_src, os.path.join(target_dir, "schema"), dirs_exist_ok=True)
    # Copy DLL
    dll_src = os.path.join(extract_dir, dll_path)
    if os.path.exists(dll_src):
      shutil.copy2(dll_src, target_dir)

    # Cleanup
    shutil.rmtree(extract_dir)
    os.remove(zip_file)

    def run(self):
      if not self.dry_run:
          self.fetch_oms()
      super().run()
