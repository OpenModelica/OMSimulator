-- status: correct
-- teardown_command: rm -rf signalFilter-lua/
-- linux: yes
-- linux32: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./signalFilter-lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)

oms_addSystem("model.root.System1", oms_system_sc)

oms_addConnector("model.root.System1.Input_1", oms_causality_input, oms_signal_type_real)
oms_addConnector("model.root.System1.parameter_1", oms_causality_parameter, oms_signal_type_real)

oms_setReal("model.root.System1.Input_1", 10)
oms_setReal("model.root.System1.parameter_1", 20)

oms_addSystem("model.root.System2", oms_system_sc)

oms_addConnector("model.root.System2.Input_1", oms_causality_input, oms_signal_type_real)
oms_addConnector("model.root.System2.parameter_1", oms_causality_parameter, oms_signal_type_real)

oms_setReal("model.root.System2.Input_1", 30)
oms_setReal("model.root.System2.parameter_1", 40)

oms_addSubModel("model.root.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")

oms_addSignalsToResults("model", "model.root.System2.*")
oms_addSignalsToResults("model", "model.root.Gain.*")

oms_removeSignalsFromResults("model", "model.root.System2.parameter_1")
oms_removeSignalsFromResults("model", "model.root.Gain.y")

oms_instantiate("model")

oms_initialize("model")

oms_simulate("model")

oms_export("model", "signalFilter.ssp")

src, status = oms_list("model")
print(src)

oms_terminate("model")
oms_delete("model")

-- Result:
-- info:    model doesn't contain any continuous state
-- info:    model doesn't contain any continuous state
-- info:    Result file: model_res.mat (bufferSize=10)
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="model" version="1.0">
-- 	<ssd:System name="root">
-- 		<ssd:Elements>
-- 			<ssd:System name="System2">
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="Input_1" kind="input">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="parameter_1" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 				</ssd:Connectors>
-- 				<ssd:ParameterBindings>
-- 					<ssd:ParameterBinding>
-- 						<ssd:ParameterValues>
-- 							<ssv:ParameterSet version="1.0" name="parameters">
-- 								<ssv:Parameters>
-- 									<ssv:Parameter name="parameter_1">
-- 										<ssv:Real value="40" />
-- 									</ssv:Parameter>
-- 									<ssv:Parameter name="Input_1">
-- 										<ssv:Real value="30" />
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
-- 								<oms:VariableStepSolver description="euler" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
-- 							</oms:SimulationInformation>
-- 						</oms:Annotations>
-- 					</ssc:Annotation>
-- 				</ssd:Annotations>
-- 			</ssd:System>
-- 			<ssd:System name="System1">
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="Input_1" kind="input">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="parameter_1" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 				</ssd:Connectors>
-- 				<ssd:ParameterBindings>
-- 					<ssd:ParameterBinding>
-- 						<ssd:ParameterValues>
-- 							<ssv:ParameterSet version="1.0" name="parameters">
-- 								<ssv:Parameters>
-- 									<ssv:Parameter name="parameter_1">
-- 										<ssv:Real value="20" />
-- 									</ssv:Parameter>
-- 									<ssv:Parameter name="Input_1">
-- 										<ssv:Real value="10" />
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
-- 								<oms:VariableStepSolver description="euler" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
-- 							</oms:SimulationInformation>
-- 						</oms:Annotations>
-- 					</ssc:Annotation>
-- 				</ssd:Annotations>
-- 			</ssd:System>
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
-- 					<oms:SimulationInformation resultFile="model_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter="resources/signalFilter.xml" />
-- 				</oms:Annotations>
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:DefaultExperiment>
-- </ssd:SystemStructureDescription>
--
-- endResult
