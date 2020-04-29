-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./import_export_parameters_lua/")

oms_newModel("import_export_parameters")
oms_addSystem("import_export_parameters.co_sim", oms_system_wc)

-- top level system connector 
oms_addConnector("import_export_parameters.co_sim.T_cref", oms_causality_parameter, oms_signal_type_real)
-- set top level system parameter
oms_setReal("import_export_parameters.co_sim.T_cref", 20.0)

oms_addSystem("import_export_parameters.co_sim.foo", oms_system_sc)
-- top level system connector
oms_addConnector("import_export_parameters.co_sim.foo.F_cref", oms_causality_output, oms_signal_type_real)
-- set top level system parameter
oms_setReal("import_export_parameters.co_sim.foo.F_cref", 30.0)

-- instantiate FMUs
oms_addSubModel("import_export_parameters.co_sim.addP", "../resources/Modelica.Blocks.Math.Add.fmu")
oms_addSubModel("import_export_parameters.co_sim.P", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms_addSubModel("import_export_parameters.co_sim.addI", "../resources/Modelica.Blocks.Math.Add3.fmu")

-- oms_addConnection("import_export_parameters.co_sim.F_cref", "import_export_parameters.co_sim.addP.y")

-- setParameters
oms_setReal("import_export_parameters.co_sim.addP.k1", 10.0)
oms_setReal("import_export_parameters.co_sim.addP.k2", -1.0)
oms_setReal("import_export_parameters.co_sim.addI.k2",  2.0)

-- simulation settings
oms_setStartTime("import_export_parameters", 0.0)
oms_setStopTime("import_export_parameters", 4.0)
oms_setFixedStepSize("import_export_parameters.co_sim", 1e-3)
oms_setResultFile("import_export_parameters", "import_export_parameters.mat", 100)

src, status = oms_list("import_export_parameters")
print(src)

oms_export("import_export_parameters", "import_export_parameters.ssp");
oms_delete("import_export_parameters")

oms_importFile("import_export_parameters.ssp");

oms_instantiate("import_export_parameters")

print("info:      Parameter settings")
print("info:      import_export_parameters.co_sim.addP.k1    : " .. oms_getReal("import_export_parameters.co_sim.addP.k1"))
print("info:      import_export_parameters.co_sim.addP.k2    : " .. oms_getReal("import_export_parameters.co_sim.addP.k2"))
print("info:      import_export_parameters.co_sim.addI.k2    : " .. oms_getReal("import_export_parameters.co_sim.addI.k2"))
print("info:      import_export_parameters.co_sim.T_cref     : " .. oms_getReal("import_export_parameters.co_sim.T_cref"))
print("info:      import_export_parameters.co_sim.foo.F_cref : " .. oms_getReal("import_export_parameters.co_sim.foo.F_cref"))

oms_initialize("import_export_parameters")
print("info:    Initialization")
print("info:      import_export_parameters.co_sim.addP.k1    : " .. oms_getReal("import_export_parameters.co_sim.addP.k1"))
print("info:      import_export_parameters.co_sim.addP.k2    : " .. oms_getReal("import_export_parameters.co_sim.addP.k2"))
print("info:      import_export_parameters.co_sim.addI.k2    : " .. oms_getReal("import_export_parameters.co_sim.addI.k2"))
print("info:      import_export_parameters.co_sim.T_cref     : " .. oms_getReal("import_export_parameters.co_sim.T_cref"))
print("info:      import_export_parameters.co_sim.foo.F_cref : " .. oms_getReal("import_export_parameters.co_sim.foo.F_cref"))

oms_simulate("import_export_parameters")
print("info:    Simulation")
print("info:      import_export_parameters.co_sim.addP.k1    : " .. oms_getReal("import_export_parameters.co_sim.addP.k1"))
print("info:      import_export_parameters.co_sim.addP.k2    : " .. oms_getReal("import_export_parameters.co_sim.addP.k2"))
print("info:      import_export_parameters.co_sim.addI.k2    : " .. oms_getReal("import_export_parameters.co_sim.addI.k2"))
print("info:      import_export_parameters.co_sim.T_cref     : " .. oms_getReal("import_export_parameters.co_sim.T_cref"))
print("info:      import_export_parameters.co_sim.foo.F_cref : " .. oms_getReal("import_export_parameters.co_sim.foo.F_cref"))

oms_terminate("import_export_parameters")
oms_delete("import_export_parameters")



-- Result:
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription name="import_export_parameters" version="1.0">
-- 	<ssd:System name="co_sim">
-- 		<ssd:Annotations>
-- 			<ssd:Annotation type="org.openmodelica">
-- 				<oms:SimulationInformation>
-- 					<FixedStepMaster description="oms-ma" stepSize="0.001000" />
-- 				</oms:SimulationInformation>
-- 			</ssd:Annotation>
-- 		</ssd:Annotations>
-- 		<ssd:Connectors>
-- 			<ssd:Connector name="T_cref" kind="parameter">
-- 				<ssc:Real />
-- 			</ssd:Connector>
-- 		</ssd:Connectors>
-- 		<ssd:ParameterBindings>
-- 			<ssd:ParameterBinding>
-- 				<ssv:ParameterSet version="1.0" name="parameters">
-- 					<ssv:Parameters>
-- 						<ssv:Parameter name="T_cref">
-- 							<ssv:Real value="20" />
-- 						</ssv:Parameter>
-- 					</ssv:Parameters>
-- 				</ssv:ParameterSet>
-- 			</ssd:ParameterBinding>
-- 		</ssd:ParameterBindings>
-- 		<ssd:Elements>
-- 			<ssd:System name="foo">
-- 				<ssd:Annotations>
-- 					<ssd:Annotation type="org.openmodelica">
-- 						<oms:SimulationInformation>
-- 							<VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
-- 						</oms:SimulationInformation>
-- 					</ssd:Annotation>
-- 				</ssd:Annotations>
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="F_cref" kind="output">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 				</ssd:Connectors>
-- 				<ssd:ParameterBindings>
-- 					<ssd:ParameterBinding>
-- 						<ssv:ParameterSet version="1.0" name="parameters">
-- 							<ssv:Parameters>
-- 								<ssv:Parameter name="F_cref">
-- 									<ssv:Real value="30" />
-- 								</ssv:Parameter>
-- 							</ssv:Parameters>
-- 						</ssv:ParameterSet>
-- 					</ssd:ParameterBinding>
-- 				</ssd:ParameterBindings>
-- 				<ssd:Elements />
-- 				<ssd:Connections />
-- 			</ssd:System>
-- 			<ssd:Component name="addP" type="application/x-fmu-sharedlibrary" source="resources/0001_addP.fmu">
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
-- 				<ssd:ParameterBindings>
-- 					<ssd:ParameterBinding>
-- 						<ssv:ParameterSet version="1.0" name="parameters">
-- 							<ssv:Parameters>
-- 								<ssv:Parameter name="k2">
-- 									<ssv:Real value="-1" />
-- 								</ssv:Parameter>
-- 								<ssv:Parameter name="k1">
-- 									<ssv:Real value="10" />
-- 								</ssv:Parameter>
-- 							</ssv:Parameters>
-- 						</ssv:ParameterSet>
-- 					</ssd:ParameterBinding>
-- 				</ssd:ParameterBindings>
-- 			</ssd:Component>
-- 			<ssd:Component name="addI" type="application/x-fmu-sharedlibrary" source="resources/0003_addI.fmu">
-- 				<ssd:Connectors>
-- 					<ssd:Connector name="u1" kind="input">
-- 						<ssc:Real />
-- 						<ssd:ConnectorGeometry x="0.000000" y="0.250000" />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="u2" kind="input">
-- 						<ssc:Real />
-- 						<ssd:ConnectorGeometry x="0.000000" y="0.500000" />
-- 					</ssd:Connector>
-- 					<ssd:Connector name="u3" kind="input">
-- 						<ssc:Real />
-- 						<ssd:ConnectorGeometry x="0.000000" y="0.750000" />
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
-- 					<ssd:Connector name="k3" kind="parameter">
-- 						<ssc:Real />
-- 					</ssd:Connector>
-- 				</ssd:Connectors>
-- 				<ssd:ParameterBindings>
-- 					<ssd:ParameterBinding>
-- 						<ssv:ParameterSet version="1.0" name="parameters">
-- 							<ssv:Parameters>
-- 								<ssv:Parameter name="k2">
-- 									<ssv:Real value="2" />
-- 								</ssv:Parameter>
-- 							</ssv:Parameters>
-- 						</ssv:ParameterSet>
-- 					</ssd:ParameterBinding>
-- 				</ssd:ParameterBindings>
-- 			</ssd:Component>
-- 			<ssd:Component name="P" type="application/x-fmu-sharedlibrary" source="resources/0002_P.fmu">
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
-- 		<ssd:Connections />
-- 	</ssd:System>
-- 	<ssd:DefaultExperiment startTime="0.000000" stopTime="4.000000" />
-- </ssd:SystemStructureDescription>
-- 
-- info:    model doesn't contain any continuous state
-- info:      Parameter settings
-- info:      import_export_parameters.co_sim.addP.k1    : 10.0
-- info:      import_export_parameters.co_sim.addP.k2    : -1.0
-- info:      import_export_parameters.co_sim.addI.k2    : 2.0
-- info:      import_export_parameters.co_sim.T_cref     : 20.0
-- info:      import_export_parameters.co_sim.foo.F_cref : 30.0
-- info:    Result file: import_export_parameters_res.mat (bufferSize=10)
-- info:    Initialization
-- info:      import_export_parameters.co_sim.addP.k1    : 10.0
-- info:      import_export_parameters.co_sim.addP.k2    : -1.0
-- info:      import_export_parameters.co_sim.addI.k2    : 2.0
-- info:      import_export_parameters.co_sim.T_cref     : 20.0
-- info:      import_export_parameters.co_sim.foo.F_cref : 30.0
-- info:    Simulation
-- info:      import_export_parameters.co_sim.addP.k1    : 10.0
-- info:      import_export_parameters.co_sim.addP.k2    : -1.0
-- info:      import_export_parameters.co_sim.addI.k2    : 2.0
-- info:      import_export_parameters.co_sim.T_cref     : 20.0
-- info:      import_export_parameters.co_sim.foo.F_cref : 30.0
-- endResult
