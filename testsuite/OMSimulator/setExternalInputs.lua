-- status: correct
-- linux: yes
-- mingw: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./exportConnectors/")

oms_newModel("setExternalInputs")

oms_addSystem("setExternalInputs.Root", oms_system_wc)

oms_addSubModel("setExternalInputs.Root.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")

oms_setStopTime("setExternalInputs", 5.0)

oms_instantiate("setExternalInputs")
oms_initialize("setExternalInputs")

oms_stepUntil("setExternalInputs", 1.0)
print("info:      setExternalInputs.Root.Gain.u at time 1.0: " .. oms_getReal("setExternalInputs.Root.Gain.u"))

-- set external input
oms_setReal("setExternalInputs.Root.Gain.u", 5.0)

oms_stepUntil("setExternalInputs", 3.0)
print("info:      setExternalInputs.Root.Gain.u at time 3.0: " .. oms_getReal("setExternalInputs.Root.Gain.u"))

-- set external input
oms_setReal("setExternalInputs.Root.Gain.u", 10.0)

oms_stepUntil("setExternalInputs", 5.0)
print("info:      setExternalInputs.Root.Gain.u at time 5.0: " .. oms_getReal("setExternalInputs.Root.Gain.u"))

src = oms_list("setExternalInputs")
print(src)

oms_terminate("setExternalInputs")
oms_delete("setExternalInputs")

-- Result:
-- info:    Result file: setExternalInputs_res.mat (bufferSize=10)
-- info:      setExternalInputs.Root.Gain.u at time 1.0: 0.0
-- info:      setExternalInputs.Root.Gain.u at time 3.0: 5.0
-- info:      setExternalInputs.Root.Gain.u at time 5.0: 10.0
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="setExternalInputs" version="1.0">
-- 	<ssd:System name="Root">
-- 		<ssd:Annotations>
-- 			<ssc:Annotation type="org.openmodelica">
-- 				<oms:SimulationInformation>
-- 					<oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
-- 				</oms:SimulationInformation>
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 		<ssd:Connectors />
-- 		<ssd:Elements>
-- 			<ssd:Component name="Gain" type="application/x-fmu-sharedlibrary" source="resources/0001_Gain.fmu">
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="u" kind="input">
-- 						<ssc:Real />
-- 						<ssd:ConnectorGeometry x="0.000000" y="0.500000" />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="y" kind="output">
-- 						<ssc:Real />
-- 						<ssd:ConnectorGeometry x="1.000000" y="0.500000" />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="k" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 				</ssd:Connectors>
-- 			</ssd:Component>
-- 		</ssd:Elements>
-- 		<ssd:Connections />
-- 	</ssd:System>
-- 	<ssd:DefaultExperiment startTime="0.000000" stopTime="5.000000">
-- 		<ssd:Annotations>
-- 			<ssc:Annotation type="org.openmodelica">
-- 				<oms:SimulationInformation resultFile="setExternalInputs_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter="" />
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:DefaultExperiment>
-- </ssd:SystemStructureDescription>
-- 
-- endResult
