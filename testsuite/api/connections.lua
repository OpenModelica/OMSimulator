-- status: correct
-- teardown_command: rm -rf connections-lua/
-- linux: yes
-- mingw: yes
-- win: yes
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")

function printStatus(status, expected)
  cmp = ""
  if status == expected then
    cmp = "correct"
  else
    cmp = "wrong"
  end

  if 0 == status then
    status = "ok"
  elseif 1 == status then
    status = "warning"
  elseif 3 == status then
    status = "error"
  end
  print("status:  [" .. cmp .. "] " .. status)
end

status = oms_setTempDirectory("./connections-lua/")
printStatus(status, 0)

status = oms_newModel("model")
printStatus(status, 0)

status = oms_addSystem("model.wc", oms_system_wc)
printStatus(status, 0)

status = oms_addSystem("model.wc.sc1", oms_system_sc)
printStatus(status, 0)

status = oms_addSystem("model.wc.sc2", oms_system_sc)
printStatus(status, 0)

status = oms_addConnector("model.wc.sc1.u1", input, oms_signal_type_real)
status = oms_addConnector("model.wc.sc1.u2", input, oms_signal_type_real)
status = oms_addConnector("model.wc.sc1.y", output, oms_signal_type_real)
printStatus(status, 0)

status = oms_addConnector("model.wc.sc2.y1", output, oms_signal_type_real)
status = oms_addConnector("model.wc.sc2.y2", output, oms_signal_type_integer)
status = oms_addConnector("model.wc.sc2.y3", output, oms_signal_type_real)
printStatus(status, 0)

status = oms_addConnection("model.wc.sc1.u1", "model.wc.sc2.y1")
printStatus(status, 0)

--Connecting input to input (illegal)
status = oms_addConnection("model.wc.sc1.y", "model.wc.sc2.y3")
printStatus(status, 3)

--Connecting Real to Integer (illegal)
status = oms_addConnection("model.wc.sc1.u2", "model.wc.sc2.y2")
printStatus(status, 3)

--Connecting to already connected connector (illegal)
status = oms_addConnection("model.wc.sc1.u1", "model.wc.sc2.y3")
printStatus(status, 3)

src, status = oms_list("model.wc")
print(src)

status = oms_deleteConnection("model.wc.sc1.u1", "model.wc.sc2.y1")
printStatus(status, 0)

src, status = oms_list("model.wc")
print(src)

status = oms_delete("model")
printStatus(status, 0)

-- Result:
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- error:   [addConnection] Causality mismatch in connection: sc1.y -> sc2.y3
-- status:  [correct] error
-- error:   [addConnection] Type mismatch in connection: sc1.u2 -> sc2.y2
-- status:  [correct] error
-- error:   [addConnection] Connector is already connected: sc1.u1
-- status:  [correct] error
-- <?xml version="1.0"?>
-- <ssd:System name="wc">
-- 	<ssd:SimulationInformation>
-- 		<FixedStepMaster description="oms-ma" stepSize="0.100000" />
-- 	</ssd:SimulationInformation>
-- 	<ssd:Elements>
-- 		<ssd:System name="sc2">
-- 			<ssd:SimulationInformation>
-- 				<VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
-- 			</ssd:SimulationInformation>
-- 			<ssd:Elements />
-- 			<ssd:Connectors>
-- 				<ssd:Connector name="y1" kind="output" type="Real" />
-- 				<ssd:Connector name="y2" kind="output" type="Integer" />
-- 				<ssd:Connector name="y3" kind="output" type="Real" />
-- 			</ssd:Connectors>
-- 			<ssd:Connections />
-- 		</ssd:System>
-- 		<ssd:System name="sc1">
-- 			<ssd:SimulationInformation>
-- 				<VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
-- 			</ssd:SimulationInformation>
-- 			<ssd:Elements />
-- 			<ssd:Connectors>
-- 				<ssd:Connector name="u1" kind="input" type="Real" />
-- 				<ssd:Connector name="u2" kind="input" type="Real" />
-- 				<ssd:Connector name="y" kind="output" type="Real" />
-- 			</ssd:Connectors>
-- 			<ssd:Connections />
-- 		</ssd:System>
-- 	</ssd:Elements>
-- 	<ssd:Connectors />
-- 	<ssd:Connections>
-- 		<ssd:Connection startElement="sc2" startConnector="y1" endElement="sc1" endConnector="u1" />
-- 	</ssd:Connections>
-- </ssd:System>
--
-- status:  [correct] ok
-- <?xml version="1.0"?>
-- <ssd:System name="wc">
-- 	<ssd:SimulationInformation>
-- 		<FixedStepMaster description="oms-ma" stepSize="0.100000" />
-- 	</ssd:SimulationInformation>
-- 	<ssd:Elements>
-- 		<ssd:System name="sc2">
-- 			<ssd:SimulationInformation>
-- 				<VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
-- 			</ssd:SimulationInformation>
-- 			<ssd:Elements />
-- 			<ssd:Connectors>
-- 				<ssd:Connector name="y1" kind="output" type="Real" />
-- 				<ssd:Connector name="y2" kind="output" type="Integer" />
-- 				<ssd:Connector name="y3" kind="output" type="Real" />
-- 			</ssd:Connectors>
-- 			<ssd:Connections />
-- 		</ssd:System>
-- 		<ssd:System name="sc1">
-- 			<ssd:SimulationInformation>
-- 				<VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
-- 			</ssd:SimulationInformation>
-- 			<ssd:Elements />
-- 			<ssd:Connectors>
-- 				<ssd:Connector name="u1" kind="input" type="Real" />
-- 				<ssd:Connector name="u2" kind="input" type="Real" />
-- 				<ssd:Connector name="y" kind="output" type="Real" />
-- 			</ssd:Connectors>
-- 			<ssd:Connections />
-- 		</ssd:System>
-- 	</ssd:Elements>
-- 	<ssd:Connectors />
-- 	<ssd:Connections />
-- </ssd:System>
--
-- status:  [correct] ok
-- info:    0 warnings
-- info:    3 errors
-- endResult
