-- status: correct
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: no
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./deleteStartValues_lua/")

oms_newModel("deleteStartValues")

oms_addSystem("deleteStartValues.Root", oms_system_wc)
oms_addConnector("deleteStartValues.Root.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteStartValues.Root.C2", oms_causality_output, oms_signal_type_real)
oms_addConnector("deleteStartValues.Root.C3", oms_causality_parameter, oms_signal_type_real)
oms_setReal("deleteStartValues.Root.C3", 10.0)

oms_addSystem("deleteStartValues.Root.System1", oms_system_sc)
oms_addConnector("deleteStartValues.Root.System1.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteStartValues.Root.System1.C2", oms_causality_input, oms_signal_type_real)

oms_setReal("deleteStartValues.Root.System1.C1", -10.0)
oms_setReal("deleteStartValues.Root.System1.C2", -30.0)

oms_addSystem("deleteStartValues.Root.System2", oms_system_sc)
oms_addConnector("deleteStartValues.Root.System2.C3", oms_causality_output, oms_signal_type_real)
oms_addConnector("deleteStartValues.Root.System2.C4", oms_causality_output, oms_signal_type_real)
oms_setReal("deleteStartValues.Root.System2.C3", 20.0)
oms_setReal("deleteStartValues.Root.System2.C4", 30.0)

oms_addSubModel("deleteStartValues.Root.System1.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")

--oms_setReal("deleteStartValues.Root.System1.Gain.k:start", 30.0) // this is allowed before initialization
--oms_setReal("deleteStartValues.Root.System1.Gain.k", 20.0) // allowed only if model is initialized, we are allowed to provide different value after initialization

src = oms_list("deleteStartValues")
print(src)

oms_delete("deleteStartValues.Root.C3:start")
oms_delete("deleteStartValues.Root.System1.C1:start")
oms_delete("deleteStartValues.Root.System1.C2:start")
oms_delete("deleteStartValues.Root.System2.C3:start")
oms_delete("deleteStartValues.Root.System2.C4:start")

-- delete unknown system startValue for warning
oms_delete("deleteStartValues.Root.System3.C4:start")

-- delete components startValue which was not set
oms_delete("deleteStartValues.Root.System1.Gain.k:start")

src = oms_list("deleteStartValues")
print(src)

-- Result:
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="deleteStartValues" version="1.0">
-- 	<ssd:System name="Root">
-- 		<ssd:Connectors>
-- 			<ssd:Connector name="C1" kind="input">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 			<ssd:Connector name="C2" kind="output">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 			<ssd:Connector name="C3" kind="parameter">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 		</ssd:Connectors>
-- 		<ssd:ParameterBindings>
-- 			<ssd:ParameterBinding>
-- 				<ssd:ParameterValues>
-- 					<ssv:ParameterSet version="1.0" name="parameters">
-- 						<ssv:Parameters>
-- 							<ssv:Parameter name="C3">
-- 								<ssv:Real value="10" />
-- 							</ssv:Parameter>
-- 						</ssv:Parameters>
-- 					</ssv:ParameterSet>
-- 				</ssd:ParameterValues>
-- 			</ssd:ParameterBinding>
-- 		</ssd:ParameterBindings>
-- 		<ssd:Elements>
-- 			<ssd:System name="System2">
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="C3" kind="output">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="C4" kind="output">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 				</ssd:Connectors>
-- 				<ssd:ParameterBindings>
-- 					<ssd:ParameterBinding>
-- 						<ssd:ParameterValues>
-- 							<ssv:ParameterSet version="1.0" name="parameters">
-- 								<ssv:Parameters>
-- 									<ssv:Parameter name="C4">
-- 										<ssv:Real value="30" />
-- 									</ssv:Parameter>
-- 									<ssv:Parameter name="C3">
-- 										<ssv:Real value="20" />
-- 									</ssv:Parameter>
-- 								</ssv:Parameters>
-- 							</ssv:ParameterSet>
-- 						</ssd:ParameterValues>
-- 					</ssd:ParameterBinding>
-- 				</ssd:ParameterBindings>
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
-- 			<ssd:System name="System1">
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="C1" kind="input">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="C2" kind="input">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 				</ssd:Connectors>
-- 				<ssd:ParameterBindings>
-- 					<ssd:ParameterBinding>
-- 						<ssd:ParameterValues>
-- 							<ssv:ParameterSet version="1.0" name="parameters">
-- 								<ssv:Parameters>
-- 									<ssv:Parameter name="C2">
-- 										<ssv:Real value="-30" />
-- 									</ssv:Parameter>
-- 									<ssv:Parameter name="C1">
-- 										<ssv:Real value="-10" />
-- 									</ssv:Parameter>
-- 								</ssv:Parameters>
-- 							</ssv:ParameterSet>
-- 						</ssd:ParameterValues>
-- 					</ssd:ParameterBinding>
-- 				</ssd:ParameterBindings>
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
-- 					<oms:SimulationInformation resultFile="deleteStartValues_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter="resources/signalFilter.xml" />
-- 				</oms:Annotations>
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:DefaultExperiment>
-- </ssd:SystemStructureDescription>
--
-- warning: failed to delete object "deleteStartValues.Root.System3.C4:start" because the identifier couldn't be resolved to any connector, component, system, or model
-- warning: failed to delete start value "deleteStartValues.Root.System1.Gain.k:start" because the identifier couldn't be resolved to any component signal
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="deleteStartValues" version="1.0">
-- 	<ssd:System name="Root">
-- 		<ssd:Connectors>
-- 			<ssd:Connector name="C1" kind="input">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 			<ssd:Connector name="C2" kind="output">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 			<ssd:Connector name="C3" kind="parameter">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 		</ssd:Connectors>
-- 		<ssd:Elements>
-- 			<ssd:System name="System2">
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="C3" kind="output">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="C4" kind="output">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 				</ssd:Connectors>
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
-- 					<oms:SimulationInformation resultFile="deleteStartValues_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter="resources/signalFilter.xml" />
-- 				</oms:Annotations>
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:DefaultExperiment>
-- </ssd:SystemStructureDescription>
--
-- info:    2 warnings
-- info:    0 errors
-- endResult
