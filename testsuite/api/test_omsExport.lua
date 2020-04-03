-- status: correct
-- teardown_command: rm -rf test_omsExport-lua/
-- linux: yes
-- linux32: yes
-- mingw: yes
-- win: yes
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./test_omsExport-lua/")

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

status = oms_newModel("model")
printStatus(status, 0)

status = oms_addSystem("model.sc", oms_system_sc)
printStatus(status, 0)

src, status = oms_list("model")
printStatus(status, 0)
print(src)

status= oms_export("model", "model.ssp")
printStatus(status, 0)

status = oms_export("model", "model.ssp")
printStatus(status, 0)

-- Result:
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- <?xml version="1.0"?>
-- <ssd:SystemStructureDescription name="model" version="1.0">
-- 	<ssd:System name="sc">
-- 		<ssd:Annotations>
-- 			<ssd:Annotation type="org.openmodelica">
-- 				<oms:SimulationInformation>
-- 					<VariableStepSolver description="cvode" absoluteTolerance="0.000100" relativeTolerance="0.000100" minimumStepSize="0.000100" maximumStepSize="0.100000" initialStepSize="0.000100" />
-- 				</oms:SimulationInformation>
-- 			</ssd:Annotation>
-- 		</ssd:Annotations>
-- 		<ssd:Elements />
-- 		<ssd:Connectors />
-- 		<ssd:Connections />
-- 	</ssd:System>
-- 	<ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000" />
-- </ssd:SystemStructureDescription>
--
-- status:  [correct] ok
-- status:  [correct] ok
-- endResult
