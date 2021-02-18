-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true --exportParametersInline=false")
oms_setTempDirectory("./new_snapshot/")

oms_newModel("newsnapshot")
oms_addSystem("newsnapshot.root", oms_system_wc)

oms_addConnector("newsnapshot.root.C1", oms_causality_input, oms_signal_type_real)
oms_setReal("newsnapshot.root.C1", -10)

oms_addSubModel("newsnapshot.root.add", "../resources/Modelica.Blocks.Math.Add.fmu")
oms_setReal("newsnapshot.root.add.u1", 10)
oms_setReal("newsnapshot.root.add.k1", 30)

snapshot = oms_exportSnapshot("newsnapshot")
print(snapshot)

--error
snapshot = oms_exportSnapshot("newsnapshot.root.add")

-- error
snapshot = oms_exportSnapshot("newsnapshot:resources/snapshot1.ssv")

snapshot = oms_exportSnapshot("newsnapshot:resources/newsnapshot.ssv")
print(snapshot)

-- error
oms_importSnapshot("newsnapshot:resources/newsnapshot1.ssv", snapshot)

-- correct
oms_importSnapshot("newsnapshot:resources/newsnapshot.ssv", snapshot)

oms_delete("newsnapshot")


-- Result:
-- <?xml version="1.0"?>
-- <oms:snapshot>
-- 	<oms:ssd_file name="SystemStructure.ssd">
-- 		<ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="newsnapshot" version="1.0">
-- 			<ssd:System name="root">
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="C1" kind="input">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 				</ssd:Connectors>
-- 				<ssd:ParameterBindings>
-- 					<ssd:ParameterBinding source="resources/newsnapshot.ssv" />
-- 				</ssd:ParameterBindings>
-- 				<ssd:Elements>
-- 					<ssd:Component name="add" type="application/x-fmu-sharedlibrary" source="resources/0001_add.fmu">
-- 						<ssd:Connectors>
-- 							<ssd:Connector name="u1" kind="input">
-- 								<ssc:Real />
-- 								<ssd:ConnectorGeometry x="0.000000" y="0.333333" />
-- 							</ssd:Connector>
-- 							<ssd:Connector name="u2" kind="input">
-- 								<ssc:Real />
-- 								<ssd:ConnectorGeometry x="0.000000" y="0.666667" />
-- 							</ssd:Connector>
-- 							<ssd:Connector name="y" kind="output">
-- 								<ssc:Real />
-- 								<ssd:ConnectorGeometry x="1.000000" y="0.500000" />
-- 							</ssd:Connector>
-- 							<ssd:Connector name="k1" kind="parameter">
-- 								<ssc:Real />
-- 							</ssd:Connector>
-- 							<ssd:Connector name="k2" kind="parameter">
-- 								<ssc:Real />
-- 							</ssd:Connector>
-- 						</ssd:Connectors>
-- 					</ssd:Component>
-- 				</ssd:Elements>
-- 				<ssd:Annotations>
-- 					<ssc:Annotation type="org.openmodelica">
-- 						<oms:Annotations>
-- 							<oms:SimulationInformation>
-- 								<oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
-- 							</oms:SimulationInformation>
-- 						</oms:Annotations>
-- 					</ssc:Annotation>
-- 				</ssd:Annotations>
-- 			</ssd:System>
-- 			<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
-- 				<ssd:Annotations>
-- 					<ssc:Annotation type="org.openmodelica">
-- 						<oms:Annotations>
-- 							<oms:SimulationInformation resultFile="newsnapshot_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
-- 						</oms:Annotations>
-- 					</ssc:Annotation>
-- 				</ssd:Annotations>
-- 			</ssd:DefaultExperiment>
-- 		</ssd:SystemStructureDescription>
-- 	</oms:ssd_file>
-- 	<oms:ssv_file name="resources/newsnapshot.ssv">
-- 		<ssv:ParameterSet xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" version="1.0" name="parameters">
-- 			<ssv:Parameters>
-- 				<ssv:Parameter name="C1">
-- 					<ssv:Real value="-10" />
-- 				</ssv:Parameter>
-- 				<ssv:Parameter name="add.u1">
-- 					<ssv:Real value="10" />
-- 				</ssv:Parameter>
-- 				<ssv:Parameter name="add.k1">
-- 					<ssv:Real value="30" />
-- 				</ssv:Parameter>
-- 			</ssv:Parameters>
-- 		</ssv:ParameterSet>
-- 	</oms:ssv_file>
-- </oms:snapshot>
--
-- error:   [oms_exportSnapshot] Model "newsnapshot.root.add" does not exist in the scope
-- error:   [exportSnapshot] resource file "resources/snapshot1.ssv" does not exist
-- <?xml version="1.0"?>
-- <oms:ssv_file name="resources/newsnapshot.ssv">
-- 	<ssv:ParameterSet xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" version="1.0" name="parameters">
-- 		<ssv:Parameters>
-- 			<ssv:Parameter name="C1">
-- 				<ssv:Real value="-10" />
-- 			</ssv:Parameter>
-- 			<ssv:Parameter name="add.u1">
-- 				<ssv:Real value="10" />
-- 			</ssv:Parameter>
-- 			<ssv:Parameter name="add.k1">
-- 				<ssv:Real value="30" />
-- 			</ssv:Parameter>
-- 		</ssv:Parameters>
-- 	</ssv:ParameterSet>
-- </oms:ssv_file>
--
-- error:   [importSnapshot] resource file "resources/newsnapshot1.ssv" does not exist
-- info:    0 warnings
-- info:    3 errors
-- endResult
