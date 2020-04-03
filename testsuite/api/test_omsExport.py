## status: correct
## teardown_command: rm -rf test_omsExport-py/
## linux: yes
## linux32: yes
## mingw: yes
## win: yes
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./test_omsExport-py/")

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

status = oms.newModel("model")
printStatus(status, 0)

status = oms.addSystem("model.sc", oms.system_sc)
printStatus(status, 0)

src, status = oms.list("model")
printStatus(status, 0)
print(src)

status= oms.export("model", "model.ssp")
printStatus(status, 0)

status = oms.export("model", "model.ssp")
printStatus(status, 0)

## Result:
## status:  [correct] ok
## status:  [correct] ok
## status:  [wrong] <?xml version="1.0"?>
## <ssd:SystemStructureDescription name="model" version="1.0">
## 	<ssd:System name="sc">
## 		<ssd:Annotations>
## 			<ssd:Annotation type="org.openmodelica">
## 				<oms:SimulationInformation>
## 					<VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
## 				</oms:SimulationInformation>
## 			</ssd:Annotation>
## 		</ssd:Annotations>
## 		<ssd:Elements />
## 		<ssd:Connectors />
## 		<ssd:Connections />
## 	</ssd:System>
## 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000" />
## </ssd:SystemStructureDescription>
##
## 0
## status:  [correct] ok
## status:  [correct] ok
## endResult
