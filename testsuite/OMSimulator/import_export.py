## status: correct
## teardown_command: rm -rf import_export-py/ test-py.ssp
## linux: yes
## mingw: yes
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./import_export-py/")

def printStatus(status, expected):
  cmp = ""
  if status == expected:
    cmp = "correct"
  else:
    cmp = "wrong"

  if 0 == status:
    status = "ok"
  elif 1 == status:
    status = "warning"
  elif 3 == status:
    status = "error"
  print("status:  [%s] %s" % (cmp, status), flush=True)

status = oms.newModel("test")
printStatus(status, 0)

status = oms.addSystem("test.eoo", oms.system_tlm)
printStatus(status, 0)

status = oms.setTLMSocketData("test.eoo", "127.0.1.1", 11111, 11121)

status = oms.addSystem("test.eoo.foo", oms.system_wc)
printStatus(status, 0)

## CS FMU
status = oms.addSubModel("test.eoo.foo.A", "../resources/tlm.source.fmu")
printStatus(status, 0)

## table
status = oms.addSubModel("test.eoo.foo.T", "../resources/setpoint.csv")
printStatus(status, 0)

status = oms.addSystem("test.eoo.foo.goo", oms.system_sc)
printStatus(status, 0)

status = oms.addSystem("test.eoo.foo2", oms.system_wc)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo.f", oms.input, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo.x", oms.output, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo.v", oms.output, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo.y1", oms.output, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo.y2", oms.output, oms.signal_type_real)
printStatus(status, 0)

status = oms.addTLMBus("test.eoo.foo.tlm", oms.tlm_domain_mechanical, 1, oms.default)
printStatus(status, 0)

status = oms.addConnectorToTLMBus("test.eoo.foo.tlm", "test.eoo.foo.f", "effort")
printStatus(status, 0)

status = oms.addConnectorToTLMBus("test.eoo.foo.tlm", "test.eoo.foo.x", "state")
printStatus(status, 0)

status = oms.addConnectorToTLMBus("test.eoo.foo.tlm", "test.eoo.foo.v", "flow")
printStatus(status, 0)

status = oms.addBus("test.eoo.foo.bus")
printStatus(status, 0)

status = oms.addConnectorToBus("test.eoo.foo.bus", "test.eoo.foo.y1")
printStatus(status, 0)

status = oms.addConnectorToBus("test.eoo.foo.bus", "test.eoo.foo.y2")
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo2.f", oms.input, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo2.x", oms.output, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo2.v", oms.output, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo2.u1", oms.input, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("test.eoo.foo2.u2", oms.input, oms.signal_type_real)
printStatus(status, 0)

status = oms.addTLMBus("test.eoo.foo2.tlm", oms.tlm_domain_mechanical, 1, oms.default)
printStatus(status, 0)

status = oms.addConnectorToTLMBus("test.eoo.foo2.tlm", "test.eoo.foo2.f", "effort")
printStatus(status, 0)

status = oms.addConnectorToTLMBus("test.eoo.foo2.tlm", "test.eoo.foo2.x", "state")
printStatus(status, 0)

status = oms.addConnectorToTLMBus("test.eoo.foo2.tlm", "test.eoo.foo2.v", "flow")
printStatus(status, 0)

status = oms.addBus("test.eoo.foo2.bus")
printStatus(status, 0)

status = oms.addConnectorToBus("test.eoo.foo2.bus", "test.eoo.foo2.u1")
printStatus(status, 0)

status = oms.addConnectorToBus("test.eoo.foo2.bus", "test.eoo.foo2.u2")
printStatus(status, 0)

status = oms.addTLMConnection("test.eoo.foo.tlm", "test.eoo.foo2.tlm", 0.001, 0.3, 100, 0)
printStatus(status, 0)

status = oms.addConnection("test.eoo.foo.y1", "test.eoo.foo2.u1")
printStatus(status, 0)

status = oms.addConnection("test.eoo.foo.y2", "test.eoo.foo2.u2")
printStatus(status, 0)

status = oms.addConnection("test.eoo.foo.bus", "test.eoo.foo2.bus")
printStatus(status, 0)

src, status = oms.list("test")
printStatus(status, 0)
print(src)

status = oms.export("test", "test-py.ssp")
printStatus(status, 0)

status = oms.delete("test")
printStatus(status, 0)

model, status = oms.importFile("test-py.ssp")
printStatus(status, 0)

src, status = oms.list(model)
printStatus(status, 0)
print(src)

status = oms.delete(model)
printStatus(status, 0)

