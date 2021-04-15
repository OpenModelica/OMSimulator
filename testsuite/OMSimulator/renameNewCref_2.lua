-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./rename_new_cref_02_lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)

-- rename model
modelSnapshot = [[
  <?xml version="1.0"?>
  <oms:snapshot
    xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
    partial="false">
    <oms:file
      name="SystemStructure.ssd">
      <ssd:SystemStructureDescription
        xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
        xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription"
        xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
        xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping"
        xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary"
        xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
        name="model_4"
        version="1.0">
        <ssd:System
          name="root">
          <ssd:Annotations>
            <ssc:Annotation
              type="org.openmodelica">
              <oms:Annotations>
                <oms:SimulationInformation>
                  <oms:FixedStepMaster
                    description="oms-ma"
                    stepSize="0.100000"
                    absoluteTolerance="0.000100"
                    relativeTolerance="0.000100" />
                </oms:SimulationInformation>
              </oms:Annotations>
            </ssc:Annotation>
          </ssd:Annotations>
        </ssd:System>
        <ssd:DefaultExperiment
          startTime="0.000000"
          stopTime="1.000000">
          <ssd:Annotations>
            <ssc:Annotation
              type="org.openmodelica">
              <oms:Annotations>
                <oms:SimulationInformation
                  resultFile="model_res.mat"
                  loggingInterval="0.000000"
                  bufferSize="10"
                  signalFilter="resources/signalFilter.xml" />
              </oms:Annotations>
            </ssc:Annotation>
          </ssd:Annotations>
        </ssd:DefaultExperiment>
      </ssd:SystemStructureDescription>
    </oms:file>
    <oms:file
      name="resources/signalFilter.xml">
      <oms:SignalFilter
        version="1.0" />
    </oms:file>
  </oms:snapshot>
]]


newcref, status = oms_importSnapshot("model", modelSnapshot)
print(newcref)

oms_rename("model_4", "model_7")

snapshot = oms_exportSnapshot("model_7")
print(snapshot)

-- Result:
-- model_4
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
--       name="model_7"
--       version="1.0">
--       <ssd:System
--         name="root">
--         <ssd:Annotations>
--           <ssc:Annotation
--             type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation>
--                 <oms:FixedStepMaster
--                   description="oms-ma"
--                   stepSize="0.100000"
--                   absoluteTolerance="0.000100"
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
--                 resultFile="model_res.mat"
--                 loggingInterval="0.000000"
--                 bufferSize="10"
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
--       version="1.0" />
--   </oms:file>
-- </oms:snapshot>
--
-- endResult
