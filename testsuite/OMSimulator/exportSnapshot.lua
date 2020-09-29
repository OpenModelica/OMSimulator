-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no


oms_setCommandLineOption("--suppressPath=true --exportParametersInline=false")
status = oms_setTempDirectory("./exportSnapshot/")

oms_newModel("exportSnapshot")
oms_addSystem("exportSnapshot.root", oms_system_wc)

oms_addSubModel("exportSnapshot.root.add", "../resources/Modelica.Blocks.Math.Add.fmu")

oms_setReal("exportSnapshot.root.add.u1", 10)

-- src1 = oms_list("exportSnapshot") 
-- print(src1)

oms_export("exportSnapshot", "exportSnapshot.ssp");
oms_delete("exportSnapshot")

oms_importFile("exportSnapshot.ssp");

src1 = oms_list("exportSnapshot") 
print(src1)

oms_loadSnapshot("exportSnapshot", src1)

src1 = oms_exportSnapshot("exportSnapshot")  
print(src1)

src1 = oms_exportSnapshot("exportSnapshot.root.add")  
print(src1)

oms_setStopTime("exportSnapshot", 2)
 
oms_instantiate("exportSnapshot")

oms_initialize("exportSnapshot")
oms_terminate("exportSnapshot")
oms_delete("exportSnapshot")


-- Result:
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="exportSnapshot" version="1.0">
-- 	<ssd:System name="root">
-- 		<ssd:Annotations>
-- 			<ssc:Annotation type="org.openmodelica">
-- 				<oms:SimulationInformation>
-- 					<oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
-- 				</oms:SimulationInformation>
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 		<ssd:Connectors />
-- 		<ssd:ParameterBindings>
-- 			<ssd:ParameterBinding source="resources/exportSnapshot.ssv" />
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
-- 		<ssd:Connections />
-- 	</ssd:System>
-- 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
-- 		<ssd:Annotations>
-- 			<ssc:Annotation type="org.openmodelica">
-- 				<oms:SimulationInformation resultFile="exportSnapshot_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter="" />
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:DefaultExperiment>
-- </ssd:SystemStructureDescription>
-- 
-- <?xml version="1.0"?>
-- <oms:snapshot>
-- 	<oms:ssd_file name="SystemStructure.ssd">
-- 		<ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="exportSnapshot" version="1.0">
-- 			<ssd:System name="root">
-- 				<ssd:Annotations>
-- 					<ssc:Annotation type="org.openmodelica">
-- 						<oms:SimulationInformation>
-- 							<oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
-- 						</oms:SimulationInformation>
-- 					</ssc:Annotation>
-- 				</ssd:Annotations>
-- 				<ssd:Connectors />
-- 				<ssd:ParameterBindings>
-- 					<ssd:ParameterBinding source="resources/exportSnapshot.ssv" />
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
-- 				<ssd:Connections />
-- 			</ssd:System>
-- 			<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
-- 				<ssd:Annotations>
-- 					<ssc:Annotation type="org.openmodelica">
-- 						<oms:SimulationInformation resultFile="exportSnapshot_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter="" />
-- 					</ssc:Annotation>
-- 				</ssd:Annotations>
-- 			</ssd:DefaultExperiment>
-- 		</ssd:SystemStructureDescription>
-- 	</oms:ssd_file>
-- 	<oms:ssv_file name="resources/exportSnapshot.ssv" />
-- </oms:snapshot>
-- 
-- error:   [exportSnapshot] "exportSnapshot.root.add" is not a top level model
-- 
-- info:    Result file: exportSnapshot_res.mat (bufferSize=10)
-- info:    0 warnings
-- info:    1 errors
-- endResult
