## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: yes

import zipfile
from OMSimulator import FMU

# An FMI 3 FMU writes its patch release into fmiVersion (the schema allows
# "3.0", "3.0.1", "3.1-dev", ...), so a 3.0.1 FMU must be read like a 3.0 one.
# The same FMU twice: as it is ("3.0") and with fmiVersion="3.0.1".
with zipfile.ZipFile('../resources/BouncingBall3.fmu') as src, zipfile.ZipFile('fmuinfo3_301.fmu', 'w') as dst:
  for item in src.infolist():
    data = src.read(item.filename)
    if item.filename == 'modelDescription.xml':
      assert b'fmiVersion="3.0"' in data
      data = data.replace(b'fmiVersion="3.0"', b'fmiVersion="3.0.1"', 1)
    dst.writestr(item, data)

for path in ['../resources/BouncingBall3.fmu', 'fmuinfo3_301.fmu']:
  fmu = FMU(path)
  print('fmi version', fmu.fmiVersion)
  print('  variables', sorted(str(var.name) for var in fmu.variables))
  print('  connectors', sorted(str(c.name) for c in fmu.makeConnectors()))

## Result:
## fmi version 3.0
##   variables ['der(h)', 'der(v)', 'e', 'g', 'h', 'time', 'v', 'v_min']
##   connectors ['e', 'g', 'h', 'v']
## fmi version 3.0.1
##   variables ['der(h)', 'der(v)', 'e', 'g', 'h', 'time', 'v', 'v_min']
##   connectors ['e', 'g', 'h', 'v']
## endResult
