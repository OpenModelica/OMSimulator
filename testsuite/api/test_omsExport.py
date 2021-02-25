## status: correct
## teardown_command: rm -rf test_omsExport-py/
## linux: yes
## linux32: yes
## mingw: yes
## win: no
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
  print("status:  [%s] %s" % (cmp, status), flush=True)

status = oms.newModel("model")
printStatus(status, 0)

status = oms.addSystem("model.sc", oms.system_sc)
printStatus(status, 0)

status = oms.setSignalFilter("model", "[AB]")
printStatus(status, 0)

(src, status) = oms.exportSnapshot("model")
printStatus(status, 0)
print(src, flush=True)

status = oms.removeSignalsFromResults("model", "A")
printStatus(status, 0)

status = oms.addSignalsToResults("model", "Y")
printStatus(status, 0)

(src, status) = oms.exportSnapshot("model")
printStatus(status, 0)
print(src, flush=True)

status= oms.export("model", "model.ssp")
printStatus(status, 0)


## Result:
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## <?xml version="1.0"?>
## <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="model" version="1.0">
## 	<ssd:System name="sc">
## 		<ssd:Annotations>
## 			<ssc:Annotation type="org.openmodelica">
## 				<oms:Annotations>
## 					<oms:SimulationInformation>
## 						<oms:VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
## 					</oms:SimulationInformation>
## 				</oms:Annotations>
## 			</ssc:Annotation>
## 		</ssd:Annotations>
## 	</ssd:System>
## 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
## 		<ssd:Annotations>
## 			<ssc:Annotation type="org.openmodelica">
## 				<oms:Annotations>
## 					<oms:SimulationInformation resultFile="model_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter="[AB]" />
## 				</oms:Annotations>
## 			</ssc:Annotation>
## 		</ssd:Annotations>
## 	</ssd:DefaultExperiment>
## </ssd:SystemStructureDescription>
##
## warning: [removeSignalsFromResults] is deprecated and [setSignalFilter] is the recommended API
## status:  [correct] ok
## warning: [addSignalsToResults] is deprecated and [setSignalFilter] is the recommended API
## status:  [correct] ok
## status:  [correct] ok
## <?xml version="1.0"?>
## <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="model" version="1.0">
## 	<ssd:System name="sc">
## 		<ssd:Annotations>
## 			<ssc:Annotation type="org.openmodelica">
## 				<oms:Annotations>
## 					<oms:SimulationInformation>
## 						<oms:VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
## 					</oms:SimulationInformation>
## 				</oms:Annotations>
## 			</ssc:Annotation>
## 		</ssd:Annotations>
## 	</ssd:System>
## 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
## 		<ssd:Annotations>
## 			<ssc:Annotation type="org.openmodelica">
## 				<oms:Annotations>
## 					<oms:SimulationInformation resultFile="model_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter="[AB]" />
## 				</oms:Annotations>
## 			</ssc:Annotation>
## 		</ssd:Annotations>
## 	</ssd:DefaultExperiment>
## </ssd:SystemStructureDescription>
##
## status:  [correct] ok
## info:    2 warnings
## info:    0 errors
## endResult
