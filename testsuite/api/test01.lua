-- status: correct
-- teardown_command: rm -rf test01-lua/
-- linux: yes
-- linux32: yes
-- mingw: yes
-- win: yes
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./test01-lua/")

function printStatus(status, expected)
  cmp = ""
  if status == expected then
    cmp = "correct"
  else
    cmp = "wrong"
  end

  if 0 == status then
    status = "ok"
  elseif 1 == status then
    status = "warning"
  elseif 3 == status then
    status = "error"
  end
  print("status:  [" .. cmp .. "] " .. status)
end

status = oms_newModel("test")
printStatus(status, 0)

status = oms_addSystem("test", oms_system_tlm)
printStatus(status, 3)

status = oms_addSystem("test.foo", oms_system_wc)
printStatus(status, 0)

status = oms_addSystem("test.foo.goo", oms_system_sc)
printStatus(status, 0)

status = oms_addSystem("test.foo.hoo", oms_system_wc)
printStatus(status, 3)

src, status = oms_list("test")
print(src)

src, status = oms_list("test.foo")
print(src)

src, status = oms_list("test.foo.goo")
print(src)

status = oms_newModel("test")
printStatus(status, 3)

status = oms_rename("test", "foo")
printStatus(status, 0)

status = oms_newModel("test")
printStatus(status, 0)

status = oms_delete("test")
printStatus(status, 0)

status = oms_delete("foo")
printStatus(status, 0)

status = oms_newModel("test")
printStatus(status, 0)

src, status = oms_list("test")
print(src)

status = oms_delete("test")
printStatus(status, 0)

status = oms_delete("foo")
printStatus(status, 3)

-- Result:
-- status:  [correct] ok
-- error:   [addSystem] Model "test" does not contain any system
-- status:  [correct] error
-- status:  [correct] ok
-- status:  [correct] ok
-- error:   [NewSystem] A WC system must be the root system or a subsystem of a TLM system.
-- status:  [correct] error
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription name="test" version="Draft20180219">
-- 	<ssd:System name="foo">
-- 		<ssd:SimulationInformation>
-- 			<FixedStepMaster description="oms-ma" stepSize="0.100000" />
-- 		</ssd:SimulationInformation>
-- 		<ssd:Elements>
-- 			<ssd:System name="goo">
-- 				<ssd:SimulationInformation>
-- 					<VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
-- 				</ssd:SimulationInformation>
-- 				<ssd:Elements />
-- 				<ssd:Connectors />
-- 				<ssd:Connections />
-- 			</ssd:System>
-- 		</ssd:Elements>
-- 		<ssd:Connectors />
-- 		<ssd:Connections />
-- 	</ssd:System>
-- 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000" />
-- </ssd:SystemStructureDescription>
--
-- <?xml version="1.0"?>
-- <ssd:System name="foo">
-- 	<ssd:SimulationInformation>
-- 		<FixedStepMaster description="oms-ma" stepSize="0.100000" />
-- 	</ssd:SimulationInformation>
-- 	<ssd:Elements>
-- 		<ssd:System name="goo">
-- 			<ssd:SimulationInformation>
-- 				<VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
-- 			</ssd:SimulationInformation>
-- 			<ssd:Elements />
-- 			<ssd:Connectors />
-- 			<ssd:Connections />
-- 		</ssd:System>
-- 	</ssd:Elements>
-- 	<ssd:Connectors />
-- 	<ssd:Connections />
-- </ssd:System>
--
-- <?xml version="1.0"?>
-- <ssd:System name="goo">
-- 	<ssd:SimulationInformation>
-- 		<VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
-- 	</ssd:SimulationInformation>
-- 	<ssd:Elements />
-- 	<ssd:Connectors />
-- 	<ssd:Connections />
-- </ssd:System>
--
-- error:   [newModel] "test" already exists in the scope
-- status:  [correct] error
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription name="test" version="Draft20180219">
-- 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000" />
-- </ssd:SystemStructureDescription>
--
-- status:  [correct] ok
-- error:   [deleteModel] Model "foo" does not exist in the scope
-- status:  [correct] error
-- info:    0 warnings
-- info:    4 errors
-- endResult
