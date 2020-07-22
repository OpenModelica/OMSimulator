-- status: correct
-- linux: yes
-- mingw: yes
-- win: yes
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./deleteConnector_lua/")

oms_newModel("deleteConnector")

oms_addSystem("deleteConnector.Root", oms_system_wc)
oms_addSystem("deleteConnector.Root.System1", oms_system_sc)
oms_addConnector("deleteConnector.Root.System1.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteConnector.Root.System1.C2", oms_causality_input, oms_signal_type_real)

oms_addSystem("deleteConnector.Root.System2", oms_system_sc)
oms_addConnector("deleteConnector.Root.System2.C3", oms_causality_output, oms_signal_type_real)
oms_addConnector("deleteConnector.Root.System2.C4", oms_causality_output, oms_signal_type_real)

oms_addConnection("deleteConnector.Root.System2.C3", "deleteConnector.Root.System1.C1")
oms_addConnection("deleteConnector.Root.System2.C4", "deleteConnector.Root.System1.C2")

src = oms_list("deleteConnector")
print(src)

-- delete connector "deleteConnector.Root.System2.C3" and associated connection
oms_delete("deleteConnector.Root.System2.C3")

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
-- 		<ssd:Connectors />
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
-- 				<ssd:Elements />
-- 				<ssd:Connections />
-- 			</ssd:System>
-- 		</ssd:Elements>
-- 		<ssd:Connections>
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
-- 		<ssd:Connectors />
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
-- 				<ssd:Elements />
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
-- endResult
