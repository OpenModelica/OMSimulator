-- status: correct
-- linux: yes
-- mingw: yes
-- win: yes
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./setAndGetStartValues_lua/")

oms_newModel("setAndGetStartValues")

oms_addSystem("setAndGetStartValues.Root", oms_system_wc)
oms_addConnector("setAndGetStartValues.Root.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("setAndGetStartValues.Root.C2", oms_causality_output, oms_signal_type_real)
oms_addConnector("setAndGetStartValues.Root.C3", oms_causality_parameter, oms_signal_type_real)

-- set start values
oms_setReal("setAndGetStartValues.Root.C1:start", 20.0)

oms_addSubModel("setAndGetStartValues.Root.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms_setReal("setAndGetStartValues.Root.Gain.u:start", -40)

-- not allowed without:start suffix before intitialization or instantiation
oms_setReal("setAndGetStartValues.Root.Gain.u", -60)

oms_instantiate("setAndGetStartValues")
print("info:      Parameter settings")
print("info:      setAndGetStartValues.Root.C1:start         : " .. oms_getReal("setAndGetStartValues.Root.C1:start"))
print("info:      setAndGetStartValues.Root.Gain.u:start     : " .. oms_getReal("setAndGetStartValues.Root.Gain.u:start"))
print("info:      setAndGetStartValues.Root.Gain.u           : " .. oms_getReal("setAndGetStartValues.Root.Gain.u"))

oms_initialize("setAndGetStartValues")
print("info:    Initialization")
print("info:      setAndGetStartValues.Root.C1:start         : " .. oms_getReal("setAndGetStartValues.Root.C1:start"))
print("info:      setAndGetStartValues.Root.C1               : " .. oms_getReal("setAndGetStartValues.Root.C1"))
print("info:      setAndGetStartValues.Root.Gain.u:start     : " .. oms_getReal("setAndGetStartValues.Root.Gain.u:start"))
print("info:      setAndGetStartValues.Root.Gain.u           : " .. oms_getReal("setAndGetStartValues.Root.Gain.u"))

-- not allowed to provide start value after initialization
oms_setReal("setAndGetStartValues.Root.C1:start", -20)

-- set inputs after initialization
oms_setReal("setAndGetStartValues.Root.Gain.u", -35)
oms_setReal("setAndGetStartValues.Root.C1", 50.0)


oms_simulate("setAndGetStartValues")
print("info:    Simulation")
print("info:      setAndGetStartValues.Root.C1:start         : " .. oms_getReal("setAndGetStartValues.Root.C1:start"))
print("info:      setAndGetStartValues.Root.C1               : " .. oms_getReal("setAndGetStartValues.Root.C1"))
print("info:      setAndGetStartValues.Root.Gain.u:start     : " .. oms_getReal("setAndGetStartValues.Root.Gain.u:start"))
print("info:      setAndGetStartValues.Root.Gain.u           : " .. oms_getReal("setAndGetStartValues.Root.Gain.u"))

src = oms_list("setAndGetStartValues")
print(src)


-- Result:
-- error:   [setReal] wrong model state, cannot set value or start value for signal "setAndGetStartValues.Root.Gain.u"
-- info:      Parameter settings
-- info:      setAndGetStartValues.Root.C1:start         : 20.0
-- info:      setAndGetStartValues.Root.Gain.u:start     : -40.0
-- info:      setAndGetStartValues.Root.Gain.u           : -40.0
-- info:    Result file: setAndGetStartValues_res.mat (bufferSize=10)
-- info:    Initialization
-- info:      setAndGetStartValues.Root.C1:start         : 20.0
-- info:      setAndGetStartValues.Root.C1               : 0.0
-- info:      setAndGetStartValues.Root.Gain.u:start     : -40.0
-- info:      setAndGetStartValues.Root.Gain.u           : -40.0
-- error:   [setReal] wrong model state, cannot set value or start value for signal "setAndGetStartValues.Root.C1:start"
-- info:    Simulation
-- info:      setAndGetStartValues.Root.C1:start         : 20.0
-- info:      setAndGetStartValues.Root.C1               : 50.0
-- info:      setAndGetStartValues.Root.Gain.u:start     : -40.0
-- info:      setAndGetStartValues.Root.Gain.u           : -35.0
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="setAndGetStartValues" version="1.0">
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
-- 							<ssv:Parameter name="C1">
-- 								<ssv:Real value="20" />
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
-- 										<ssv:Real value="-40" />
-- 									</ssv:Parameter>
-- 								</ssv:Parameters>
-- 							</ssv:ParameterSet>
-- 						</ssd:ParameterValues>
-- 					</ssd:ParameterBinding>
-- 				</ssd:ParameterBindings>
-- 			</ssd:Component>
-- 		</ssd:Elements>
-- 		<ssd:Connections />
-- 	</ssd:System>
-- 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
-- 		<ssd:Annotations>
-- 			<ssc:Annotation type="org.openmodelica">
-- 				<oms:SimulationInformation resultFile="setAndGetStartValues_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter="" />
-- 			</ssc:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:DefaultExperiment>
-- </ssd:SystemStructureDescription>
-- 
-- info:    0 warnings
-- info:    2 errors
-- endResult
