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

oms_importFile('../../resources/invalidSRMD.ssp')
--oms_terminate()
--oms_delete()

-- Result:
-- warning: invalid "SimulationResourceMetaData" detected in file "/OpenModelica/OMSimulator/testsuite/validate/validateSRMD-lua/import_export_parameters-2sc3pl7p/resources/SRMD_example_D16.srmd" at line: 9 column: 44, no declaration found for element 'OperationalDomain'
-- warning: XML parse error: validation failed.
-- error:   [importModel] SRMD format validation of "/OpenModelica/OMSimulator/testsuite/validate/validateSRMD-lua/import_export_parameters-2sc3pl7p/resources/SRMD_example_D16.srmd" failed

-- info:    2 warnings
-- info:    1 errors
-- endResult
