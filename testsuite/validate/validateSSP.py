## status: correct
## teardown_command: rm -rf validateSSP-py/
## linux: yes
## mac: no
## mingw32: yes
## mingw64: yes
## win: yes

import OMSimulator as oms

oms.setCommandLineOption('--suppressPath=true')
oms.setTempDirectory('./validateSSP-py/')

model = oms.importFile('../resources/invalidSSP.ssp')

model.delete()

## Result:
## error:   [reportParseException] invalid SystemStructure.ssd detected in file "../resources/invalidSSP.ssp" at line: 16 column: 22, attribute 'type' is not declared for element 'Connector'
## error:   [validateSSD] SystemStructure.ssd does not conform to the SSP standard schema
## info:    0 warnings
## info:    2 errors
## endResult
