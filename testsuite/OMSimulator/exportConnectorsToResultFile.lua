-- status: correct
-- linux: yes
-- mingw: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./exportConnectors/")

oms_newModel("exportConnectors")

oms_addSystem("exportConnectors.Root", oms_system_wc)
oms_addConnector("exportConnectors.Root.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("exportConnectors.Root.C2", oms_causality_output, oms_signal_type_real)
oms_addConnector("exportConnectors.Root.C3", oms_causality_parameter, oms_signal_type_real)


-- oms_addSystem("exportConnectors.Root.System1", oms_system_sc)
-- oms_addConnector("exportConnectors.Root.System1.T1", oms_causality_input, oms_signal_type_real)

oms_addSubModel("exportConnectors.Root.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")

print("info:  Before Setting Start Values")
print("info:      exportConnectors.Root.C1                    : " .. oms_getReal("exportConnectors.Root.C1"))
print("info:      exportConnectors.Root.C3                    : " .. oms_getReal("exportConnectors.Root.C3"))
print("info:      exportConnectors.Root.Gain.u                : " .. oms_getReal("exportConnectors.Root.Gain.u"))
print("info:      exportConnectors.Root.Gain.k                : " .. oms_getReal("exportConnectors.Root.Gain.k"))


oms_setReal("exportConnectors.Root.C1", 2.5)
oms_setReal("exportConnectors.Root.C3", 4.5)
oms_setReal("exportConnectors.Root.Gain.u", 7.5)

print("info:  After Setting Start Values")
print("info:      exportConnectors.Root.C1                    : " .. oms_getReal("exportConnectors.Root.C1"))
print("info:      exportConnectors.Root.C3                    : " .. oms_getReal("exportConnectors.Root.C3"))
print("info:      exportConnectors.Root.Gain.u                : " .. oms_getReal("exportConnectors.Root.Gain.u"))

oms_instantiate("exportConnectors")
print("info:  After instantiation")
print("info:      exportConnectors.Root.C1                    : " .. oms_getReal("exportConnectors.Root.C1"))
print("info:      exportConnectors.Root.C3                    : " .. oms_getReal("exportConnectors.Root.C3"))
print("info:      exportConnectors.Root.Gain.u                : " .. oms_getReal("exportConnectors.Root.Gain.u"))

oms_initialize("exportConnectors")
print("info:  Initialization")
print("info:      exportConnectors.Root.C1                    : " .. oms_getReal("exportConnectors.Root.C1"))
print("info:      exportConnectors.Root.C3                    : " .. oms_getReal("exportConnectors.Root.C3"))
print("info:      exportConnectors.Root.Gain.u                : " .. oms_getReal("exportConnectors.Root.Gain.u"))

-- set external inputs
oms_setReal("exportConnectors.Root.C1", -12.5)
oms_setReal("exportConnectors.Root.Gain.u", 10.5)

oms_simulate("exportConnectors")
print("info:  Simulation")
print("info:      exportConnectors.Root.C1                    : " .. oms_getReal("exportConnectors.Root.C1"))
print("info:      exportConnectors.Root.C3                    : " .. oms_getReal("exportConnectors.Root.C3"))
print("info:      exportConnectors.Root.Gain.u                : " .. oms_getReal("exportConnectors.Root.Gain.u"))

src = oms_list("exportConnectors")
print(src)

oms_terminate("exportConnectors")
oms_delete("exportConnectors")

-- Result:
-- info:  Before Setting Start Values
-- info:      exportConnectors.Root.C1                    : 0.0
-- info:      exportConnectors.Root.C3                    : 0.0
-- info:      exportConnectors.Root.Gain.u                : 0.0
-- info:      exportConnectors.Root.Gain.k                : 1.0
-- info:  After Setting Start Values
-- info:      exportConnectors.Root.C1                    : 2.5
-- info:      exportConnectors.Root.C3                    : 4.5
-- info:      exportConnectors.Root.Gain.u                : 7.5
-- info:  After instantiation
-- info:      exportConnectors.Root.C1                    : 2.5
-- info:      exportConnectors.Root.C3                    : 4.5
-- info:      exportConnectors.Root.Gain.u                : 7.5
-- info:    Result file: exportConnectors_res.mat (bufferSize=10)
-- info:  Initialization
-- info:      exportConnectors.Root.C1                    : 2.5
-- info:      exportConnectors.Root.C3                    : 4.5
-- info:      exportConnectors.Root.Gain.u                : 7.5
-- info:  Simulation
-- info:      exportConnectors.Root.C1                    : -12.5
-- info:      exportConnectors.Root.C3                    : 4.5
-- info:      exportConnectors.Root.Gain.u                : 10.5
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="exportConnectors" version="1.0">
-- 	<ssd:System name="Root">
-- 		<ssd:Annotations>
-- 			<ssc:Annotation type="org.openmodelica">
-- 				<oms:SimulationInformation>
-- 					<oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
-- 				</oms:SimulationInformation>
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
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
-- 								<ssv:Real value="4.5" />
-- 							</ssv:Parameter>
-- 							<ssv:Parameter name="C1">
-- 								<ssv:Real value="2.5" />
-- 							</ssv:Parameter>
-- 						</ssv:Parameters>
-- 					</ssv:ParameterSet>
-- 				</ssd:ParameterValues>
-- 			</ssd:ParameterBinding>
-- 		</ssd:ParameterBindings>
-- 		<ssd:Elements>
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
-- 				<ssd:ParameterBindings>
-- 					<ssd:ParameterBinding>
-- 						<ssd:ParameterValues>
-- 							<ssv:ParameterSet version="1.0" name="parameters">
-- 								<ssv:Parameters>
-- 									<ssv:Parameter name="u">
-- 										<ssv:Real value="7.5" />
-- 									</ssv:Parameter>
-- 								</ssv:Parameters>
-- 							</ssv:ParameterSet>
-- 						</ssd:ParameterValues>
-- 					</ssd:ParameterBinding>
-- 				</ssd:ParameterBindings>
-- 			</ssd:Component>
-- 		</ssd:Elements>
-- 	</ssd:System>
-- 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
-- 		<ssd:Annotations>
-- 			<ssc:Annotation type="org.openmodelica">
-- 				<oms:SimulationInformation resultFile="exportConnectors_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:DefaultExperiment>
-- </ssd:SystemStructureDescription>
-- 
-- endResult
