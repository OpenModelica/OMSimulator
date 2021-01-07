-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./import_parameter_mapping_inline_lua/")

oms_importFile("../resources/importParameterMappingInline.ssp");

src, status = oms_list("import_parameter_mapping")
print(src)

oms_instantiate("import_parameter_mapping")

print("info:      Instantiation")
print("info:      import_parameter_mapping.co_sim.Input_1              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_1"))
print("info:      import_parameter_mapping.co_sim.Input_2              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_2"))
print("info:      import_parameter_mapping.co_sim.Input_3              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_3"))
print("info:      import_parameter_mapping.co_sim.parameter_1          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_1"))
print("info:      import_parameter_mapping.co_sim.parameter_2          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System1.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_1"))
print("info:      import_parameter_mapping.co_sim.System1.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_2"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System2.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_1"))
print("info:      import_parameter_mapping.co_sim.System2.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_2"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_2"))


oms_initialize("import_parameter_mapping")
print("info:    Initialization")
print("info:      import_parameter_mapping.co_sim.Input_1              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_1"))
print("info:      import_parameter_mapping.co_sim.Input_2              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_2"))
print("info:      import_parameter_mapping.co_sim.Input_3              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_3"))
print("info:      import_parameter_mapping.co_sim.parameter_1          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_1"))
print("info:      import_parameter_mapping.co_sim.parameter_2          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System1.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_1"))
print("info:      import_parameter_mapping.co_sim.System1.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_2"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System2.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_1"))
print("info:      import_parameter_mapping.co_sim.System2.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_2"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_2"))


oms_simulate("import_parameter_mapping")
print("info:    Simulation")
print("info:      import_parameter_mapping.co_sim.Input_1              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_1"))
print("info:      import_parameter_mapping.co_sim.Input_2              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_2"))
print("info:      import_parameter_mapping.co_sim.Input_3              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_3"))
print("info:      import_parameter_mapping.co_sim.parameter_1          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_1"))
print("info:      import_parameter_mapping.co_sim.parameter_2          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System1.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_1"))
print("info:      import_parameter_mapping.co_sim.System1.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_2"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System2.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_1"))
print("info:      import_parameter_mapping.co_sim.System2.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_2"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_2"))

-- Result:
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="import_parameter_mapping" version="1.0">
-- 	<ssd:System name="co_sim">
-- 		<ssd:Connectors>
-- 			<ssd:Connector name="Input_1" kind="input">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 			<ssd:Connector name="Input_2" kind="input">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 			<ssd:Connector name="Input_3" kind="input">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 			<ssd:Connector name="Output_cref" kind="output">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 			<ssd:Connector name="parameter_1" kind="parameter">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 			<ssd:Connector name="parameter_2" kind="parameter">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 		</ssd:Connectors>
-- 		<ssd:ParameterBindings>
-- 			<ssd:ParameterBinding>
-- 				<ssd:ParameterValues>
-- 					<ssv:ParameterSet version="1.0" name="parameters">
-- 						<ssv:Parameters>
-- 							<ssv:Parameter name="parameter_2">
-- 								<ssv:Real value="-40" />
-- 							</ssv:Parameter>
-- 							<ssv:Parameter name="cosim_parameters">
-- 								<ssv:Real value="-30" />
-- 							</ssv:Parameter>
-- 							<ssv:Parameter name="Input_3">
-- 								<ssv:Real value="50" />
-- 							</ssv:Parameter>
-- 							<ssv:Parameter name="cosim_inputs">
-- 								<ssv:Real value="20" />
-- 							</ssv:Parameter>
-- 						</ssv:Parameters>
-- 					</ssv:ParameterSet>
-- 				</ssd:ParameterValues>
-- 				<ssd:ParameterMapping>
-- 					<ssm:ParameterMapping>
-- 						<ssm:MappingEntry source="cosim_parameters" target="parameter_1" />
-- 						<ssm:MappingEntry source="cosim_inputs" target="Input_1" />
-- 						<ssm:MappingEntry source="cosim_inputs" target="Input_2" />
-- 					</ssm:ParameterMapping>
-- 				</ssd:ParameterMapping>
-- 			</ssd:ParameterBinding>
-- 		</ssd:ParameterBindings>
-- 		<ssd:Elements>
-- 			<ssd:System name="System2">
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="Input_1" kind="input">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="Input_2" kind="input">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="parameter_1" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="parameter_2" kind="parameter">
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
-- 									<ssv:Parameter name="system2_inputs">
-- 										<ssv:Real value="70" />
-- 									</ssv:Parameter>
-- 								</ssv:Parameters>
-- 							</ssv:ParameterSet>
-- 						</ssd:ParameterValues>
-- 						<ssd:ParameterMapping>
-- 							<ssm:ParameterMapping>
-- 								<ssm:MappingEntry source="system2_inputs" target="Input_1" />
-- 								<ssm:MappingEntry source="system2_inputs" target="Input_2" />
-- 								<ssm:MappingEntry source="system2_inputs" target="parameter_1" />
-- 								<ssm:MappingEntry source="system2_inputs" target="parameter_2" />
-- 							</ssm:ParameterMapping>
-- 						</ssd:ParameterMapping>
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
-- 					<ssd:Connector name="Input_1" kind="input">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="Input_2" kind="input">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="parameter_1" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="parameter_2" kind="parameter">
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
-- 									<ssv:Parameter name="system1_parameters">
-- 										<ssv:Real value="-50" />
-- 									</ssv:Parameter>
-- 									<ssv:Parameter name="system1_inputs">
-- 										<ssv:Real value="-100" />
-- 									</ssv:Parameter>
-- 								</ssv:Parameters>
-- 							</ssv:ParameterSet>
-- 						</ssd:ParameterValues>
-- 						<ssd:ParameterMapping>
-- 							<ssm:ParameterMapping>
-- 								<ssm:MappingEntry source="system1_parameters" target="parameter_1" />
-- 								<ssm:MappingEntry source="system1_parameters" target="parameter_2" />
-- 								<ssm:MappingEntry source="system1_inputs" target="Input_1" />
-- 								<ssm:MappingEntry source="system1_inputs" target="Input_2" />
-- 							</ssm:ParameterMapping>
-- 						</ssd:ParameterMapping>
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
-- 					<oms:SimulationInformation resultFile="import_parameter_mapping_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
-- 				</oms:Annotations>
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:DefaultExperiment>
-- </ssd:SystemStructureDescription>
-- 
-- info:    model doesn't contain any continuous state
-- info:    model doesn't contain any continuous state
-- info:      Instantiation
-- info:      import_parameter_mapping.co_sim.Input_1              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_2              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_3              : 50.0
-- info:      import_parameter_mapping.co_sim.parameter_1          : -30.0
-- info:      import_parameter_mapping.co_sim.parameter_2          : -40.0
-- info:      import_parameter_mapping.co_sim.System1.Input_1      : -100.0
-- info:      import_parameter_mapping.co_sim.System1.Input_2      : -100.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_1  : -50.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_2  : -50.0
-- info:      import_parameter_mapping.co_sim.System2.Input_1      : 70.0
-- info:      import_parameter_mapping.co_sim.System2.Input_2      : 70.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_1  : 70.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_2  : 70.0
-- info:    Result file: import_parameter_mapping_res.mat (bufferSize=10)
-- info:    Initialization
-- info:      import_parameter_mapping.co_sim.Input_1              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_2              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_3              : 50.0
-- info:      import_parameter_mapping.co_sim.parameter_1          : -30.0
-- info:      import_parameter_mapping.co_sim.parameter_2          : -40.0
-- info:      import_parameter_mapping.co_sim.System1.Input_1      : -100.0
-- info:      import_parameter_mapping.co_sim.System1.Input_2      : -100.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_1  : -50.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_2  : -50.0
-- info:      import_parameter_mapping.co_sim.System2.Input_1      : 70.0
-- info:      import_parameter_mapping.co_sim.System2.Input_2      : 70.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_1  : 70.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_2  : 70.0
-- info:    Simulation
-- info:      import_parameter_mapping.co_sim.Input_1              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_2              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_3              : 50.0
-- info:      import_parameter_mapping.co_sim.parameter_1          : -30.0
-- info:      import_parameter_mapping.co_sim.parameter_2          : -40.0
-- info:      import_parameter_mapping.co_sim.System1.Input_1      : -100.0
-- info:      import_parameter_mapping.co_sim.System1.Input_2      : -100.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_1  : -50.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_2  : -50.0
-- info:      import_parameter_mapping.co_sim.System2.Input_1      : 70.0
-- info:      import_parameter_mapping.co_sim.System2.Input_2      : 70.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_1  : 70.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_2  : 70.0
-- endResult
