-- status: correct
-- teardown_command: rm -rf test03-lua/ test_res.mat
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./test03-lua/")

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

function printType(t)
  if oms_system_tlm == t then
    print("type: oms_system_tlm")
  elseif oms_system_wc == t then
    print("type: oms_system_wc")
  elseif oms_system_sc == t then
    print("type: oms_system_sc")
  else
    print("Unknown type")
  end
end

oms_newModel("test")
oms_addSystem("test.eoo", oms_system_wc)
oms_addSubModel("test.eoo.source", "../resources/Modelica.Blocks.Sources.Sine.fmu")

-- save snapshot
src = oms_list("test")
print(src)

-- change model
oms_addSystem("test.eoo.goo", oms_system_sc)
oms_delete("test.eoo.source")
oms_addSubModel("test.eoo.source", "../resources/Modelica.Blocks.Sources.Constant.fmu")

-- restore model from snapshot
status = oms_loadSnapshot("test", src)
printStatus(status, 0)

src = oms_list("test")
print(src)

oms_instantiate("test")
oms_initialize("test")
oms_simulate("test")
oms_terminate("test")

oms_delete("test")

-- Result:
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="test" version="1.0">
-- 	<ssd:System name="eoo">
-- 		<ssd:Elements>
-- 			<ssd:Component name="source" type="application/x-fmu-sharedlibrary" source="resources/0001_source.fmu">
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="y" kind="output">
-- 						<ssc:Real />
-- 						<ssd:ConnectorGeometry x="1.000000" y="0.500000" />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="amplitude" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="freqHz" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="offset" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="phase" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="startTime" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 				</ssd:Connectors>
-- 			</ssd:Component>
-- 		</ssd:Elements>
-- 		<ssd:Annotations>
-- 			<ssc:Annotation type="org.openmodelica">
-- 				<oms:Annotations>
-- 					<oms:SimulationInformation>
-- 						<oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
-- 					</oms:SimulationInformation>
-- 				</oms:Annotations>
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:System>
-- 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
-- 		<ssd:Annotations>
-- 			<ssc:Annotation type="org.openmodelica">
-- 				<oms:Annotations>
-- 					<oms:SimulationInformation resultFile="test_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
-- 				</oms:Annotations>
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:DefaultExperiment>
-- </ssd:SystemStructureDescription>
--
-- info:    Delete source
-- status:  [correct] ok
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="test" version="1.0">
-- 	<ssd:System name="eoo">
-- 		<ssd:Elements>
-- 			<ssd:Component name="source" type="application/x-fmu-sharedlibrary" source="resources/0001_source.fmu">
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="y" kind="output">
-- 						<ssc:Real />
-- 						<ssd:ConnectorGeometry x="1.000000" y="0.500000" />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="amplitude" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="freqHz" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="offset" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="phase" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="startTime" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 				</ssd:Connectors>
-- 			</ssd:Component>
-- 		</ssd:Elements>
-- 		<ssd:Annotations>
-- 			<ssc:Annotation type="org.openmodelica">
-- 				<oms:Annotations>
-- 					<oms:SimulationInformation>
-- 						<oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
-- 					</oms:SimulationInformation>
-- 				</oms:Annotations>
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:System>
-- 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
-- 		<ssd:Annotations>
-- 			<ssc:Annotation type="org.openmodelica">
-- 				<oms:Annotations>
-- 					<oms:SimulationInformation resultFile="test_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
-- 				</oms:Annotations>
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:DefaultExperiment>
-- </ssd:SystemStructureDescription>
--
-- info:    Result file: test_res.mat (bufferSize=10)
-- endResult
