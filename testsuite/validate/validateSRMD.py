## status: correct
## teardown_command: rm -rf validateSRMD-py/
## linux: yes
## mac: no
## mingw32: no
## mingw64: yes
## win: yes

import OMSimulator as oms

oms.setCommandLineOption('--suppressPath=true')
oms.setTempDirectory('./validateSRMD-py/')

model = oms.importFile('../resources/invalidSRMD.ssp')

model.delete()

## Result:
# warning: invalid "SimulationResourceMetaData" detected in file "resources/SRMD_example_D16.srmd" at line: 9 column: 44, no declaration found for element 'OperationalDomain'
# warning: XML parse error: validation failed.
# warning: SRMD format validation of "../../resources/invalidSRMD.ssp" failed
# info:    3 warnings
# info:    0 errors
## endResult
