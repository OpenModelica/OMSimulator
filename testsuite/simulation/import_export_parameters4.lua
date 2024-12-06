-- status: correct
-- teardown_command: rm -rf import_export_parameters4_lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./import_export_parameters4_lua/")
oms_setWorkingDirectory("./import_export_parameters4_lua/")

oms_newModel("import_export_parameters")

oms_addSystem("import_export_parameters.co_sim", oms_system_wc)
oms_addConnector("import_export_parameters.co_sim.Input_cref", oms_causality_input, oms_signal_type_real)
oms_addConnector("import_export_parameters.co_sim.Input1", oms_causality_input, oms_signal_type_real)
oms_addConnector("import_export_parameters.co_sim.Output_cref", oms_causality_output, oms_signal_type_real)
oms_addConnector("import_export_parameters.co_sim.T_cref", oms_causality_parameter, oms_signal_type_real)
oms_addConnector("import_export_parameters.co_sim.k_cref", oms_causality_parameter, oms_signal_type_real)

oms_setReal("import_export_parameters.co_sim.Input_cref", -20.0)
oms_setReal("import_export_parameters.co_sim.T_cref", 20.0)
oms_setReal("import_export_parameters.co_sim.k_cref", 30.0)


oms_addSystem("import_export_parameters.co_sim.foo", oms_system_sc)
oms_addConnector("import_export_parameters.co_sim.foo.F_cref", oms_causality_parameter, oms_signal_type_real)
oms_setReal("import_export_parameters.co_sim.foo.F_cref", -40.0)

-- instantiate FMUs
oms_addSubModel("import_export_parameters.co_sim.addP", "../../resources/Modelica.Blocks.Math.Add.fmu")
oms_setReal("import_export_parameters.co_sim.addP.k1", 60)
oms_setReal("import_export_parameters.co_sim.addP.k2", 150)

oms_setResultFile("import_export_parameters", "import_export_parameters4.mat", 100)

src = oms_exportSnapshot("import_export_parameters")
print(src)

print("info:      Virgin Parameter settings")
print("info:      import_export_parameters.co_sim.addP.k1     : " .. oms_getReal("import_export_parameters.co_sim.addP.k1"))
print("info:      import_export_parameters.co_sim.addP.k2     : " .. oms_getReal("import_export_parameters.co_sim.addP.k2"))
print("info:      import_export_parameters.co_sim.addP.u1     : " .. oms_getReal("import_export_parameters.co_sim.addP.u1"))

-- Top level cref's
print("info:      Virgin-Top-System-Level-Vars")
print("info:      import_export_parameters.co_sim.Input_cref  : " .. oms_getReal("import_export_parameters.co_sim.Input_cref"))
print("info:      import_export_parameters.co_sim.Input1      : " .. oms_getReal("import_export_parameters.co_sim.Input1"))
print("info:      import_export_parameters.co_sim.T_cref      : " .. oms_getReal("import_export_parameters.co_sim.T_cref"))
print("info:      import_export_parameters.co_sim.k_cref      : " .. oms_getReal("import_export_parameters.co_sim.k_cref"))
print("info:      import_export_parameters.co_sim.foo.F_cref  : " .. oms_getReal("import_export_parameters.co_sim.foo.F_cref"))

oms_instantiate("import_export_parameters")

print("info:      Parameter settings")
print("info:      import_export_parameters.co_sim.addP.k1     : " .. oms_getReal("import_export_parameters.co_sim.addP.k1"))
print("info:      import_export_parameters.co_sim.addP.k2     : " .. oms_getReal("import_export_parameters.co_sim.addP.k2"))
print("info:      import_export_parameters.co_sim.addP.u1     : " .. oms_getReal("import_export_parameters.co_sim.addP.u1"))
print("info:      import_export_parameters.co_sim.addP.y      : " .. oms_getReal("import_export_parameters.co_sim.addP.y"))

-- Top level cref's
print("info:      Top-System-Level-Vars")
print("info:      import_export_parameters.co_sim.Input_cref  : " .. oms_getReal("import_export_parameters.co_sim.Input_cref"))
print("info:      import_export_parameters.co_sim.Input1      : " .. oms_getReal("import_export_parameters.co_sim.Input1"))
print("info:      import_export_parameters.co_sim.T_cref      : " .. oms_getReal("import_export_parameters.co_sim.T_cref"))
print("info:      import_export_parameters.co_sim.k_cref      : " .. oms_getReal("import_export_parameters.co_sim.k_cref"))
print("info:      import_export_parameters.co_sim.Output_cref : " .. oms_getReal("import_export_parameters.co_sim.Output_cref"))
print("info:      import_export_parameters.co_sim.foo.F_cref  : " .. oms_getReal("import_export_parameters.co_sim.foo.F_cref"))

