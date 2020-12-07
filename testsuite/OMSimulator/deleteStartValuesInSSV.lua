-- status: correct
-- linux: yes
-- mingw: no
-- win: no
-- mac: yes

oms_setCommandLineOption("--suppressPath=true --exportParametersInline=false")
oms_setCommandLineOption("--suppressPath=true")

oms_setTempDirectory("./deleteStartValuesInSSV_lua/")

oms_newModel("deleteStartValuesInSSV")

oms_addSystem("deleteStartValuesInSSV.Root", oms_system_wc)

oms_addConnector("deleteStartValuesInSSV.Root.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteStartValuesInSSV.Root.C2", oms_causality_output, oms_signal_type_real)
oms_setReal("deleteStartValuesInSSV.Root.C1", 10.0)
oms_setReal("deleteStartValuesInSSV.Root.C2", 15.0)

-- add subSystem
oms_addSystem("deleteStartValuesInSSV.Root.System1", oms_system_sc)
oms_addConnector("deleteStartValuesInSSV.Root.System1.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteStartValuesInSSV.Root.System1.C2", oms_causality_input, oms_signal_type_real)

oms_setReal("deleteStartValuesInSSV.Root.System1.C1", 20.0)
oms_setReal("deleteStartValuesInSSV.Root.System1.C2", 30.0)

-- add submodel
oms_addSubModel("deleteStartValuesInSSV.Root.System1.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms_setReal("deleteStartValuesInSSV.Root.System1.Gain.k", 40.0)

src = oms_list("deleteStartValuesInSSV")
print(src)

-- delete top level system start value
oms_delete("deleteStartValuesInSSV.Root.C1:start")

-- delete subsystem start value
oms_delete("deleteStartValuesInSSV.Root.System1.C1:start")

-- delete unknown top level start value for warning
oms_delete("deleteStartValuesInSSV.Root.C3:start")

-- delete unknown subsystem start value for warning
oms_delete("deleteStartValuesInSSV.Root.System1.C4:start")

-- delete unknown component start values for warning
oms_delete("deleteStartValuesInSSV.Root.System1.Gain.j:start")

oms_export("deleteStartValuesInSSV", "deleteStartValuesInSSV.ssp");

oms_delete("deleteStartValuesInSSV")



-- Result:
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="deleteStartValuesInSSV" version="1.0">
-- 	<ssd:System name="Root">
-- 		<ssd:Connectors>
-- 			<ssd:Connector name="C1" kind="input">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 			<ssd:Connector name="C2" kind="output">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 		</ssd:Connectors>
-- 		<ssd:ParameterBindings>
-- 			<ssd:ParameterBinding source="resources/deleteStartValuesInSSV.ssv" />
-- 		</ssd:ParameterBindings>
-- 		<ssd:Elements>
-- 			<ssd:System name="System1">
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="C1" kind="input">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="C2" kind="input">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 				</ssd:Connectors>
-- 				<ssd:Elements>
-- 					<ssd:Component name="Gain" type="application/x-fmu-sharedlibrary" source="resources/0001_Gain.fmu">
-- 						<ssd:Connectors>
-- 							<ssd:Connector name="u" kind="input">
-- 								<ssc:Real />
-- 								<ssd:ConnectorGeometry x="0.000000" y="0.500000" />
-- 							</ssd:Connector>
-- 							<ssd:Connector name="y" kind="output">
-- 								<ssc:Real />
-- 								<ssd:ConnectorGeometry x="1.000000" y="0.500000" />
-- 							</ssd:Connector>
-- 							<ssd:Connector name="k" kind="parameter">
-- 								<ssc:Real />
-- 							</ssd:Connector>
-- 						</ssd:Connectors>
-- 					</ssd:Component>
-- 				</ssd:Elements>
-- 				<ssd:Annotations>
-- 					<ssc:Annotation type="org.openmodelica">
-- 						<oms:Annotations>
-- 							<oms:SimulationInformation>
-- 								<oms:VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
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
-- 					<oms:SimulationInformation resultFile="deleteStartValuesInSSV_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
-- 				</oms:Annotations>
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:DefaultExperiment>
-- </ssd:SystemStructureDescription>
--
-- warning: failed to delete start value "deleteStartValuesInSSV.Root.C3:start" because the identifier couldn't be resolved to any system signal
-- warning: failed to delete start value "deleteStartValuesInSSV.Root.System1.C4:start" because the identifier couldn't be resolved to any system signal
-- warning: failed to delete start value "deleteStartValuesInSSV.Root.System1.Gain.j:start" because the identifier couldn't be resolved to any component signal
-- info:    3 warnings
-- info:    0 errors
-- endResult
