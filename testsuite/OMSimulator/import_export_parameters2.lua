-- status: correct
-- teardown_command: rm -rf import_export_parameters_02_lua/
-- linux: no
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./import_export_parameters_02_lua/")

oms_importFile("../resources/import_export_parameters2.ssp");

-- update of new values
oms_setReal("import_export_parameters.co_sim.Input_cref", -3.7)
oms_setReal("import_export_parameters.co_sim.addP.k1", -60)
oms_setReal("import_export_parameters.co_sim.addI.k2", -150)
oms_setReal("import_export_parameters.co_sim.foo.F_cref", -55)

-- set new value in resources
oms_setReal("import_export_parameters.co_sim.Input1", -1000)
oms_setReal("import_export_parameters.co_sim.addP.u1", 300)

src = oms_exportSnapshot("import_export_parameters")
print(src)

print("info:      Virgin Parameter settings")
print("info:      import_export_parameters.co_sim.addP.k1     : " .. oms_getReal("import_export_parameters.co_sim.addP.k1"))
print("info:      import_export_parameters.co_sim.addP.k2     : " .. oms_getReal("import_export_parameters.co_sim.addP.k2"))
print("info:      import_export_parameters.co_sim.addP.u1     : " .. oms_getReal("import_export_parameters.co_sim.addP.u1"))
print("info:      import_export_parameters.co_sim.addI.k2     : " .. oms_getReal("import_export_parameters.co_sim.addI.k2"))
print("info:      import_export_parameters.co_sim.addI.k3     : " .. oms_getReal("import_export_parameters.co_sim.addI.k3"))

-- Top level cref's
print("info:      Virgin-Top-System-Level-Vars")
print("info:      import_export_parameters.co_sim.Input_cref  : " .. oms_getReal("import_export_parameters.co_sim.Input_cref"))
print("info:      import_export_parameters.co_sim.Input1      : " .. oms_getReal("import_export_parameters.co_sim.Input1"))
print("info:      import_export_parameters.co_sim.T_cref      : " .. oms_getReal("import_export_parameters.co_sim.T_cref"))
print("info:      import_export_parameters.co_sim.k_cref      : " .. oms_getReal("import_export_parameters.co_sim.k_cref"))
print("info:      import_export_parameters.co_sim.Output_cref : " .. oms_getReal("import_export_parameters.co_sim.Output_cref"))
print("info:      import_export_parameters.co_sim.foo.F_cref  : " .. oms_getReal("import_export_parameters.co_sim.foo.F_cref"))

oms_instantiate("import_export_parameters")