oms_initialize("import_export_parameters")
print("info:    Initialization")
print("info:      import_export_parameters.co_sim.addP.k1     : " .. oms_getReal("import_export_parameters.co_sim.addP.k1"))
print("info:      import_export_parameters.co_sim.addP.k2     : " .. oms_getReal("import_export_parameters.co_sim.addP.k2"))
print("info:      import_export_parameters.co_sim.addP.u1     : " .. oms_getReal("import_export_parameters.co_sim.addP.u1"))
print("info:      import_export_parameters.co_sim.addP.y      : " .. oms_getReal("import_export_parameters.co_sim.addP.y"))

-- Top level cref's
print("info:      Top-System-Level-Vars")
print("info:      import_export_parameters.co_sim.Input_cref  : " .. oms_getReal("import_export_parameters.co_sim.Input_cref"))
print("info:      import_export_parameters.co_sim.Input1      : " .. oms_getReal("import_export_parameters.co_sim.Input1"))
print("info:      import_export_parameters.co_sim.T_cref      : " .. oms_getReal("import_export_parameters.co_sim.T_cref"))
print("info:      import_export_parameters.co_sim.k_cref      : " .. oms_getReal("import_export_parameters.co_sim.k_cref"))
print("info:      import_export_parameters.co_sim.Output_cref : " .. oms_getReal("import_export_parameters.co_sim.Output_cref"))
print("info:      import_export_parameters.co_sim.foo.F_cref  : " .. oms_getReal("import_export_parameters.co_sim.foo.F_cref"))

oms_simulate("import_export_parameters")
print("info:    Simulation")
print("info:      import_export_parameters.co_sim.addP.k1     : " .. oms_getReal("import_export_parameters.co_sim.addP.k1"))
print("info:      import_export_parameters.co_sim.addP.k2     : " .. oms_getReal("import_export_parameters.co_sim.addP.k2"))
print("info:      import_export_parameters.co_sim.addP.u1     : " .. oms_getReal("import_export_parameters.co_sim.addP.u1"))
print("info:      import_export_parameters.co_sim.addP.y      : " .. oms_getReal("import_export_parameters.co_sim.addP.y"))

-- Top level cref's
print("info:      Top-System-Level-Vars")
print("info:      import_export_parameters.co_sim.Input_cref  : " .. oms_getReal("import_export_parameters.co_sim.Input_cref"))
print("info:      import_export_parameters.co_sim.Input1      : " .. oms_getReal("import_export_parameters.co_sim.Input1"))
print("info:      import_export_parameters.co_sim.T_cref      : " .. oms_getReal("import_export_parameters.co_sim.T_cref"))
print("info:      import_export_parameters.co_sim.k_cref      : " .. oms_getReal("import_export_parameters.co_sim.k_cref"))
print("info:      import_export_parameters.co_sim.Output_cref : " .. oms_getReal("import_export_parameters.co_sim.Output_cref"))
print("info:      import_export_parameters.co_sim.foo.F_cref  : " .. oms_getReal("import_export_parameters.co_sim.foo.F_cref"))

oms_terminate("import_export_parameters")
oms_delete("import_export_parameters")


