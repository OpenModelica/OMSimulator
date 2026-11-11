## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: yes

import zipfile
from lxml import etree as ET
from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# A component without a source (an architecture placeholder) must be exported
# without a source attribute. An empty source, "." or no attribute on import
# all mean "no source"; a component with an FMU keeps its source.

def printSources(ssp):
  root = ET.fromstring(zipfile.ZipFile(ssp).read('SystemStructure.ssd'))
  for component in root.iter('{http://ssp-standard.org/SSP1/SystemStructureDescription}Component'):
    print(component.get('name'), 'source' in component.attrib, component.get('source'), flush=True)

model = SSP('../resources/componentNoSource.ssp')
model.addResource('../resources/replaceA.fmu', new_name='resources/replaceA.fmu')
model.addComponent(CRef('default', 'A'), 'resources/replaceA.fmu')
model.export('componentNoSource1.ssp')
printSources('componentNoSource1.ssp')

model2 = SSP('componentNoSource1.ssp')
model2.export('componentNoSource1_2.ssp')
printSources('componentNoSource1_2.ssp')

try:
  model2.instantiate()
except ValueError as e:
  print('error:', e, flush=True)

## Result:
## emptySource False None
## noSource False None
## dotSource False None
## A True resources/replaceA.fmu
## emptySource False None
## noSource False None
## dotSource False None
## A True resources/replaceA.fmu
## error: Component 'emptySource' has no source FMU and cannot be simulated
## endResult
