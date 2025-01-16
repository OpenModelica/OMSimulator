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

oms.importFile('../resources/invalidSRMD.ssp')


## Result:
## warning: invalid "SimulationResourceMetaData" detected in file "/OpenModelica/OMSimulator/testsuite/validate/validateSRMD-lua/import_export_parameters-2sc3pl7p/resources/SRMD_example_D16.srmd" at line: 9 column: 44, no declaration found for element 'OperationalDomain'
## warning: XML parse error: validation failed.
## error:   [importModel] SRMD format validation of "/OpenModelica/OMSimulator/testsuite/validate/validateSRMD-lua/import_export_parameters-2sc3pl7p/resources/SRMD_example_D16.srmd" failed

## info:    2 warnings
## info:    1 errors
## endResult
