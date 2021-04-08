-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./rename_lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)
oms_addSubModel("model.root.gain", "../resources/Modelica.Blocks.Math.Gain.fmu")

systemSnapshot = [[
    <?xml version="1.0"?>
    <oms:snapshot
      partial="true">
      <oms:file
        name="SystemStructure.ssd"
        node="model.root">
        <ssd:System
          name="root_3">
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
      </oms:file>
    </oms:snapshot>
]]

newcref, status = oms_importSnapshot("model", systemSnapshot)
print(newcref)

componentSnapshot = [[
    <?xml version="1.0"?>
    <oms:snapshot
      xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
      partial="true">
      <oms:file
        name="SystemStructure.ssd"
        node="model.root.gain">
        <ssd:Component
          name="gain_1"
          type="application/x-fmu-sharedlibrary"
          source="resources/0001_gain.fmu">
          <ssd:Connectors>
            <ssd:Connector
              name="u"
              kind="input">
              <ssc:Real />
              <ssd:ConnectorGeometry
                x="0.000000"
                y="0.500000" />
            </ssd:Connector>
            <ssd:Connector
              name="y"
              kind="output">
              <ssc:Real />
              <ssd:ConnectorGeometry
                x="1.000000"
                y="0.500000" />
            </ssd:Connector>
            <ssd:Connector
              name="k"
              kind="parameter">
              <ssc:Real />
            </ssd:Connector>
          </ssd:Connectors>
        </ssd:Component>
      </oms:file>
    </oms:snapshot>
]]

newcref, status = oms_importSnapshot("model", componentSnapshot)
print(newcref)

-- Result:
-- root_3
-- gain_1
-- endResult
