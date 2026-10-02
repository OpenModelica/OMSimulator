## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: yes

import zipfile
from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

## MetaData of the SystemStructureDescription itself and several MetaData
## elements on one component must survive an import/export round trip,
## including attributes and inline content the API does not model.

def printMetaData(filename):
  with zipfile.ZipFile(filename) as ssp:
    ssd = ssp.read('SystemStructure.ssd').decode('utf-8')
  for line in ssd.splitlines():
    if 'MetaData' in line or 'Content' in line:
      print(line.strip())

model = SSP()
model.addResource('../resources/acvs_compositeModel/resources/L90LS_UD_OS_OS.fmu', new_name='resources/L90.fmu')
model.addResource('../resources/acvs_compositeModel/resources/od_L90LS_UD_OS_OS.xml', new_name='resources/od.xml')
model.addComponent(CRef('default', 'L90'), 'resources/L90.fmu')
## MetaData on the SystemStructureDescription
model.addMetaDataReference(None, 'resources/od.xml', kind='quality', type='application/x-omuq')
## two MetaData elements on the same component
model.addMetaDataReference(CRef('default', 'L90'), 'resources/od.xml', type='application/x-a')
model.addMetaDataReference(CRef('default', 'L90'), 'resources/od.xml', type='application/x-b')
model.export('metadata2.ssp')

## add a root MetaData element with an id and inline content, as another tool would write it
with zipfile.ZipFile('metadata2.ssp') as ssp:
  files = {name: ssp.read(name) for name in ssp.namelist()}
inline = b'<ssc:MetaData kind="general" type="text/plain" id="note"><ssc:Content>hello</ssc:Content></ssc:MetaData>'
files['SystemStructure.ssd'] = files['SystemStructure.ssd'].replace(b'</ssd:SystemStructureDescription>', inline + b'</ssd:SystemStructureDescription>')
with zipfile.ZipFile('metadata2-edited.ssp', 'w') as ssp:
  for name, data in files.items():
    ssp.writestr(name, data)

model2 = SSP('metadata2-edited.ssp')
model2.export('metadata2-roundtrip.ssp')
printMetaData('metadata2-roundtrip.ssp')

## Result:
## <ssc:MetaData kind="general" type="application/x-a" source="resources/od.xml"/>
## <ssc:MetaData kind="general" type="application/x-b" source="resources/od.xml"/>
## <ssc:MetaData kind="quality" type="application/x-omuq" source="resources/od.xml"/>
## <ssc:MetaData kind="general" type="text/plain" id="note">
## <ssc:Content>hello</ssc:Content>
## </ssc:MetaData>
## endResult
