-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no


oms_setCommandLineOption("--suppressPath=true --exportParametersInline=false")
status = oms_setTempDirectory("./import_export_snapshot/")

oms_newModel("import_export_snapshot")
oms_addSystem("import_export_snapshot.root", oms_system_wc)
oms_addConnector("import_export_snapshot.root.C1", oms_causality_input, oms_signal_type_real)
oms_setReal("import_export_snapshot.root.C1", -10)

oms_addSubModel("import_export_snapshot.root.add", "../resources/Modelica.Blocks.Math.Add.fmu")

oms_setReal("import_export_snapshot.root.add.u1", 10)
oms_setReal("import_export_snapshot.root.add.k1", 30)

-- src1 = oms_list("import_export_snapshot")
-- print(src1)

oms_export("import_export_snapshot", "import_export_snapshot.ssp");
oms_delete("import_export_snapshot")

oms_importFile("import_export_snapshot.ssp");

src1 = oms_list("import_export_snapshot")
print(src1)

src2 = oms_exportSnapshot("import_export_snapshot")
print(src2)

oms_importSnapshot("import_export_snapshot", src2)

-- check of error msg
oms_exportSnapshot("import_export_snapshot.root.add")

oms_setStopTime("import_export_snapshot", 2)

oms_instantiate("import_export_snapshot")

oms_initialize("import_export_snapshot")
oms_simulate("import_export_snapshot")
oms_terminate("import_export_snapshot")
oms_delete("import_export_snapshot")


-- Result:
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="import_export_snapshot" version="1.0">
-- 	<ssd:System name="root">
-- 		<ssd:Connectors>
-- 			<ssd:Connector name="C1" kind="input">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 		</ssd:Connectors>
-- 		<ssd:ParameterBindings>
-- 			<ssd:ParameterBinding source="resources/import_export_snapshot.ssv" />
-- 		</ssd:ParameterBindings>
-- 		<ssd:Elements>
-- 			<ssd:Component name="add" type="application/x-fmu-sharedlibrary" source="resources/0001_add.fmu">
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="u1" kind="input">
-- 						<ssc:Real />
-- 						<ssd:ConnectorGeometry x="0.000000" y="0.333333" />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="u2" kind="input">
-- 						<ssc:Real />
-- 						<ssd:ConnectorGeometry x="0.000000" y="0.666667" />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="y" kind="output">
-- 						<ssc:Real />
-- 						<ssd:ConnectorGeometry x="1.000000" y="0.500000" />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="k1" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="k2" kind="parameter">
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
-- 					<oms:SimulationInformation resultFile="import_export_snapshot_res.mat" loggingInterval="0.000000" bufferSize="10" />
-- 				</oms:Annotations>
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:DefaultExperiment>
-- </ssd:SystemStructureDescription>
--
-- <?xml version="1.0"?>
-- <oms:snapshot>
-- 	<oms:ssd_file name="SystemStructure.ssd">
-- 		<ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="import_export_snapshot" version="1.0">
-- 			<ssd:System name="root">
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="C1" kind="input">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 				</ssd:Connectors>
-- 				<ssd:ParameterBindings>
-- 					<ssd:ParameterBinding source="resources/import_export_snapshot.ssv" />
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
-- 							<oms:SimulationInformation resultFile="import_export_snapshot_res.mat" loggingInterval="0.000000" bufferSize="10" />
-- 						</oms:Annotations>
-- 					</ssc:Annotation>
-- 				</ssd:Annotations>
-- 			</ssd:DefaultExperiment>
-- 		</ssd:SystemStructureDescription>
-- 	</oms:ssd_file>
-- 	<oms:ssv_file name="resources/import_export_snapshot.ssv">
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
-- 	<oms:signalFilter_file name="resources/signalFilter.xml">
-- 		<oms:SignalFilter version="1.0">
-- 			<oms:Variable name="import_export_snapshot.root.add.u1" type="real" kind="input" />
-- 			<oms:Variable name="import_export_snapshot.root.add.u2" type="real" kind="input" />
-- 			<oms:Variable name="import_export_snapshot.root.add.y" type="real" kind="output" />
-- 			<oms:Variable name="import_export_snapshot.root.add.k1" type="real" kind="parameter" />
-- 			<oms:Variable name="import_export_snapshot.root.add.k2" type="real" kind="parameter" />
-- 		</oms:SignalFilter>
-- 	</oms:signalFilter_file>
-- </oms:snapshot>
--
-- error:   [exportSnapshot] "import_export_snapshot.root.add" is not a top level model
-- info:    Result file: import_export_snapshot_res.mat (bufferSize=10)
-- info:    0 warnings
-- info:    1 errors
-- endResult
