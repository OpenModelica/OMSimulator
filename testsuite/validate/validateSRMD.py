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
## warning: invalid "SimulationResourceMetaData" detected in file "resources/SRMD_example_D16.srmd" at line: 0 column: 0, unsupported protocol in URL
## warning: invalid "SimulationResourceMetaData" detected in file "resources/SRMD_example_D16.srmd" at line: 12 column: 116, fatal error during schema scan
## warning: Could not load schema file: C:/OPENMODELICAGIT/OpenModelica/build/bin/../share/OMSimulator/schema/ssp/SSPTraceabilityCommon.xsd
## warning: SRMD format validation of "../../resources/invalidSRMD.ssp" failed
## info:    4 warnings
## info:    0 errors
## endResult