-- Result:
-- <?xml version="1.0"?>
-- <oms:snapshot
--   xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
--   partial="false">
--   <oms:file
--     name="SystemStructure.ssd">
--     <ssd:SystemStructureDescription
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping"
--       xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary"
--       xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
--       name="import_export_parameters"
--       version="1.0">
--       <ssd:System
--         name="co_sim">
--         <ssd:Connectors>
--           <ssd:Connector
--             name="Input_cref"
--             kind="input">
--             <ssc:Real />
--           </ssd:Connector>
--           <ssd:Connector
--             name="Input1"
--             kind="input">
--             <ssc:Real />
--           </ssd:Connector>
--           <ssd:Connector
--             name="Output_cref"
--             kind="output">
--             <ssc:Real />
--           </ssd:Connector>
--           <ssd:Connector
--             name="T_cref"
--             kind="parameter">
--             <ssc:Real />
--           </ssd:Connector>
--           <ssd:Connector
--             name="k_cref"
--             kind="parameter">
--             <ssc:Real />
--           </ssd:Connector>
--         </ssd:Connectors>
--         <ssd:ParameterBindings>
--           <ssd:ParameterBinding>
--             <ssd:ParameterValues>
--               <ssv:ParameterSet
--                 version="1.0"
--                 name="parameters">
--                 <ssv:Parameters>
--                   <ssv:Parameter
--                     name="k_cref">
--                     <ssv:Real
--                       value="30" />
--                   </ssv:Parameter>
--                   <ssv:Parameter
--                     name="T_cref">
--                     <ssv:Real
--                       value="20" />
--                   </ssv:Parameter>
--                   <ssv:Parameter
--                     name="Input_cref">
--                     <ssv:Real
--                       value="-20" />
--                   </ssv:Parameter>
--                 </ssv:Parameters>
--               </ssv:ParameterSet>
--             </ssd:ParameterValues>
--           </ssd:ParameterBinding>
--         </ssd:ParameterBindings>
--         <ssd:Elements>
--           <ssd:System
--             name="foo">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="F_cref"
--                 kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--             <ssd:ParameterBindings>
--               <ssd:ParameterBinding>
--                 <ssd:ParameterValues>
--                   <ssv:ParameterSet
--                     version="1.0"
--                     name="parameters">
--                     <ssv:Parameters>
--                       <ssv:Parameter
--                         name="F_cref">
--                         <ssv:Real
--                           value="-40" />
--                       </ssv:Parameter>
--                     </ssv:Parameters>
--                   </ssv:ParameterSet>
--                 </ssd:ParameterValues>
--               </ssd:ParameterBinding>
--             </ssd:ParameterBindings>
--             <ssd:Annotations>
--               <ssc:Annotation
--                 type="org.openmodelica">
--                 <oms:Annotations>
--                   <oms:SimulationInformation>
--                     <oms:VariableStepSolver
--                       description="cvode"
--                       relativeTolerance="0.0001"
--                       minimumStepSize="1e-12"
--                       maximumStepSize="0.001"
--                       initialStepSize="1e-06" />
--                   </oms:SimulationInformation>
--                 </oms:Annotations>
--               </ssc:Annotation>
--             </ssd:Annotations>
--           </ssd:System>
--           <ssd:Component
--             name="addP"
--             type="application/x-fmu-sharedlibrary"
--             source="resources/0001_addP.fmu">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="u1"
--                 kind="input">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry
--                   x="0.000000"
--                   y="0.333333" />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="u2"
--                 kind="input">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry
--                   x="0.000000"
--                   y="0.666667" />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="y"
--                 kind="output">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry
--                   x="1.000000"
--                   y="0.500000" />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="k1"
--                 kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="k2"
--                 kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--             <ssd:ParameterBindings>
--               <ssd:ParameterBinding>
--                 <ssd:ParameterValues>
--                   <ssv:ParameterSet
--                     version="1.0"
--                     name="parameters">
--                     <ssv:Parameters>
--                       <ssv:Parameter
--                         name="k2">
--                         <ssv:Real
--                           value="150" />
--                       </ssv:Parameter>
--                       <ssv:Parameter
--                         name="k1">
--                         <ssv:Real
--                           value="60" />
--                       </ssv:Parameter>
--                     </ssv:Parameters>
--                   </ssv:ParameterSet>
--                 </ssd:ParameterValues>
--               </ssd:ParameterBinding>
--             </ssd:ParameterBindings>
--           </ssd:Component>
--         </ssd:Elements>
--         <ssd:Annotations>
--           <ssc:Annotation
--             type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation>
--                 <oms:FixedStepMaster
--                   description="oms-ma"
--                   stepSize="0.001000"
--                   relativeTolerance="0.000100" />
--               </oms:SimulationInformation>
--             </oms:Annotations>
--           </ssc:Annotation>
--         </ssd:Annotations>
--       </ssd:System>
--       <ssd:DefaultExperiment
--         startTime="0.000000"
--         stopTime="1.000000">
--         <ssd:Annotations>
--           <ssc:Annotation
--             type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation
--                 resultFile="import_export_parameters4.mat"
--                 loggingInterval="0.000000"
--                 bufferSize="100"
--                 signalFilter="resources/signalFilter.xml" />
--             </oms:Annotations>
--           </ssc:Annotation>
--         </ssd:Annotations>
--       </ssd:DefaultExperiment>
--     </ssd:SystemStructureDescription>
--   </oms:file>
--   <oms:file
--     name="resources/signalFilter.xml">
--     <oms:SignalFilter
--       version="1.0">
--       <oms:Variable
--         name="import_export_parameters.co_sim.Input_cref"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.Input1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.Output_cref"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.T_cref"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.k_cref"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.addP.u1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.addP.u2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.addP.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.addP.k1"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.addP.k2"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.foo.F_cref"
--         type="Real"
--         kind="parameter" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- info:      Virgin Parameter settings
-- info:      import_export_parameters.co_sim.addP.k1     : 60.0
-- info:      import_export_parameters.co_sim.addP.k2     : 150.0
-- info:      import_export_parameters.co_sim.addP.u1     : 0.0
-- info:      Virgin-Top-System-Level-Vars
-- info:      import_export_parameters.co_sim.Input_cref  : -20.0
-- info:      import_export_parameters.co_sim.Input1      : 0.0
-- info:      import_export_parameters.co_sim.T_cref      : 20.0
-- info:      import_export_parameters.co_sim.k_cref      : 30.0
-- info:      import_export_parameters.co_sim.foo.F_cref  : -40.0
-- info:    model doesn't contain any continuous state
-- info:      Parameter settings
-- info:      import_export_parameters.co_sim.addP.k1     : 60.0
-- info:      import_export_parameters.co_sim.addP.k2     : 150.0
-- info:      import_export_parameters.co_sim.addP.u1     : 0.0
-- info:      import_export_parameters.co_sim.addP.y      : 0.0
-- info:      Top-System-Level-Vars
-- info:      import_export_parameters.co_sim.Input_cref  : -20.0
-- info:      import_export_parameters.co_sim.Input1      : 0.0
-- info:      import_export_parameters.co_sim.T_cref      : 20.0
-- info:      import_export_parameters.co_sim.k_cref      : 30.0
-- info:      import_export_parameters.co_sim.Output_cref : 0.0
-- info:      import_export_parameters.co_sim.foo.F_cref  : -40.0
-- info:    maximum step size for 'import_export_parameters.co_sim.foo': 0.001000
-- info:    Result file: import_export_parameters4.mat (bufferSize=100)
-- info:    Initialization
-- info:      import_export_parameters.co_sim.addP.k1     : 60.0
-- info:      import_export_parameters.co_sim.addP.k2     : 150.0
-- info:      import_export_parameters.co_sim.addP.u1     : 0.0
-- info:      import_export_parameters.co_sim.addP.y      : 0.0
-- info:      Top-System-Level-Vars
-- info:      import_export_parameters.co_sim.Input_cref  : -20.0
-- info:      import_export_parameters.co_sim.Input1      : 0.0
-- info:      import_export_parameters.co_sim.T_cref      : 20.0
-- info:      import_export_parameters.co_sim.k_cref      : 30.0
-- info:      import_export_parameters.co_sim.Output_cref : 0.0
-- info:      import_export_parameters.co_sim.foo.F_cref  : -40.0
-- info:    Simulation
-- info:      import_export_parameters.co_sim.addP.k1     : 60.0
-- info:      import_export_parameters.co_sim.addP.k2     : 150.0
-- info:      import_export_parameters.co_sim.addP.u1     : 0.0
-- info:      import_export_parameters.co_sim.addP.y      : 0.0
-- info:      Top-System-Level-Vars
-- info:      import_export_parameters.co_sim.Input_cref  : -20.0
-- info:      import_export_parameters.co_sim.Input1      : 0.0
-- info:      import_export_parameters.co_sim.T_cref      : 20.0
-- info:      import_export_parameters.co_sim.k_cref      : 30.0
-- info:      import_export_parameters.co_sim.Output_cref : 0.0
-- info:      import_export_parameters.co_sim.foo.F_cref  : -40.0
-- info:    Final Statistics for 'import_export_parameters.co_sim.foo':
--          NumSteps = 1001 NumRhsEvals  = 1002 NumLinSolvSetups = 51
--          NumNonlinSolvIters = 1001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
-- endResult