## Result:
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## <?xml version="1.0"?>
## <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="test" version="1.0">
##     <ssd:System name="eoo">
##         <ssd:Annotations>
##             <ssc:Annotation type="org.openmodelica">
##                 <oms:SimulationInformation>
##                     <oms:TlmMaster ip="127.0.1.1" managerport="11111" monitorport="11121" />
##                 </oms:SimulationInformation>
##             </ssc:Annotation>
##         </ssd:Annotations>
##         <ssd:Elements>
##             <ssd:System name="foo2">
##                 <ssd:Annotations>
##                     <ssc:Annotation type="org.openmodelica">
##                         <oms:SimulationInformation>
##                             <oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
##                         </oms:SimulationInformation>
##                     </ssc:Annotation>
##                 </ssd:Annotations>
##                 <ssd:Connectors>
##                     <ssd:Connector name="f" kind="input">
##                         <ssc:Real />
##                     </ssd:Connector>
##                     <ssd:Connector name="x" kind="output">
##                         <ssc:Real />
##                     </ssd:Connector>
##                     <ssd:Connector name="v" kind="output">
##                         <ssc:Real />
##                     </ssd:Connector>
##                     <ssd:Connector name="u1" kind="input">
##                         <ssc:Real />
##                     </ssd:Connector>
##                     <ssd:Connector name="u2" kind="input">
##                         <ssc:Real />
##                     </ssd:Connector>
##                 </ssd:Connectors>
##                 <ssd:Annotations>
##                     <ssc:Annotation type="org.openmodelica">
##                         <oms:Bus name="bus">
##                             <oms:Signals>
##                                 <oms:Signal name="u1" />
##                                 <oms:Signal name="u2" />
##                             </oms:Signals>
##                         </oms:Bus>
##                         <oms:Bus name="tlm" type="tlm" domain="mechanical" dimensions="1" interpolation="none">
##                             <oms:Signals>
##                                 <oms:Signal name="f" type="effort" />
##                                 <oms:Signal name="v" type="flow" />
##                                 <oms:Signal name="x" type="state" />
##                             </oms:Signals>
##                         </oms:Bus>
##                     </ssc:Annotation>
##                 </ssd:Annotations>
##             </ssd:System>
##             <ssd:System name="foo">
##                 <ssd:Annotations>
##                     <ssc:Annotation type="org.openmodelica">
##                         <oms:SimulationInformation>
##                             <oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
##                         </oms:SimulationInformation>
##                     </ssc:Annotation>
##                 </ssd:Annotations>
##                 <ssd:Connectors>
##                     <ssd:Connector name="f" kind="input">
##                         <ssc:Real />
##                     </ssd:Connector>
##                     <ssd:Connector name="x" kind="output">
##                         <ssc:Real />
##                     </ssd:Connector>
##                     <ssd:Connector name="v" kind="output">
##                         <ssc:Real />
##                     </ssd:Connector>
##                     <ssd:Connector name="y1" kind="output">
##                         <ssc:Real />
##                     </ssd:Connector>
##                     <ssd:Connector name="y2" kind="output">
##                         <ssc:Real />
##                     </ssd:Connector>
##                 </ssd:Connectors>
##                 <ssd:Elements>
##                     <ssd:System name="goo">
##                         <ssd:Annotations>
##                             <ssc:Annotation type="org.openmodelica">
##                                 <oms:SimulationInformation>
##                                     <oms:VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
##                                 </oms:SimulationInformation>
##                             </ssc:Annotation>
##                         </ssd:Annotations>
##                         <ssd:Connectors />
##                         <ssd:Elements />
##                         <ssd:Connections />
##                     </ssd:System>
##                     <ssd:Component name="T" type="application/table" source="resources/0002_T.csv">
##                         <ssd:Connectors>
##                             <ssd:Connector name="time" kind="output">
##                                 <ssc:Real />
##                                 <ssd:ConnectorGeometry x="1.000000" y="0.333333" />
##                             </ssd:Connector>
##                             <ssd:Connector name="speed" kind="output">
##                                 <ssc:Real />
##                                 <ssd:ConnectorGeometry x="1.000000" y="0.666667" />
##                             </ssd:Connector>
##                         </ssd:Connectors>
##                     </ssd:Component>
##                     <ssd:Component name="A" type="application/x-fmu-sharedlibrary" source="resources/0001_A.fmu">
##                         <ssd:Connectors>
##                             <ssd:Connector name="y" kind="output">
##                                 <ssc:Real />
##                                 <ssd:ConnectorGeometry x="1.000000" y="0.500000" />
##                             </ssd:Connector>
##                             <ssd:Connector name="A" kind="parameter">
##                                 <ssc:Real />
##                             </ssd:Connector>
##                             <ssd:Connector name="omega" kind="parameter">
##                                 <ssc:Real />
##                             </ssd:Connector>
##                         </ssd:Connectors>
##                     </ssd:Component>
##                 </ssd:Elements>
##                 <ssd:Annotations>
##                     <ssc:Annotation type="org.openmodelica">
##                         <oms:Bus name="bus">
##                             <oms:Signals>
##                                 <oms:Signal name="y1" />
##                                 <oms:Signal name="y2" />
##                             </oms:Signals>
##                         </oms:Bus>
##                         <oms:Bus name="tlm" type="tlm" domain="mechanical" dimensions="1" interpolation="none">
##                             <oms:Signals>
##                                 <oms:Signal name="f" type="effort" />
##                                 <oms:Signal name="v" type="flow" />
##                                 <oms:Signal name="x" type="state" />
##                             </oms:Signals>
##                         </oms:Bus>
##                     </ssc:Annotation>
##                 </ssd:Annotations>
##             </ssd:System>
##         </ssd:Elements>
##         <ssd:Connections>
##             <ssd:Connection startElement="foo" startConnector="y1" endElement="foo2" endConnector="u1" />
##             <ssd:Connection startElement="foo" startConnector="y2" endElement="foo2" endConnector="u2" />
##         </ssd:Connections>
##         <ssd:Annotations>
##             <ssc:Annotation type="org.openmodelica">
##                 <oms:Connections>
##                     <oms:Connection startElement="foo" startConnector="tlm" endElement="foo2" endConnector="tlm" delay="0.001000" alpha="0.300000" linearimpedance="100.000000" angularimpedance="0.000000" />
##                     <oms:Connection startElement="foo" startConnector="bus" endElement="foo2" endConnector="bus" />
##                 </oms:Connections>
##             </ssc:Annotation>
##         </ssd:Annotations>
##     </ssd:System>
##     <ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
##         <ssd:Annotations>
##             <ssc:Annotation type="org.openmodelica">
##                 <oms:SimulationInformation resultFile="test_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
##             </ssc:Annotation>
##         </ssd:Annotations>
##     </ssd:DefaultExperiment>
## </ssd:SystemStructureDescription>
##
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## <?xml version="1.0"?>
## <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="test" version="1.0">
##     <ssd:System name="eoo">
##         <ssd:Annotations>
##             <ssc:Annotation type="org.openmodelica">
##                 <oms:SimulationInformation>
##                     <oms:TlmMaster ip="" managerport="0" monitorport="0" />
##                 </oms:SimulationInformation>
##             </ssc:Annotation>
##         </ssd:Annotations>
##         <ssd:Elements>
##             <ssd:System name="foo2">
##                 <ssd:Annotations>
##                     <ssc:Annotation type="org.openmodelica">
##                         <oms:SimulationInformation>
##                             <oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
##                         </oms:SimulationInformation>
##                     </ssc:Annotation>
##                 </ssd:Annotations>
##                 <ssd:Connectors>
##                     <ssd:Connector name="f" kind="input">
##                         <ssc:Real />
##                     </ssd:Connector>
##                     <ssd:Connector name="x" kind="output">
##                         <ssc:Real />
##                     </ssd:Connector>
##                     <ssd:Connector name="v" kind="output">
##                         <ssc:Real />
##                     </ssd:Connector>
##                     <ssd:Connector name="u1" kind="input">
##                         <ssc:Real />
##                     </ssd:Connector>
##                     <ssd:Connector name="u2" kind="input">
##                         <ssc:Real />
##                     </ssd:Connector>
##                 </ssd:Connectors>
##                 <ssd:Annotations>
##                     <ssc:Annotation type="org.openmodelica">
##                         <oms:Bus name="bus">
##                             <oms:Signals>
##                                 <oms:Signal name="u1" />
##                                 <oms:Signal name="u2" />
##                             </oms:Signals>
##                         </oms:Bus>
##                         <oms:Bus name="tlm" type="tlm" domain="mechanical" dimensions="1" interpolation="none">
##                             <oms:Signals>
##                                 <oms:Signal name="f" type="effort" />
##                                 <oms:Signal name="v" type="flow" />
##                                 <oms:Signal name="x" type="state" />
##                             </oms:Signals>
##                         </oms:Bus>
##                     </ssc:Annotation>
##                 </ssd:Annotations>
##             </ssd:System>
##             <ssd:System name="foo">
##                 <ssd:Annotations>
##                     <ssc:Annotation type="org.openmodelica">
##                         <oms:SimulationInformation>
##                             <oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
##                         </oms:SimulationInformation>
##                     </ssc:Annotation>
##                 </ssd:Annotations>
##                 <ssd:Connectors>
##                     <ssd:Connector name="f" kind="input">
##                         <ssc:Real />
##                     </ssd:Connector>
##                     <ssd:Connector name="x" kind="output">
##                         <ssc:Real />
##                     </ssd:Connector>
##                     <ssd:Connector name="v" kind="output">
##                         <ssc:Real />
##                     </ssd:Connector>
##                     <ssd:Connector name="y1" kind="output">
##                         <ssc:Real />
##                     </ssd:Connector>
##                     <ssd:Connector name="y2" kind="output">
##                         <ssc:Real />
##                     </ssd:Connector>
##                 </ssd:Connectors>
##                 <ssd:Elements>
##                     <ssd:System name="goo">
##                         <ssd:Annotations>
##                             <ssc:Annotation type="org.openmodelica">
##                                 <oms:SimulationInformation>
##                                     <oms:VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
##                                 </oms:SimulationInformation>
##                             </ssc:Annotation>
##                         </ssd:Annotations>
##                     </ssd:System>
##                     <ssd:Component name="T" type="application/table" source="resources/0002_T.csv">
##                         <ssd:Connectors>
##                             <ssd:Connector name="time" kind="output">
##                                 <ssc:Real />
##                                 <ssd:ConnectorGeometry x="1.000000" y="0.333333" />
##                             </ssd:Connector>
##                             <ssd:Connector name="speed" kind="output">
##                                 <ssc:Real />
##                                 <ssd:ConnectorGeometry x="1.000000" y="0.666667" />
##                             </ssd:Connector>
##                         </ssd:Connectors>
##                     </ssd:Component>
##                     <ssd:Component name="A" type="application/x-fmu-sharedlibrary" source="resources/0001_A.fmu">
##                         <ssd:Connectors>
##                             <ssd:Connector name="y" kind="output">
##                                 <ssc:Real />
##                                 <ssd:ConnectorGeometry x="1.000000" y="0.500000" />
##                             </ssd:Connector>
##                             <ssd:Connector name="A" kind="parameter">
##                                 <ssc:Real />
##                             </ssd:Connector>
##                             <ssd:Connector name="omega" kind="parameter">
##                                 <ssc:Real />
##                             </ssd:Connector>
##                         </ssd:Connectors>
##                     </ssd:Component>
##                 </ssd:Elements>
##                 <ssd:Annotations>
##                     <ssc:Annotation type="org.openmodelica">
##                         <oms:Bus name="bus">
##                             <oms:Signals>
##                                 <oms:Signal name="y1" />
##                                 <oms:Signal name="y2" />
##                             </oms:Signals>
##                         </oms:Bus>
##                         <oms:Bus name="tlm" type="tlm" domain="mechanical" dimensions="1" interpolation="none">
##                             <oms:Signals>
##                                 <oms:Signal name="f" type="effort" />
##                                 <oms:Signal name="v" type="flow" />
##                                 <oms:Signal name="x" type="state" />
##                             </oms:Signals>
##                         </oms:Bus>
##                     </ssc:Annotation>
##                 </ssd:Annotations>
##             </ssd:System>
##         </ssd:Elements>
##         <ssd:Connections>
##             <ssd:Connection startElement="foo" startConnector="y1" endElement="foo2" endConnector="u1" />
##             <ssd:Connection startElement="foo" startConnector="y2" endElement="foo2" endConnector="u2" />
##         </ssd:Connections>
##         <ssd:Annotations>
##             <ssc:Annotation type="org.openmodelica">
##                 <oms:Connections>
##                     <oms:Connection startElement="foo" startConnector="tlm" endElement="foo2" endConnector="tlm" delay="0.001000" alpha="0.300000" linearimpedance="100.000000" angularimpedance="0.000000" />
##                     <oms:Connection startElement="foo" startConnector="bus" endElement="foo2" endConnector="bus" />
##                 </oms:Connections>
##             </ssc:Annotation>
##         </ssd:Annotations>
##     </ssd:System>
##     <ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
##         <ssd:Annotations>
##             <ssc:Annotation type="org.openmodelica">
##                 <oms:SimulationInformation resultFile="test_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
##             </ssc:Annotation>
##         </ssd:Annotations>
##     </ssd:DefaultExperiment>
## </ssd:SystemStructureDescription>
##
## status:  [correct] ok
## endResult
