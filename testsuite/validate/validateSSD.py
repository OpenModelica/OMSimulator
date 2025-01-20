## status: correct
## teardown_command: rm -rf validateSSD-py/
## linux: yes
## mac: no
## mingw32: no
## mingw64: yes
## win: yes

import OMSimulator as oms

oms.setCommandLineOption('--suppressPath=true')
oms.setTempDirectory('./validateSSD-py/')

model = oms.importFile('../resources/invalidSSD.ssp')

model.delete()

## Result:
## warning: invalid "SystemStructureDescription" detected in file "../resources/invalidSSD.ssp" at line: 16 column: 22, attribute 'type' is not declared for element 'Connector'
## warning: "SystemStructureDescription" does not conform to the SSP standard schema
## info:    2 warnings
## info:    0 errors
## endResult
