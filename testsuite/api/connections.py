## status: correct
## teardown_command: rm -rf connections-py/
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

status = oms.setTempDirectory("./connections-py/")
printStatus(status, 0)

status = oms.newModel("model")
printStatus(status, 0)

status = oms.addSystem("model.wc", oms.system_wc)
printStatus(status, 0)

status = oms.addSystem("model.wc.sc1", oms.system_sc)
printStatus(status, 0)

status = oms.addSystem("model.wc.sc2", oms.system_sc)
printStatus(status, 0)

status = oms.addConnector("model.wc.sc1.u1", oms.input, oms.signal_type_real)
status = oms.addConnector("model.wc.sc1.u2", oms.input, oms.signal_type_real)
status = oms.addConnector("model.wc.sc1.y", oms.output, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnector("model.wc.sc2.y1", oms.output, oms.signal_type_real)
status = oms.addConnector("model.wc.sc2.y2", oms.output, oms.signal_type_integer)
status = oms.addConnector("model.wc.sc2.y3", oms.output, oms.signal_type_real)
printStatus(status, 0)

status = oms.addConnection("model.wc.sc1.u1", "model.wc.sc2.y1")
printStatus(status, 0)

## Connecting input to input (illegal)
status = oms.addConnection("model.wc.sc1.y", "model.wc.sc2.y3")
printStatus(status, 3)

## Connecting Real to Integer (illegal)
status = oms.addConnection("model.wc.sc1.u2", "model.wc.sc2.y2")
printStatus(status, 3)

## Connecting to already connected connector (illegal)
status = oms.addConnection("model.wc.sc1.u1", "model.wc.sc2.y3")
printStatus(status, 3)

src, status = oms.exportSnapshot("model.wc")
print(src, flush=True)

status = oms.deleteConnection("model.wc.sc1.u1", "model.wc.sc2.y1")
printStatus(status, 0)

src, status = oms.exportSnapshot("model.wc")
print(src, flush=True)

status = oms.delete("model")
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
## error:   [addConnection] Causality mismatch in connection: sc1.y -> sc2.y3
## status:  [correct] error
## error:   [addConnection] Type mismatch in connection: sc1.u2 -> sc2.y2
## status:  [correct] error
## error:   [addConnection] Connector sc1.u1 is already connected to sc2.y1
## status:  [correct] error
## <?xml version="1.0"?>
## <oms:snapshot>
##   <oms:file name="SystemStructure.ssd">
##     <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="model" version="1.0">
##       <ssd:System name="wc">
##         <ssd:Elements>
##           <ssd:System name="sc2">
##             <ssd:Connectors>
##               <ssd:Connector name="y1" kind="output">
##                 <ssc:Real />
##               </ssd:Connector>
##               <ssd:Connector name="y2" kind="output">
##                 <ssc:Integer />
##               </ssd:Connector>
##               <ssd:Connector name="y3" kind="output">
##                 <ssc:Real />
##               </ssd:Connector>
##             </ssd:Connectors>
##             <ssd:Annotations>
##               <ssc:Annotation type="org.openmodelica">
##                 <oms:Annotations>
##                   <oms:SimulationInformation>
##                     <oms:VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
##                   </oms:SimulationInformation>
##                 </oms:Annotations>
##               </ssc:Annotation>
##             </ssd:Annotations>
##           </ssd:System>
##           <ssd:System name="sc1">
##             <ssd:Connectors>
##               <ssd:Connector name="u1" kind="input">
##                 <ssc:Real />
##               </ssd:Connector>
##               <ssd:Connector name="u2" kind="input">
##                 <ssc:Real />
##               </ssd:Connector>
##               <ssd:Connector name="y" kind="output">
##                 <ssc:Real />
##               </ssd:Connector>
##             </ssd:Connectors>
##             <ssd:Annotations>
##               <ssc:Annotation type="org.openmodelica">
##                 <oms:Annotations>
##                   <oms:SimulationInformation>
##                     <oms:VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
##                   </oms:SimulationInformation>
##                 </oms:Annotations>
##               </ssc:Annotation>
##             </ssd:Annotations>
##           </ssd:System>
##         </ssd:Elements>
##         <ssd:Connections>
##           <ssd:Connection startElement="sc2" startConnector="y1" endElement="sc1" endConnector="u1" />
##         </ssd:Connections>
##         <ssd:Annotations>
##           <ssc:Annotation type="org.openmodelica">
##             <oms:Annotations>
##               <oms:SimulationInformation>
##                 <oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
##               </oms:SimulationInformation>
##             </oms:Annotations>
##           </ssc:Annotation>
##         </ssd:Annotations>
##       </ssd:System>
##       <ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
##         <ssd:Annotations>
##           <ssc:Annotation type="org.openmodelica">
##             <oms:Annotations>
##               <oms:SimulationInformation resultFile="model_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
##             </oms:Annotations>
##           </ssc:Annotation>
##         </ssd:Annotations>
##       </ssd:DefaultExperiment>
##     </ssd:SystemStructureDescription>
##   </oms:file>
## </oms:snapshot>
##
## status:  [correct] ok
## <?xml version="1.0"?>
## <oms:snapshot>
##   <oms:file name="SystemStructure.ssd">
##     <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="model" version="1.0">
##       <ssd:System name="wc">
##         <ssd:Elements>
##           <ssd:System name="sc2">
##             <ssd:Connectors>
##               <ssd:Connector name="y1" kind="output">
##                 <ssc:Real />
##               </ssd:Connector>
##               <ssd:Connector name="y2" kind="output">
##                 <ssc:Integer />
##               </ssd:Connector>
##               <ssd:Connector name="y3" kind="output">
##                 <ssc:Real />
##               </ssd:Connector>
##             </ssd:Connectors>
##             <ssd:Annotations>
##               <ssc:Annotation type="org.openmodelica">
##                 <oms:Annotations>
##                   <oms:SimulationInformation>
##                     <oms:VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
##                   </oms:SimulationInformation>
##                 </oms:Annotations>
##               </ssc:Annotation>
##             </ssd:Annotations>
##           </ssd:System>
##           <ssd:System name="sc1">
##             <ssd:Connectors>
##               <ssd:Connector name="u1" kind="input">
##                 <ssc:Real />
##               </ssd:Connector>
##               <ssd:Connector name="u2" kind="input">
##                 <ssc:Real />
##               </ssd:Connector>
##               <ssd:Connector name="y" kind="output">
##                 <ssc:Real />
##               </ssd:Connector>
##             </ssd:Connectors>
##             <ssd:Annotations>
##               <ssc:Annotation type="org.openmodelica">
##                 <oms:Annotations>
##                   <oms:SimulationInformation>
##                     <oms:VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
##                   </oms:SimulationInformation>
##                 </oms:Annotations>
##               </ssc:Annotation>
##             </ssd:Annotations>
##           </ssd:System>
##         </ssd:Elements>
##         <ssd:Annotations>
##           <ssc:Annotation type="org.openmodelica">
##             <oms:Annotations>
##               <oms:SimulationInformation>
##                 <oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
##               </oms:SimulationInformation>
##             </oms:Annotations>
##           </ssc:Annotation>
##         </ssd:Annotations>
##       </ssd:System>
##       <ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
##         <ssd:Annotations>
##           <ssc:Annotation type="org.openmodelica">
##             <oms:Annotations>
##               <oms:SimulationInformation resultFile="model_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
##             </oms:Annotations>
##           </ssc:Annotation>
##         </ssd:Annotations>
##       </ssd:DefaultExperiment>
##     </ssd:SystemStructureDescription>
##   </oms:file>
## </oms:snapshot>
##
## status:  [correct] ok
## info:    0 warnings
## info:    3 errors
## endResult
