## status: correct
## teardown_command: rm -rf validateSSM-py/
## linux: yes
## mac: no
## mingw32: no
## mingw64: yes
## win: yes

import OMSimulator as oms

oms.setCommandLineOption('--suppressPath=true')
oms.setTempDirectory('./validateSSM-py/')

model = oms.importFile('../resources/invalidSSM.ssp')

model.delete()

## Result:
## warning: invalid "SystemStructureParameterMapping" detected in file "resources/import_parameter_mapping.ssm" at line: 12 column: 85, attribute 'destination' is not declared for element 'MappingEntry'
## warning: "SystemStructureParameterMapping" does not conform to the SSP standard schema
## info:    2 warnings
## info:    0 errors
## endResult
