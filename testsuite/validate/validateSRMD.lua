-- status: correct
-- teardown_command: rm -rf validateSRMD-lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: yes
-- asan: yes

oms_setCommandLineOption('--suppressPath=true')
oms_setTempDirectory('./validateSRMD-lua/')
oms_setWorkingDirectory("./validateSRMD-lua/")

model = oms_importFile('../../resources/invalidSRMD.ssp')
oms_terminate(model)
oms_delete(model)

-- Result:
-- warning: invalid "SimulationResourceMetaData" detected in file "resources/SRMD_example_D16.srmd" at line: 9 column: 44, no declaration found for element 'OperationalDomain'
-- warning: XML parse error: validation failed.
-- warning: SRMD format validation of "../../resources/invalidSRMD.ssp" failed
-- info:    3 warnings
-- info:    0 errors
-- endResult