print("info:      Parameter settings")
print("info:      import_export_parameters.co_sim.addP.k1     : " .. oms_getReal("import_export_parameters.co_sim.addP.k1"))
print("info:      import_export_parameters.co_sim.addP.k2     : " .. oms_getReal("import_export_parameters.co_sim.addP.k2"))
print("info:      import_export_parameters.co_sim.addP.u1     : " .. oms_getReal("import_export_parameters.co_sim.addP.u1"))
print("info:      import_export_parameters.co_sim.addP.y      : " .. oms_getReal("import_export_parameters.co_sim.addP.y"))
print("info:      import_export_parameters.co_sim.addI.k2     : " .. oms_getReal("import_export_parameters.co_sim.addI.k2"))
print("info:      import_export_parameters.co_sim.addI.k3     : " .. oms_getReal("import_export_parameters.co_sim.addI.k3"))

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
print("info:      import_export_parameters.co_sim.addI.k2     : " .. oms_getReal("import_export_parameters.co_sim.addI.k2"))
print("info:      import_export_parameters.co_sim.addI.k3     : " .. oms_getReal("import_export_parameters.co_sim.addI.k3"))

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
print("info:      import_export_parameters.co_sim.addI.k2     : " .. oms_getReal("import_export_parameters.co_sim.addI.k2"))
print("info:      import_export_parameters.co_sim.addI.k3     : " .. oms_getReal("import_export_parameters.co_sim.addI.k3"))

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
--           <ssd:ParameterBinding
--             source="resources/cosim.ssv" />
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
--               <ssd:ParameterBinding
--                 source="resources/foo.ssv" />
--             </ssd:ParameterBindings>
--             <ssd:Annotations>
--               <ssc:Annotation
--                 type="org.openmodelica">
--                 <oms:Annotations>
--                   <oms:SimulationInformation>
--                     <oms:VariableStepSolver
--                       description="cvode"
--                       absoluteTolerance="0.000100"
--                       relativeTolerance="0.000100"
--                       minimumStepSize="0.000100"
--                       maximumStepSize="0.100000"
--                       initialStepSize="0.000100" />
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
--               <ssd:ParameterBinding
--                 source="resources/addP.ssv" />
--             </ssd:ParameterBindings>
--           </ssd:Component>
--           <ssd:Component
--             name="addI"
--             type="application/x-fmu-sharedlibrary"
--             source="resources/0003_addI.fmu">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="u1"
--                 kind="input">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry
--                   x="0.000000"
--                   y="0.250000" />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="u2"
--                 kind="input">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry
--                   x="0.000000"
--                   y="0.500000" />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="u3"
--                 kind="input">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry
--                   x="0.000000"
--                   y="0.750000" />
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
--               <ssd:Connector
--                 name="k3"
--                 kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--             <ssd:ParameterBindings>
--               <ssd:ParameterBinding
--                 source="resources/addI.ssv" />
--             </ssd:ParameterBindings>
--           </ssd:Component>
--           <ssd:Component
--             name="P"
--             type="application/x-fmu-sharedlibrary"
--             source="resources/0002_P.fmu">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="u"
--                 kind="input">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry
--                   x="0.000000"
--                   y="0.500000" />
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
--                 name="k"
--                 kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--           </ssd:Component>
--         </ssd:Elements>
--         <ssd:Connections>
--           <ssd:Connection
--             startElement=""
--             startConnector="T_cref"
--             endElement="addI"
--             endConnector="k2" />
--           <ssd:Connection
--             startElement=""
--             startConnector="k_cref"
--             endElement="addP"
--             endConnector="k1" />
--           <ssd:Connection
--             startElement=""
--             startConnector="Input_cref"
--             endElement="addP"
--             endConnector="u1" />
--           <ssd:Connection
--             startElement="addP"
--             startConnector="y"
--             endElement=""
--             endConnector="Output_cref" />
--         </ssd:Connections>
--         <ssd:Annotations>
--           <ssc:Annotation
--             type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation>
--                 <oms:FixedStepMaster
--                   description="oms-ma"
--                   stepSize="0.001000"
--                   absoluteTolerance="0.000100"
--                   relativeTolerance="0.000100" />
--               </oms:SimulationInformation>
--             </oms:Annotations>
--           </ssc:Annotation>
--         </ssd:Annotations>
--       </ssd:System>
--       <ssd:DefaultExperiment
--         startTime="0.000000"
--         stopTime="4.000000">
--         <ssd:Annotations>
--           <ssc:Annotation
--             type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation
--                 resultFile="import_export_parameters.mat"
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
--     name="resources/cosim.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="k_cref">
--           <ssv:Real
--             value="30" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="T_cref">
--           <ssv:Real
--             value="20" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="Input_cref">
--           <ssv:Real
--             value="-3.7000000000000002" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="Input1">
--           <ssv:Real
--             value="-1000" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/foo.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="F_cref">
--           <ssv:Real
--             value="-55" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/addP.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="u1">
--           <ssv:Real
--             value="300" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="k2">
--           <ssv:Real
--             value="-1" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="k1">
--           <ssv:Real
--             value="-60" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/addI.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="k2">
--           <ssv:Real
--             value="-150" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
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
--         name="import_export_parameters.co_sim.addI.u1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.addI.u2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.addI.u3"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.addI.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.addI.k1"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.addI.k2"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.addI.k3"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.P.u"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.P.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="import_export_parameters.co_sim.P.k"
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
-- info:      import_export_parameters.co_sim.addP.k1     : -60.0
-- info:      import_export_parameters.co_sim.addP.k2     : -1.0
-- info:      import_export_parameters.co_sim.addP.u1     : 300.0
-- info:      import_export_parameters.co_sim.addI.k2     : -150.0
-- info:      import_export_parameters.co_sim.addI.k3     : 1.0
-- info:      Virgin-Top-System-Level-Vars
-- info:      import_export_parameters.co_sim.Input_cref  : -3.7
-- info:      import_export_parameters.co_sim.Input1      : -1000.0
-- info:      import_export_parameters.co_sim.T_cref      : 20.0
-- info:      import_export_parameters.co_sim.k_cref      : 30.0
-- info:      import_export_parameters.co_sim.Output_cref : 0.0
-- info:      import_export_parameters.co_sim.foo.F_cref  : -55.0
-- info:    model doesn't contain any continuous state
-- info:      Parameter settings
-- info:      import_export_parameters.co_sim.addP.k1     : -60.0
-- info:      import_export_parameters.co_sim.addP.k2     : -1.0
-- info:      import_export_parameters.co_sim.addP.u1     : 300.0
-- info:      import_export_parameters.co_sim.addP.y      : -18000.0
-- info:      import_export_parameters.co_sim.addI.k2     : -150.0
-- info:      import_export_parameters.co_sim.addI.k3     : 1.0
-- info:      Top-System-Level-Vars
-- info:      import_export_parameters.co_sim.Input_cref  : -3.7
-- info:      import_export_parameters.co_sim.Input1      : -1000.0
-- info:      import_export_parameters.co_sim.T_cref      : 20.0
-- info:      import_export_parameters.co_sim.k_cref      : 30.0
-- info:      import_export_parameters.co_sim.Output_cref : 0.0
-- info:      import_export_parameters.co_sim.foo.F_cref  : -55.0
-- info:    Result file: import_export_parameters.mat (bufferSize=100)
-- info:    Initialization
-- info:      import_export_parameters.co_sim.addP.k1     : 30.0
-- info:      import_export_parameters.co_sim.addP.k2     : -1.0
-- info:      import_export_parameters.co_sim.addP.u1     : -3.7
-- info:      import_export_parameters.co_sim.addP.y      : -111.0
-- info:      import_export_parameters.co_sim.addI.k2     : 20.0
-- info:      import_export_parameters.co_sim.addI.k3     : 1.0
-- info:      Top-System-Level-Vars
-- info:      import_export_parameters.co_sim.Input_cref  : -3.7
-- info:      import_export_parameters.co_sim.Input1      : -1000.0
-- info:      import_export_parameters.co_sim.T_cref      : 20.0
-- info:      import_export_parameters.co_sim.k_cref      : 30.0
-- info:      import_export_parameters.co_sim.Output_cref : -111.0
-- info:      import_export_parameters.co_sim.foo.F_cref  : -55.0
-- info:    Simulation
-- info:      import_export_parameters.co_sim.addP.k1     : 30.0
-- info:      import_export_parameters.co_sim.addP.k2     : -1.0
-- info:      import_export_parameters.co_sim.addP.u1     : -3.7
-- info:      import_export_parameters.co_sim.addP.y      : -111.0
-- info:      import_export_parameters.co_sim.addI.k2     : 20.0
-- info:      import_export_parameters.co_sim.addI.k3     : 1.0
-- info:      Top-System-Level-Vars
-- info:      import_export_parameters.co_sim.Input_cref  : -3.7
-- info:      import_export_parameters.co_sim.Input1      : -1000.0
-- info:      import_export_parameters.co_sim.T_cref      : 20.0
-- info:      import_export_parameters.co_sim.k_cref      : 30.0
-- info:      import_export_parameters.co_sim.Output_cref : -111.0
-- info:      import_export_parameters.co_sim.foo.F_cref  : -55.0
-- endResult
