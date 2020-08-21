try:
  from setuptools import setup
except ImportError:
  from distutils.core import setup

import subprocess
import os, sys, sysconfig
import multiprocessing
import git

clonedir = os.path.join(os.getcwd(),"OMSimulatorGit")

## clone OMSiumlator repository if not exist 
if (not os.path.isdir(os.path.join(os.getcwd(),"OMSimulatorGit"))):
  git.Repo.clone_from('https://github.com/OpenModelica/OMSimulator', 'OMSimulatorGit', recursive=True)
#print(os.getcwd())

os.chdir(clonedir)
#print(os.getcwd())

if (sysconfig.get_platform() == 'linux-x86_64' or sysconfig.get_platform() == 'mingw'):
  subprocess.call(['make', 'config-3rdParty'])
  subprocess.call(['make', 'config-OMSimulator'])
  subprocess.call(['make', 'OMSimulator', str(multiprocessing.cpu_count())])

## TODO subprocess call for windows 

datafiles = []
installdir = os.path.join(os.getcwd(),"install") 
for root, dirs, files in os.walk(installdir):
  path = root.split(os.sep)
  for file in files:
    (tmpfile, ext) = os.path.splitext(file)
    if((tmpfile == "libOMSimulator" or file== "OMSimulator") and (ext == ".dll" or ext==".so")):
      datafiles.append(('OMSimulator',[os.path.join(root,file)]))

#print(datafiles)
os.chdir("../")

setup(
      name='OMSimulator',
      version='2.0.1',
      description='OSimulator-Python API Interface',
      author='Arunkumar Palanisamy',
      author_email='arunkumar.palanisamy@liu.se',
      maintainer='Arunkumar Palanisamy',
      maintainer_email='arunkumar.palanisamy@liu.se',
      license="BSD, OSMC-PL 1.2, GPL (user's choice)",
      url='http://openmodelica.org/',
      packages=['OMSimulator'],
      data_files = datafiles,
      install_requires=['GitPython']
      )
