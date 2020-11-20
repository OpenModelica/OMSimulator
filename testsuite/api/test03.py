## status: correct
## teardown_command: rm -rf test03-py/ test_res.mat
## linux: yes
## mingw: no
## win: no
## mac: no


from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./test03-py/")

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



oms.newModel("test")
oms.addSystem("test.eoo", oms.system_wc)
oms.addSubModel("test.eoo.source", "../resources/Modelica.Blocks.Sources.Sine.fmu")

## save snapshot
src, status = oms.list("test")
print(src, flush=True)

## change model
oms.addSystem("test.eoo.goo", oms.system_sc)
oms.delete("test.eoo.source")
oms.addSubModel("test.eoo.source", "../resources/Modelica.Blocks.Sources.Constant.fmu")

## restore model from snapshot
status = oms.loadSnapshot("test", src)
printStatus(status, 0)

src, status = oms.list("test")
print(src, flush=True)

oms.instantiate("test")
oms.initialize("test")
oms.simulate("test")
oms.terminate("test")

oms.delete("test")


## Result:
## <?xml version="1.0"?>
## <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="test" version="1.0">
## 	<ssd:System name="eoo">
## 		<ssd:Elements>
## 			<ssd:Component name="source" type="application/x-fmu-sharedlibrary" source="resources/0001_source.fmu">
## 				<ssd:Connectors>
## 					<ssd:Connector name="y" kind="output">
## 						<ssc:Real />
## 						<ssd:ConnectorGeometry x="1.000000" y="0.500000" />
## 					</ssd:Connector>
## 					<ssd:Connector name="amplitude" kind="parameter">
## 						<ssc:Real />
## 					</ssd:Connector>
## 					<ssd:Connector name="freqHz" kind="parameter">
## 						<ssc:Real />
## 					</ssd:Connector>
## 					<ssd:Connector name="offset" kind="parameter">
## 						<ssc:Real />
## 					</ssd:Connector>
## 					<ssd:Connector name="phase" kind="parameter">
## 						<ssc:Real />
## 					</ssd:Connector>
## 					<ssd:Connector name="startTime" kind="parameter">
## 						<ssc:Real />
## 					</ssd:Connector>
## 				</ssd:Connectors>
## 			</ssd:Component>
## 		</ssd:Elements>
## 		<ssd:Annotations>
## 			<ssc:Annotation type="org.openmodelica">
## 				<oms:SimulationInformation>
## 					<oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
## 				</oms:SimulationInformation>
## 			</ssc:Annotation>
## 		</ssd:Annotations>
## 	</ssd:System>
## 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
## 		<ssd:Annotations>
## 			<ssc:Annotation type="org.openmodelica">
## 				<oms:SimulationInformation resultFile="test_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
## 			</ssc:Annotation>
## 		</ssd:Annotations>
## 	</ssd:DefaultExperiment>
## </ssd:SystemStructureDescription>
##
## info:    Delete source
## status:  [correct] ok
## <?xml version="1.0"?>
## <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="test" version="1.0">
## 	<ssd:System name="eoo">
## 		<ssd:Elements>
## 			<ssd:Component name="source" type="application/x-fmu-sharedlibrary" source="resources/0001_source.fmu">
## 				<ssd:Connectors>
## 					<ssd:Connector name="y" kind="output">
## 						<ssc:Real />
## 						<ssd:ConnectorGeometry x="1.000000" y="0.500000" />
## 					</ssd:Connector>
## 					<ssd:Connector name="amplitude" kind="parameter">
## 						<ssc:Real />
## 					</ssd:Connector>
## 					<ssd:Connector name="freqHz" kind="parameter">
## 						<ssc:Real />
## 					</ssd:Connector>
## 					<ssd:Connector name="offset" kind="parameter">
## 						<ssc:Real />
## 					</ssd:Connector>
## 					<ssd:Connector name="phase" kind="parameter">
## 						<ssc:Real />
## 					</ssd:Connector>
## 					<ssd:Connector name="startTime" kind="parameter">
## 						<ssc:Real />
## 					</ssd:Connector>
## 				</ssd:Connectors>
## 			</ssd:Component>
## 		</ssd:Elements>
## 		<ssd:Annotations>
## 			<ssc:Annotation type="org.openmodelica">
## 				<oms:SimulationInformation>
## 					<oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
## 				</oms:SimulationInformation>
## 			</ssc:Annotation>
## 		</ssd:Annotations>
## 	</ssd:System>
## 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
## 		<ssd:Annotations>
## 			<ssc:Annotation type="org.openmodelica">
## 				<oms:SimulationInformation resultFile="test_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
## 			</ssc:Annotation>
## 		</ssd:Annotations>
## 	</ssd:DefaultExperiment>
## </ssd:SystemStructureDescription>
##
## info:    Result file: test_res.mat (bufferSize=10)
## endResult
