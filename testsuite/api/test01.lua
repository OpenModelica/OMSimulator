-- status: correct
-- teardown_command: rm -rf test01lua/
-- linux: yes
-- linux32: yes
-- ucrt64: yes
-- win: yes
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./test01lua/")

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

status = oms_newModel("test01lua")
printStatus(status, 0)

status = oms_addSystem("test01lua.foo", oms_system_wc)
printStatus(status, 0)

status = oms_addSystem("test01lua.foo.goo", oms_system_sc)
printStatus(status, 0)

status = oms_addSystem("test01lua.foo.hoo", oms_system_wc)
printStatus(status, 3)

src, status = oms_list("test01lua")
print(src)

src, status = oms_list("test01lua.foo")
print(src)

src, status = oms_list("test01lua.foo.goo")
print(src)

status = oms_newModel("test01lua")
printStatus(status, 3)

status = oms_rename("test01lua", "foo")
printStatus(status, 0)

status = oms_newModel("test01lua")
printStatus(status, 0)

status = oms_delete("test01lua")
printStatus(status, 0)

status = oms_delete("foo")
printStatus(status, 0)

status = oms_newModel("test01lua")
printStatus(status, 0)

src, status = oms_list("test01lua")
print(src)

status = oms_delete("test01lua")
printStatus(status, 0)

status = oms_delete("foo")
printStatus(status, 3)

-- Result:
-- status:  [correct] ok
-- error:   [addSystem] Model "test01lua" does not contain any system
-- status:  [correct] error
-- status:  [correct] ok
-- status:  [correct] ok
-- error:   [NewSystem] A WC system must be the root system or a subsystem of a TLM system.
-- status:  [correct] error
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="test01lua" version="1.0">
-- 	<ssd:System name="foo">
-- 		<ssd:Elements>
-- 			<ssd:System name="goo">
-- 				<ssd:Annotations>
-- 					<ssc:Annotation type="org.openmodelica">
-- 						<oms:Annotations>
-- 							<oms:SimulationInformation>
-- 								<oms:VariableStepSolver description="cvode" absoluteTolerance="0.0001" relativeTolerance="0.0001" minimumStepSize="1e-12" maximumStepSize="0.001" initialStepSize="1e-06" />
-- 							</oms:SimulationInformation>
-- 						</oms:Annotations>
-- 					</ssc:Annotation>
-- 				</ssd:Annotations>
-- 			</ssd:System>
-- 		</ssd:Elements>
-- 		<ssd:Annotations>
-- 			<ssc:Annotation type="org.openmodelica">
-- 				<oms:Annotations>
-- 					<oms:SimulationInformation>
-- 						<oms:FixedStepMaster description="oms-ma" stepSize="0.001000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
-- 					</oms:SimulationInformation>
-- 				</oms:Annotations>
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:System>
-- 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
-- 		<ssd:Annotations>
-- 			<ssc:Annotation type="org.openmodelica">
-- 				<oms:Annotations>
-- 					<oms:SimulationInformation resultFile="test01lua_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter="resources/signalFilter.xml" />
-- 				</oms:Annotations>
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:DefaultExperiment>
-- </ssd:SystemStructureDescription>
--
-- <?xml version="1.0"?>
-- <ssd:System name="foo">
-- 	<ssd:Elements>
-- 		<ssd:System name="goo">
-- 			<ssd:Annotations>
-- 				<ssc:Annotation type="org.openmodelica">
-- 					<oms:Annotations>
-- 						<oms:SimulationInformation>
-- 							<oms:VariableStepSolver description="cvode" absoluteTolerance="0.0001" relativeTolerance="0.0001" minimumStepSize="1e-12" maximumStepSize="0.001" initialStepSize="1e-06" />
-- 						</oms:SimulationInformation>
-- 					</oms:Annotations>
-- 				</ssc:Annotation>
-- 			</ssd:Annotations>
-- 		</ssd:System>
-- 	</ssd:Elements>
-- 	<ssd:Annotations>
-- 		<ssc:Annotation type="org.openmodelica">
-- 			<oms:Annotations>
-- 				<oms:SimulationInformation>
-- 					<oms:FixedStepMaster description="oms-ma" stepSize="0.001000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
-- 				</oms:SimulationInformation>
-- 			</oms:Annotations>
-- 		</ssc:Annotation>
-- 	</ssd:Annotations>
-- </ssd:System>
--
-- <?xml version="1.0"?>
-- <ssd:System name="goo">
-- 	<ssd:Annotations>
-- 		<ssc:Annotation type="org.openmodelica">
-- 			<oms:Annotations>
-- 				<oms:SimulationInformation>
-- 					<oms:VariableStepSolver description="cvode" absoluteTolerance="0.0001" relativeTolerance="0.0001" minimumStepSize="1e-12" maximumStepSize="0.001" initialStepSize="1e-06" />
-- 				</oms:SimulationInformation>
-- 			</oms:Annotations>
-- 		</ssc:Annotation>
-- 	</ssd:Annotations>
-- </ssd:System>
--
-- error:   [newModel] "test01lua" already exists in the scope
-- status:  [correct] error
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="test01lua" version="1.0">
-- 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
-- 		<ssd:Annotations>
-- 			<ssc:Annotation type="org.openmodelica">
-- 				<oms:Annotations>
-- 					<oms:SimulationInformation resultFile="test01lua_res.mat" loggingInterval="0.000000" bufferSize="10" />
-- 				</oms:Annotations>
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:DefaultExperiment>
-- </ssd:SystemStructureDescription>
--
-- status:  [correct] ok
-- error:   [deleteModel] Model "foo" does not exist in the scope
-- status:  [correct] error
-- info:    0 warnings
-- info:    4 errors
-- endResult
