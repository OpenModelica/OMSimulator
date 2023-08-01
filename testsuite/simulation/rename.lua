-- status: correct
-- teardown_command: rm -rf rename_lua/
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./rename_lua/")
oms_setWorkingDirectory("./rename_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)
oms_addConnector("model.root.input1", oms_causality_input, oms_signal_type_real)
oms_addConnector("model.root.output", oms_causality_output, oms_signal_type_real)

oms_setReal("model.root.input1", 10)

oms_addSystem("model.root.System1", oms_system_sc)
oms_addConnector("model.root.System1.input1", oms_causality_input, oms_signal_type_real)
oms_addConnector("model.root.System1.output", oms_causality_output, oms_signal_type_real)
-- oms_setReal("model.root.System1.input1", 20)

oms_addSubModel("model.root.add", "../../resources/Modelica.Blocks.Math.Add.fmu")

oms_addConnection("model.root.input1", "model.root.System1.input1")
oms_addConnection("model.root.input1", "model.root.add.u1")
-- oms_addConnection("model.root.input1", "model.root.add.u2")
oms_addConnection("model.root.System1.output", "model.root.add.u2")
oms_addConnection("model.root.add.y", "model.root.output")


src = oms_list("model")
print(src)

status = oms_rename("model.root", "root_1")
oms_addConnection("model.root_1.add.y", "model.root.output")

-- error as model.root is changed to root_1
status = oms_rename("model.root", "root_2")

-- error as model.root is changed to root_1
status = oms_rename("model.root.System1", "System_1")
-- TODO allow new connections after rename wth new connectors
oms_addConnection("model.root_1.System_1.output", "model.root_1.add.u2")

-- correct
status = oms_rename("model.root_1.System1", "System_1")

-- error as model.root_1.System1 is changed to System_1
status = oms_rename("model.root_1.System1", "System_2")

-- correct
status = oms_rename("model.root_1.System_1", "System_2")

-- correct
status = oms_rename("model.root_1.add", "add_1")

-- error as model_root_1.add is changed to add_1
status = oms_rename("model.root_1.add", "add_2")

oms_instantiate("model")

oms_initialize("model")

src = oms_list("model")
print(src)


print("info:      Parameter settings After Rename")
print("info:      model.root_1.input1             : " .. oms_getReal("model.root_1.input1"))
print("info:      model.root_1.System_2.input1    : " .. oms_getReal("model.root_1.System_2.input1"))
print("info:      model.root_1.add_1.u1           : " .. oms_getReal("model.root_1.add_1.u1"))
print("info:      model.root_1.add_1.u2           : " .. oms_getReal("model.root_1.add_1.u2"))


-- Result:
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="model" version="1.0">
-- 	<ssd:System name="root">
-- 		<ssd:Connectors>
-- 			<ssd:Connector name="input1" kind="input">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 			<ssd:Connector name="output" kind="output">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 		</ssd:Connectors>
-- 		<ssd:ParameterBindings>
-- 			<ssd:ParameterBinding>
-- 				<ssd:ParameterValues>
-- 					<ssv:ParameterSet version="1.0" name="parameters">
-- 						<ssv:Parameters>
-- 							<ssv:Parameter name="input1">
-- 								<ssv:Real value="10" />
-- 							</ssv:Parameter>
-- 						</ssv:Parameters>
-- 					</ssv:ParameterSet>
-- 				</ssd:ParameterValues>
-- 			</ssd:ParameterBinding>
-- 		</ssd:ParameterBindings>
-- 		<ssd:Elements>
-- 			<ssd:System name="System1">
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="input1" kind="input">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="output" kind="output">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 				</ssd:Connectors>
-- 				<ssd:Annotations>
-- 					<ssc:Annotation type="org.openmodelica">
-- 						<oms:Annotations>
-- 							<oms:SimulationInformation>
-- 								<oms:VariableStepSolver description="cvode" absoluteTolerance="0.0001" relativeTolerance="0.0001" minimumStepSize="1e-12" maximumStepSize="0.001" initialStepSize="1e-06" />
-- 							</oms:SimulationInformation>
-- 						</oms:Annotations>
-- 					</ssc:Annotation>
-- 				</ssd:Annotations>
-- 			</ssd:System>
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
-- 		<ssd:Connections>
-- 			<ssd:Connection startElement="" startConnector="input1" endElement="System1" endConnector="input1" />
-- 			<ssd:Connection startElement="" startConnector="input1" endElement="add" endConnector="u1" />
-- 			<ssd:Connection startElement="System1" startConnector="output" endElement="add" endConnector="u2" />
-- 			<ssd:Connection startElement="add" startConnector="y" endElement="" endConnector="output" />
-- 		</ssd:Connections>
-- 		<ssd:Annotations>
-- 			<ssc:Annotation type="org.openmodelica">
-- 				<oms:Annotations>
-- 					<oms:SimulationInformation>
-- 						<oms:FixedStepMaster description="oms-ma" stepSize="0.001000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
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
-- error:   [addConnection] Connection <"add.y", "output"> exists already in system "model.root_1"
-- error:   [rename] Model "model" does not contain system "root"
-- error:   [rename] Model "model" does not contain system "root"
-- error:   [addConnection] Connector "System_1.output" not found in system "model.root_1"
-- error:   [rename] failed for "model.root_1.System1" as the identifier could not be resolved to a system or subsystem or component
-- error:   [rename] failed for "model.root_1.add" as the identifier could not be resolved to a system or subsystem or component
-- info:    model doesn't contain any continuous state
-- info:    Result file: model_res.mat (bufferSize=10)
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="model" version="1.0">
-- 	<ssd:System name="root_1">
-- 		<ssd:Connectors>
-- 			<ssd:Connector name="input1" kind="input">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 			<ssd:Connector name="output" kind="output">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 		</ssd:Connectors>
-- 		<ssd:ParameterBindings>
-- 			<ssd:ParameterBinding>
-- 				<ssd:ParameterValues>
-- 					<ssv:ParameterSet version="1.0" name="parameters">
-- 						<ssv:Parameters>
-- 							<ssv:Parameter name="output">
-- 								<ssv:Real value="10" />
-- 							</ssv:Parameter>
-- 							<ssv:Parameter name="input1">
-- 								<ssv:Real value="10" />
-- 							</ssv:Parameter>
-- 						</ssv:Parameters>
-- 					</ssv:ParameterSet>
-- 				</ssd:ParameterValues>
-- 			</ssd:ParameterBinding>
-- 		</ssd:ParameterBindings>
-- 		<ssd:Elements>
-- 			<ssd:System name="System_2">
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="input1" kind="input">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="output" kind="output">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 				</ssd:Connectors>
-- 				<ssd:ParameterBindings>
-- 					<ssd:ParameterBinding>
-- 						<ssd:ParameterValues>
-- 							<ssv:ParameterSet version="1.0" name="parameters">
-- 								<ssv:Parameters>
-- 									<ssv:Parameter name="input1">
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
-- 								<oms:VariableStepSolver description="euler" absoluteTolerance="0.0001" relativeTolerance="0.0001" minimumStepSize="1e-12" maximumStepSize="0.001" initialStepSize="1e-06" />
-- 							</oms:SimulationInformation>
-- 						</oms:Annotations>
-- 					</ssc:Annotation>
-- 				</ssd:Annotations>
-- 			</ssd:System>
-- 			<ssd:Component name="add_1" type="application/x-fmu-sharedlibrary" source="resources/0001_add.fmu">
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
-- 		<ssd:Connections>
-- 			<ssd:Connection startElement="" startConnector="input1" endElement="System_2" endConnector="input1" />
-- 			<ssd:Connection startElement="" startConnector="input1" endElement="add_1" endConnector="u1" />
-- 			<ssd:Connection startElement="System_2" startConnector="output" endElement="add_1" endConnector="u2" />
-- 			<ssd:Connection startElement="add_1" startConnector="y" endElement="" endConnector="output" />
-- 		</ssd:Connections>
-- 		<ssd:Annotations>
-- 			<ssc:Annotation type="org.openmodelica">
-- 				<oms:Annotations>
-- 					<oms:SimulationInformation>
-- 						<oms:FixedStepMaster description="oms-ma" stepSize="0.001000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
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
-- info:      Parameter settings After Rename
-- info:      model.root_1.input1             : 10.0
-- info:      model.root_1.System_2.input1    : 10.0
-- info:      model.root_1.add_1.u1           : 10.0
-- info:      model.root_1.add_1.u2           : 0.0
-- info:    0 warnings
-- info:    6 errors
-- endResult
