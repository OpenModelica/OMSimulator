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

# An architecture with placeholder components (no source): one placeholder is
# replaced by an FMU, the others stay placeholders. After the export the
# placeholders still have no source attribute and all connections are kept.

def printSSD(ssp):
  root = ET.fromstring(zipfile.ZipFile(ssp).read('SystemStructure.ssd'))
  ns = '{http://ssp-standard.org/SSP1/SystemStructureDescription}'
  for component in root.iter(ns + 'Component'):
    print(component.get('name'), 'source' in component.attrib, component.get('source'), flush=True)
  for connection in root.iter(ns + 'Connection'):
    print(connection.get('startElement') + '.' + connection.get('startConnector'), '->',
          connection.get('endElement') + '.' + connection.get('endConnector'), flush=True)

model = SSP('../resources/componentNoSource.ssp')
model.addResource('../resources/replaceA.fmu', new_name='resources/replaceA.fmu')
warnings = model.replaceComponent(CRef('default', 'noSource'), 'resources/replaceA.fmu')
for warning in warnings:
  print('warning:', warning, flush=True)
model.export('componentNoSource2.ssp')
printSSD('componentNoSource2.ssp')

## Result:
## emptySource False None
## noSource True resources/replaceA.fmu
## dotSource False None
## emptySource.y -> noSource.u
## noSource.y -> dotSource.u
## endResult
