## status: correct
## teardown_command: rm -rf test03py/ test03py_res.mat
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: no


from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./test03py/")

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



oms.newModel("test03py")
oms.addSystem("test03py.eoo", oms.system_wc)
oms.addSubModel("test03py.eoo.source", "../resources/Modelica.Blocks.Sources.Sine.fmu")

## save snapshot
src, status = oms.list("test03py")
print(src, flush=True)

## change model
oms.addSystem("test03py.eoo.goo", oms.system_sc)
oms.delete("test03py.eoo.source")
oms.addSubModel("test03py.eoo.source", "../resources/Modelica.Blocks.Sources.Constant.fmu")

## restore model from snapshot
newCref, status = oms.loadSnapshot("test03py", src)
printStatus(status, 0)

src, status = oms.list("test03py")
print(src, flush=True)

oms.instantiate("test03py")
oms.initialize("test03py")
oms.simulate("test03py")
oms.terminate("test03py")

oms.delete("test03py")


## Result:
## <?xml version="1.0"?>
## <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="test03py" version="2.0">
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
## 					<ssd:Connector name="f" kind="parameter">
## 						<ssc:Real unit="Hz" />
## 					</ssd:Connector>
## 					<ssd:Connector name="offset" kind="parameter">
## 						<ssc:Real />
## 					</ssd:Connector>
## 					<ssd:Connector name="phase" kind="parameter">
## 						<ssc:Real unit="rad" />
## 					</ssd:Connector>
## 					<ssd:Connector name="startTime" kind="parameter">
## 						<ssc:Real unit="s" />
## 					</ssd:Connector>
## 				</ssd:Connectors>
## 			</ssd:Component>
## 		</ssd:Elements>
## 		<ssd:Annotations>
## 			<ssc:Annotation type="org.openmodelica">
## 				<oms:Annotations>
## 					<oms:SimulationInformation>
## 						<oms:FixedStepMaster description="oms-ma" stepSize="0.001000" relativeTolerance="0.000100" />
## 					</oms:SimulationInformation>
## 				</oms:Annotations>
## 			</ssc:Annotation>
## 		</ssd:Annotations>
## 	</ssd:System>
## 	<ssd:Units>
## 		<ssc:Unit name="Hz">
## 			<ssc:BaseUnit s="-1" />
## 		</ssc:Unit>
## 		<ssc:Unit name="rad">
## 			<ssc:BaseUnit />
## 		</ssc:Unit>
## 		<ssc:Unit name="s">
## 			<ssc:BaseUnit s="1" />
## 		</ssc:Unit>
## 	</ssd:Units>
## 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
## 		<ssd:Annotations>
## 			<ssc:Annotation type="org.openmodelica">
## 				<oms:Annotations>
## 					<oms:SimulationInformation resultFile="test03py_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter="resources/signalFilter.xml" />
## 				</oms:Annotations>
## 			</ssc:Annotation>
## 		</ssd:Annotations>
## 	</ssd:DefaultExperiment>
## </ssd:SystemStructureDescription>
##
## error:   [getResourceNode] Failed to find node "resources/signalFilter.xml"
## status:  [correct] ok
## <?xml version="1.0"?>
## <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="test03py" version="2.0">
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
## 					<ssd:Connector name="f" kind="parameter">
## 						<ssc:Real unit="Hz" />
## 					</ssd:Connector>
## 					<ssd:Connector name="offset" kind="parameter">
## 						<ssc:Real />
## 					</ssd:Connector>
## 					<ssd:Connector name="phase" kind="parameter">
## 						<ssc:Real unit="rad" />
## 					</ssd:Connector>
## 					<ssd:Connector name="startTime" kind="parameter">
## 						<ssc:Real unit="s" />
## 					</ssd:Connector>
## 				</ssd:Connectors>
## 			</ssd:Component>
## 		</ssd:Elements>
## 		<ssd:Annotations>
## 			<ssc:Annotation type="org.openmodelica">
## 				<oms:Annotations>
## 					<oms:SimulationInformation>
## 						<oms:FixedStepMaster description="oms-ma" stepSize="0.001000" relativeTolerance="0.000100" />
## 					</oms:SimulationInformation>
## 				</oms:Annotations>
## 			</ssc:Annotation>
## 		</ssd:Annotations>
## 	</ssd:System>
## 	<ssd:Units>
## 		<ssc:Unit name="Hz">
## 			<ssc:BaseUnit s="-1" />
## 		</ssc:Unit>
## 		<ssc:Unit name="rad">
## 			<ssc:BaseUnit />
## 		</ssc:Unit>
## 		<ssc:Unit name="s">
## 			<ssc:BaseUnit s="1" />
## 		</ssc:Unit>
## 	</ssd:Units>
## 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
## 		<ssd:Annotations>
## 			<ssc:Annotation type="org.openmodelica">
## 				<oms:Annotations>
## 					<oms:SimulationInformation resultFile="test03py_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter="resources/signalFilter.xml" />
## 				</oms:Annotations>
## 			</ssc:Annotation>
## 		</ssd:Annotations>
## 	</ssd:DefaultExperiment>
## </ssd:SystemStructureDescription>
##
## info:    Result file: test03py_res.mat (bufferSize=10)
## info:    0 warnings
## info:    1 errors
## endResult
