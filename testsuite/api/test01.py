## status: correct
## teardown_command: rm -rf test01-py/
## linux: yes
## mingw: yes
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./test01-py/")

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

status = oms.addSystem("test", oms.system_tlm)
printStatus(status, 3)

status = oms.addSystem("test.foo", oms.system_wc)
printStatus(status, 0)

status = oms.addSystem("test.foo.goo", oms.system_sc)
printStatus(status, 0)

status = oms.addSystem("test.foo.hoo", oms.system_wc)
printStatus(status, 3)

src, status = oms.exportSnapshot("test")
print(src, flush=True)

src, status = oms.exportSnapshot("test.foo")
print(src, flush=True)

src, status = oms.exportSnapshot("test.foo.goo")
print(src, flush=True)

status = oms.newModel("test")
printStatus(status, 3)

status = oms.rename("test", "foo")
printStatus(status, 0)

status = oms.newModel("test")
printStatus(status, 0)

status = oms.delete("test")
printStatus(status, 0)

status = oms.delete("foo")
printStatus(status, 0)

status = oms.newModel("test")
printStatus(status, 0)

src, status = oms.exportSnapshot("test")
print(src, flush=True)

status = oms.delete("test")
printStatus(status, 0)

status = oms.delete("foo")
printStatus(status, 3)

## Result:
## status:  [correct] ok
## error:   [addSystem] Model "test" does not contain any system
## status:  [correct] error
## status:  [correct] ok
## status:  [correct] ok
## error:   [NewSystem] A WC system must be the root system or a subsystem of a TLM system.
## status:  [correct] error
## <?xml version="1.0"?>
## <oms:snapshot>
##   <oms:file name="SystemStructure.ssd">
##     <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="test" version="1.0">
##       <ssd:System name="foo">
##         <ssd:Elements>
##           <ssd:System name="goo">
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
##               <oms:SimulationInformation resultFile="test_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
##             </oms:Annotations>
##           </ssc:Annotation>
##         </ssd:Annotations>
##       </ssd:DefaultExperiment>
##     </ssd:SystemStructureDescription>
##   </oms:file>
## </oms:snapshot>
##
## error:   [exportSnapshot] "test.foo" is not a top level model
##
## error:   [exportSnapshot] "test.foo.goo" is not a top level model
##
## error:   [newModel] "test" already exists in the scope
## status:  [correct] error
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## status:  [correct] ok
## <?xml version="1.0"?>
## <oms:snapshot>
##   <oms:file name="SystemStructure.ssd">
##     <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="test" version="1.0">
##       <ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
##         <ssd:Annotations>
##           <ssc:Annotation type="org.openmodelica">
##             <oms:Annotations>
##               <oms:SimulationInformation resultFile="test_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
##             </oms:Annotations>
##           </ssc:Annotation>
##         </ssd:Annotations>
##       </ssd:DefaultExperiment>
##     </ssd:SystemStructureDescription>
##   </oms:file>
## </oms:snapshot>
##
## status:  [correct] ok
## error:   [deleteModel] Model "foo" does not exist in the scope
## status:  [correct] error
## info:    0 warnings
## info:    6 errors
## endResult
