-- status: correct
-- teardown_command: rm -rf renameNewCref_2_lua/
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./renameNewCref_2_lua/")
oms_setWorkingDirectory("./renameNewCref_2_lua/")

oms_newModel("model")
oms_newModel("model_4")

oms_addSystem("model.root", oms_system_wc)

-- rename model to model_4
errormodelSnapshot = [[
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

-- error, as model name "model_4" already exists in the scope
newcref, status = oms_importSnapshot("model", errormodelSnapshot)

-- rename model
correctedModelSnapshot = [[
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
        name="model_5"
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

newcref, status = oms_importSnapshot("model", correctedModelSnapshot)
print(newcref)

oms_rename("model_5", "model_7")

snapshot = oms_exportSnapshot("model_7")
print(snapshot)

-- Result:
-- error:   [importSnapshot] Renaming the model "model" to "model_4" failed because another model with the same name already exists in the scope.
-- model_5
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
-- info:    0 warnings
-- info:    1 errors
-- endResult
