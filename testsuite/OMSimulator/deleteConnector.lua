-- status: correct
-- linux: yes
-- mingw: yes
-- win: yes
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./deleteConnector_lua/")

oms_newModel("deleteConnector")

oms_addSystem("deleteConnector.Root", oms_system_wc)
oms_addConnector("deleteConnector.Root.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteConnector.Root.C2", oms_causality_output, oms_signal_type_real)
oms_addConnection("deleteConnector.Root.C1", "deleteConnector.Root.C2")

oms_addSystem("deleteConnector.Root.System1", oms_system_sc)
oms_addConnector("deleteConnector.Root.System1.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteConnector.Root.System1.C2", oms_causality_input, oms_signal_type_real)

oms_addSystem("deleteConnector.Root.System2", oms_system_sc)
oms_addConnector("deleteConnector.Root.System2.C3", oms_causality_output, oms_signal_type_real)
oms_addConnector("deleteConnector.Root.System2.C4", oms_causality_output, oms_signal_type_real)
oms_setReal("deleteConnector.Root.System2.C3", 20.0)
oms_setReal("deleteConnector.Root.System2.C4", 30.0)

oms_addSubModel("deleteConnector.Root.System1.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms_setReal("deleteConnector.Root.System1.Gain.k", 30.0)

oms_addConnection("deleteConnector.Root.System2.C3", "deleteConnector.Root.System1.C1")
oms_addConnection("deleteConnector.Root.System2.C4", "deleteConnector.Root.System1.C2")

src = oms_list("deleteConnector")
print(src)

oms_delete("deleteConnector.Root.C1")

-- delete connector "deleteConnector.Root.System2.C3" and associated connection
oms_delete("deleteConnector.Root.System2.C3")

-- delete components connector and its startValue
oms_delete("deleteConnector.Root.System1.Gain.k")

-- delete unknown connector signal for warning
oms_delete("deleteConnector.Root.System2.C6")

-- delete unknown system for warning
oms_delete("deleteConnector.Root.System3")

-- after deletion
src = oms_list("deleteConnector")
print(src)


-- Result:
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription name="deleteConnector" version="1.0">
-- 	<ssd:System name="Root">
-- 		<ssd:Annotations>
-- 			<ssd:Annotation type="org.openmodelica">
-- 				<oms:SimulationInformation>
-- 					<oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
-- 				</oms:SimulationInformation>
-- 			</ssd:Annotation>
-- 		</ssd:Annotations>
-- 		<ssd:Connectors>
-- 			<ssd:Connector name="C1" kind="input">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 			<ssd:Connector name="C2" kind="output">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 		</ssd:Connectors>
-- 		<ssd:Elements>
-- 			<ssd:System name="System2">
-- 				<ssd:Annotations>
-- 					<ssd:Annotation type="org.openmodelica">
-- 						<oms:SimulationInformation>
-- 							<oms:VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
-- 						</oms:SimulationInformation>
-- 					</ssd:Annotation>
-- 				</ssd:Annotations>
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
-- 				<ssd:Elements />
-- 				<ssd:Connections />
-- 			</ssd:System>
-- 			<ssd:System name="System1">
-- 				<ssd:Annotations>
-- 					<ssd:Annotation type="org.openmodelica">
-- 						<oms:SimulationInformation>
-- 							<oms:VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
-- 						</oms:SimulationInformation>
-- 					</ssd:Annotation>
-- 				</ssd:Annotations>
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
-- 						<ssd:ParameterBindings>
-- 							<ssd:ParameterBinding>
-- 								<ssd:ParameterValues>
-- 									<ssv:ParameterSet version="1.0" name="parameters">
-- 										<ssv:Parameters>
-- 											<ssv:Parameter name="k">
-- 												<ssv:Real value="30" />
-- 											</ssv:Parameter>
-- 										</ssv:Parameters>
-- 									</ssv:ParameterSet>
-- 								</ssd:ParameterValues>
-- 							</ssd:ParameterBinding>
-- 						</ssd:ParameterBindings>
-- 					</ssd:Component>
-- 				</ssd:Elements>
-- 				<ssd:Connections />
-- 			</ssd:System>
-- 		</ssd:Elements>
-- 		<ssd:Connections>
-- 			<ssd:Connection startElement="" startConnector="C1" endElement="" endConnector="C2" />
-- 			<ssd:Connection startElement="System2" startConnector="C3" endElement="System1" endConnector="C1" />
-- 			<ssd:Connection startElement="System2" startConnector="C4" endElement="System1" endConnector="C2" />
-- 		</ssd:Connections>
-- 	</ssd:System>
-- 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
-- 		<ssd:Annotations>
-- 			<ssd:Annotation type="org.openmodelica">
-- 				<oms:SimulationInformation resultFile="deleteConnector_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter="" />
-- 			</ssd:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:DefaultExperiment>
-- </ssd:SystemStructureDescription>
-- 
-- warning: failed to delete object "deleteConnector.Root.System2.C6" because the identifier couldn't be resolved to any connector, component, system, or model
-- warning: failed to delete object "deleteConnector.Root.System3" because the identifier couldn't be resolved to any connector, component, system, or model
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription name="deleteConnector" version="1.0">
-- 	<ssd:System name="Root">
-- 		<ssd:Annotations>
-- 			<ssd:Annotation type="org.openmodelica">
-- 				<oms:SimulationInformation>
-- 					<oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
-- 				</oms:SimulationInformation>
-- 			</ssd:Annotation>
-- 		</ssd:Annotations>
-- 		<ssd:Connectors>
-- 			<ssd:Connector name="C2" kind="output">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 		</ssd:Connectors>
-- 		<ssd:Elements>
-- 			<ssd:System name="System2">
-- 				<ssd:Annotations>
-- 					<ssd:Annotation type="org.openmodelica">
-- 						<oms:SimulationInformation>
-- 							<oms:VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
-- 						</oms:SimulationInformation>
-- 					</ssd:Annotation>
-- 				</ssd:Annotations>
-- 				<ssd:Connectors>
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
-- 								</ssv:Parameters>
-- 							</ssv:ParameterSet>
-- 						</ssd:ParameterValues>
-- 					</ssd:ParameterBinding>
-- 				</ssd:ParameterBindings>
-- 				<ssd:Elements />
-- 				<ssd:Connections />
-- 			</ssd:System>
-- 			<ssd:System name="System1">
-- 				<ssd:Annotations>
-- 					<ssd:Annotation type="org.openmodelica">
-- 						<oms:SimulationInformation>
-- 							<oms:VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
-- 						</oms:SimulationInformation>
-- 					</ssd:Annotation>
-- 				</ssd:Annotations>
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
-- 						</ssd:Connectors>
-- 					</ssd:Component>
-- 				</ssd:Elements>
-- 				<ssd:Connections />
-- 			</ssd:System>
-- 		</ssd:Elements>
-- 		<ssd:Connections>
-- 			<ssd:Connection startElement="System2" startConnector="C4" endElement="System1" endConnector="C2" />
-- 		</ssd:Connections>
-- 	</ssd:System>
-- 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
-- 		<ssd:Annotations>
-- 			<ssd:Annotation type="org.openmodelica">
-- 				<oms:SimulationInformation resultFile="deleteConnector_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter="" />
-- 			</ssd:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:DefaultExperiment>
-- </ssd:SystemStructureDescription>
-- 
-- info:    2 warnings
-- info:    0 errors
-- endResult
