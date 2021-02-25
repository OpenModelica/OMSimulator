## status: correct
## teardown_command: rm -rf externalmodels-py
## linux: yes
## mingw: yes
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")

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

status = oms.setTempDirectory("./externalmodels-py/")
printStatus(status, 0)

status = oms.newModel("model")
printStatus(status, 0)

status = oms.addSystem("model.tlm", oms.system_tlm)
printStatus(status, 0)

status = oms.addExternalModel("model.tlm.external", "resources/external.mo", "resources/startscript.sh")
printStatus(status, 0)

status = oms.addTLMBus("model.tlm.external.tlmbus", oms.tlm_domain_mechanical, 1, oms.default)

src, status = oms.exportSnapshot("model.tlm")
print(src)

status = oms.delete("model")
printStatus(status, 0)

## Result:
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## <?xml version="1.0"?>
## <ssd:System name="tlm">
## 	<ssd:Elements>
## 		<ssd:Component name="external" source="resources/external.mo">
## 			<ssd:Annotations>
## 				<ssc:Annotation type="org.openmodelica">
## 					<oms:Annotations>
## 						<oms:Buses>
## 							<oms:Bus name="tlmbus" type="tlm" domain="mechanical" dimensions="1" interpolation="none">
## 								<oms:Signals />
## 							</oms:Bus>
## 						</oms:Buses>
## 						<oms:SimulationInformation>
## 							<oms:ExternalModel startscript="resources/startscript.sh" />
## 						</oms:SimulationInformation>
## 					</oms:Annotations>
## 				</ssc:Annotation>
## 			</ssd:Annotations>
## 		</ssd:Component>
## 	</ssd:Elements>
## 	<ssd:Annotations>
## 		<ssc:Annotation type="org.openmodelica">
## 			<oms:Annotations>
## 				<oms:SimulationInformation>
## 					<oms:TlmMaster ip="" managerport="0" monitorport="0" />
## 				</oms:SimulationInformation>
## 			</oms:Annotations>
## 		</ssc:Annotation>
## 	</ssd:Annotations>
## </ssd:System>
##
## status:  [correct] ok
## endResult
