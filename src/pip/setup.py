#!/usr/bin/env python3
# -*- coding: utf-8 -*-

__license__ = '''
This file is part of OpenModelica.
Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
c/o Linköpings universitet, Department of Computer and Information Science,
SE-58183 Linköping, Sweden.
All rights reserved.
THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
ACCORDING TO RECIPIENTS CHOICE.
The OpenModelica software and the Open Source Modelica
Consortium (OSMC) Public License (OSMC-PL) are obtained
from OSMC, either from the above address,
from the URLs: http://www.ida.liu.se/projects/OpenModelica or
http://www.openmodelica.org, and in the OpenModelica distribution.
GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
This program is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of  MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
See the full OSMC Public License conditions for more details.
'''

__author__ = 'Open Source Modelica Consortium (OSMC)'
__maintainer__ = 'https://openmodelica.org'
__status__ = 'Production'


import os
import platform
import shutil
import sysconfig
import tempfile
from distutils.command.build_py import build_py
from distutils.dir_util import copy_tree
from distutils.file_util import copy_file

from setuptools import setup

## check for requests package, if not found force install it through pip (e.g) "python -m pip install requests"
try:
  import requests
except ImportError as e:
  print(e.args, "Trying to install package \"requests\" from pip \"python -m pip install requests\"")
  import subprocess, sys
  subprocess.call([sys.executable,'-m', 'pip', 'install','requests'])

# override build_py, for compiling and copying the dlls
class my_build_py(build_py):
  def __init__(self, dist):
    # check if directory exist and remove
    topdir = os.path.join(os.getcwd(), 'OMSimulator')
    if os.path.isdir(topdir):
      os.rmdir(topdir)

    # recreate it, so that setuptools creates the package directory
    os.mkdir(topdir)

    build_py.__init__(self, dist)

  def fetch_oms(self):
    target_dir = os.path.join(self.build_lib, 'OMSimulator')

    # mkpath is a distutils helper to create directories
    self.mkpath(target_dir)

    # download the zip directory from url
    if (sysconfig.get_platform() == 'linux-x86_64'):
      response = requests.get('https://build.openmodelica.org/omsimulator/nightly/linux-amd64/OMSimulator-linux-amd64-@OMS_VERSION_STRING@.tar.gz', verify=False)
      dllpath  = "lib/x86_64-linux-gnu/libOMSimulator.so"
      sourcePath = "lib/x86_64-linux-gnu/OMSimulator"
    elif (sysconfig.get_platform() == "mingw_x86_64_ucrt" or (sysconfig.get_platform() == 'mingw' and platform.architecture()[0] == '64bit')):
      response = requests.get('https://build.openmodelica.org/omsimulator/nightly/win-mingw-ucrt64/OMSimulator-mingw-ucrt64-@OMS_VERSION_STRING@.zip', verify=False)
      dllpath  = "bin/libOMSimulator.dll"
      sourcePath = "lib/OMSimulator"
    elif (sysconfig.get_platform() == 'win-amd64'):
      response = requests.get('https://build.openmodelica.org/omsimulator/nightly/win-msvc64/OMSimulator-win64-@OMS_VERSION_STRING@.zip', verify=False)
      dllpath  = "bin/OMSimulator.dll"
      sourcePath = "lib/OMSimulator"
    elif (platform.system() == 'Darwin'):
      response = requests.get('https://build.openmodelica.org/omsimulator/nightly/osx/OMSimulator-osx-@OMS_VERSION_STRING@.zip', verify=False)
      dllpath  = "lib/libOMSimulator.dylib"
      ## the path needs to be checked, as of now we are not buiding artifacts for macos
      sourcePath = "lib/OMSimulator"
    else:
      raise Exception("Platform not supported for {} ".format(sysconfig.get_platform()))

    if response.status_code != 200:
      raise Exception("Downloading artifact failed for {} ".format(response.url))

    zipFileName = os.path.basename(response.url)
    dirname, extension = os.path.splitext(zipFileName)

    tempDir = tempfile.gettempdir()
    zipFilePath = os.path.join(tempDir, zipFileName)

    # remove zip file if exists
    if os.path.exists(zipFilePath):
      os.remove(zipFilePath)

    # copy the zip file in temp directory
    zipfile = open(zipFilePath, "wb")
    zipfile.write(response.content)
    zipfile.close()

    zipDir = os.path.join(tempDir, dirname)
    # remove zip directory if exists
    if os.path.isdir(zipDir):
      shutil.rmtree(zipDir)

    # unzip the zip file
    shutil.unpack_archive(zipFilePath, zipDir)

    # copy OMSimulator package to root directory
    copy_tree(os.path.join(zipDir, sourcePath), target_dir)

    # copy schema path to OMSimulator/schema
    copy_tree(os.path.join(zipDir, 'share/OMSimulator/schema'), target_dir +"/schema")

    # copy dll to root directory
    copy_file(os.path.join(zipDir, dllpath), target_dir)

    # remove the zip directory after copying the files
    shutil.rmtree(zipDir)

    # remove the zip file
    os.remove(zipFilePath)

  def run(self):
    if not self.dry_run:
      self.fetch_oms()
    build_py.run(self)

with open('README.md', 'r') as fh:
  long_description = fh.read()

setup(
  name = 'OMSimulator',
  version = '@OMS_SHORT_VERSION_STRING@',
  description = 'OMSimulator - the FMI-based co-simulation environment for Python',
  long_description = long_description,
  long_description_content_type = 'text/markdown',
  author = 'Open Source Modelica Consortium (OSMC)',
  author_email = 'openmodelicadevelopers@ida.liu.se',
  license = "BSD, OSMC-PL 1.2, GPL (user's choice)",
  url = 'http://openmodelica.org/',
  setup_requires = ['requests'],
  install_requires = ['requests',
                      'lxml>=5.3.1'],
  packages = ['OMSimulator'],
  cmdclass = {'build_py': my_build_py},
  zip_safe = False
)
