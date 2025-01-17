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
-- warning: invalid "SimulationResourceMetaData" detected in file "resources/SRMD_example_D16.srmd" at line: 0 column: 0, unsupported protocol in URL
-- warning: invalid "SimulationResourceMetaData" detected in file "resources/SRMD_example_D16.srmd" at line: 12 column: 116, fatal error during schema scan
-- warning: Could not load schema file: C:/OPENMODELICAGIT/OpenModelica/build/bin/../share/OMSimulator/schema/ssp/SSPTraceabilityCommon.xsd
-- warning: SRMD format validation of "../../resources/invalidSRMD.ssp" failed
-- info:    4 warnings
-- info:    0 errors
-- endResult
