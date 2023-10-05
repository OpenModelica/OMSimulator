## status: correct
## teardown_command: rm -rf validateSSV-py/
## linux: yes
## mac: no
## mingw32: yes
## mingw64: yes
## win: yes

import OMSimulator as oms

oms.setCommandLineOption('--suppressPath=true')
oms.setTempDirectory('./validateSSV-py/')

model = oms.importFile('../resources/invalidSSV.ssp')

model.delete()

## Result:
## warning: invalid "SystemStructureParameterValues" detected in file "resources/System1.ssv" at line: 4 column: 48, attribute 'type' is not declared for element 'Parameter'
## warning: "SystemStructureParameterValues" does not conform to the SSP standard schema
## info:    2 warnings
## info:    0 errors
## endResult
