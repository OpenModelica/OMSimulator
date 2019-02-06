## status: correct
## teardown_command: rm -rf tlmbuses-py/
## linux: yes
## mingw: yes
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./tlmbuses-py/")

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
  print "status:  [%s] %s" % (cmp, status)

oms.newModel("model")
oms.addSystem("model.tlm", oms.system_tlm)
oms.setTLMSocketData("model.tlm", "127.0.1.1", 11111, 11121)
oms.addSystem("model.tlm.wc1", oms.system_wc)
oms.addConnector("model.tlm.wc1.y", oms.output, oms.signal_type_real)
oms.addConnector("model.tlm.wc1.x", oms.output, oms.signal_type_real)
oms.addConnector("model.tlm.wc1.v", oms.output, oms.signal_type_real)
oms.addConnector("model.tlm.wc1.f", oms.input, oms.signal_type_real)

oms.addSystem("model.tlm.wc2", oms.system_wc)
oms.addConnector("model.tlm.wc2.y", oms.input, oms.signal_type_real)
oms.addConnector("model.tlm.wc2.x", oms.output, oms.signal_type_real)
oms.addConnector("model.tlm.wc2.v", oms.output, oms.signal_type_real)
oms.addConnector("model.tlm.wc2.f", oms.input, oms.signal_type_real)

status = oms.addTLMBus("model.tlm.wc1.bus1", oms.tlm_domain_input, 1, oms.default)
printStatus(status, 0)

status = oms.addConnectorToTLMBus("model.tlm.wc1.bus1","model.tlm.wc1.y", "value")
printStatus(status, 0)

## Test adding non-existing connector
status = oms.addConnectorToTLMBus("model.tlm.wc1.bus1","model.tlm.wc1.z", "value")
printStatus(status, 3)

## Test adding connector with illegal type
status = oms.addConnectorToTLMBus("model.tlm.wc1.bus1","model.tlm.wc1.y", "effort")
printStatus(status, 3)

status = oms.addTLMBus("model.tlm.wc1.bus2", oms.tlm_domain_mechanical, 1, oms.default)
printStatus(status, 0)

status = oms.addConnectorToTLMBus("model.tlm.wc1.bus2","model.tlm.wc1.x", "state")
printStatus(status, 0)

status = oms.addConnectorToTLMBus("model.tlm.wc1.bus2","model.tlm.wc1.v", "flow")
printStatus(status, 0)

## Test adding already existing variable type to TLm bus
status = oms.addConnectorToTLMBus("model.tlm.wc1.bus2","model.tlm.wc1.f", "flow")
printStatus(status, 3)

status = oms.addConnectorToTLMBus("model.tlm.wc1.bus2","model.tlm.wc1.f", "effort")
printStatus(status, 0)

status = oms.addTLMBus("model.tlm.wc2.bus2", oms.tlm_domain_output, 1, oms.default)
printStatus(status, 0)

status = oms.addConnectorToTLMBus("model.tlm.wc2.bus2","model.tlm.wc2.y", "value")
printStatus(status, 0)

status = oms.addTLMConnection("model.tlm.wc1.bus1","model.tlm.wc2.bus2", 0.001,0.3,100,0)
printStatus(status, 0)

src, status = oms.list("model.tlm")
print src

status = oms.deleteConnectorFromTLMBus("model.tlm.wc1.bus2","model.tlm.wc1.x")
printStatus(status, 0)

status, src = oms.list("model.tlm")
print src

status = oms.delete("model")
printStatus(status, 0)

## Result:
## status:  [correct] ok
## status:  [correct] ok
## error:   [addConnectorToTLMBus] Connector "z" not found in system "model.tlm.wc1"
## status:  [correct] error
## error:   [addConnector] Unknown TLM variable type: "effort"
## status:  [correct] error
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## error:   [addConnector] TLM bus connector "bus2" already contains a variable with type "flow"
## status:  [correct] error
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## 0
## status:  [correct] ok
## <?xml version="1.0"?>
## <ssd:System name="tlm">
## 	<ssd:SimulationInformation>
## 		<ssd:Annotations>
## 			<ssd:Annotation type="org.openmodelica">
## 				<oms:TlmMaster ip="127.0.1.1" managerport="11111" monitorport="11121" />
## 			</ssd:Annotation>
## 		</ssd:Annotations>
## 	</ssd:SimulationInformation>
## 	<ssd:Elements>
## 		<ssd:System name="wc2">
## 			<ssd:SimulationInformation>
## 				<FixedStepMaster description="oms-ma" stepSize="0.100000" />
## 			</ssd:SimulationInformation>
## 			<ssd:Elements />
## 			<ssd:Connectors>
## 				<ssd:Connector name="y" kind="input" type="Real" />
## 				<ssd:Connector name="x" kind="output" type="Real" />
## 				<ssd:Connector name="v" kind="output" type="Real" />
## 				<ssd:Connector name="f" kind="input" type="Real" />
## 			</ssd:Connectors>
## 			<ssd:Connections />
## 			<ssd:Annotations>
## 				<ssd:Annotation type="org.openmodelica">
## 					<oms:Bus name="bus2" type="tlm" domain="output" dimensions="1" interpolation="none">
## 						<oms:Signals>
## 							<oms:Signal name="y" type="value" />
## 						</oms:Signals>
## 					</oms:Bus>
## 				</ssd:Annotation>
## 			</ssd:Annotations>
## 		</ssd:System>
## 		<ssd:System name="wc1">
## 			<ssd:SimulationInformation>
## 				<FixedStepMaster description="oms-ma" stepSize="0.100000" />
## 			</ssd:SimulationInformation>
## 			<ssd:Elements />
## 			<ssd:Connectors>
## 				<ssd:Connector name="y" kind="output" type="Real" />
## 				<ssd:Connector name="x" kind="output" type="Real" />
## 				<ssd:Connector name="v" kind="output" type="Real" />
## 				<ssd:Connector name="f" kind="input" type="Real" />
## 			</ssd:Connectors>
## 			<ssd:Connections />
## 			<ssd:Annotations>
## 				<ssd:Annotation type="org.openmodelica">
## 					<oms:Bus name="bus1" type="tlm" domain="input" dimensions="1" interpolation="none">
## 						<oms:Signals>
## 							<oms:Signal name="y" type="value" />
## 						</oms:Signals>
## 					</oms:Bus>
## 					<oms:Bus name="bus2" type="tlm" domain="mechanical" dimensions="1" interpolation="none">
## 						<oms:Signals>
## 							<oms:Signal name="f" type="effort" />
## 							<oms:Signal name="v" type="flow" />
## 						</oms:Signals>
## 					</oms:Bus>
## 				</ssd:Annotation>
## 			</ssd:Annotations>
## 		</ssd:System>
## 	</ssd:Elements>
## 	<ssd:Connectors />
## 	<ssd:Connections />
## 	<ssd:Annotations>
## 		<ssd:Annotation type="org.openmodelica">
## 			<oms:Connections>
## 				<oms:Connection startElement="wc1" startConnector="bus1" endElement="wc2" endConnector="bus2" delay="0.001000" alpha="0.300000" linearimpedance="100.000000" angularimpedance="0.000000" />
## 			</oms:Connections>
## 		</ssd:Annotation>
## 	</ssd:Annotations>
## </ssd:System>
##
## status:  [correct] ok
## info:    0 warnings
## info:    3 errors
## endResult
