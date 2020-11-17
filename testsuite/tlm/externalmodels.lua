-- status: correct
-- teardown_command: rm -rf externalmodels-lua/
-- linux: yes
-- mingw: yes
-- win: yes
-- mac: no

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

status = oms_setTempDirectory("./externalmodels-lua/")
printStatus(status, 0)

status = oms_newModel("model")
printStatus(status, 0)

status = oms_addSystem("model.tlm", oms_system_tlm)
printStatus(status, 0)

status = oms_addExternalModel("model.tlm.external", "resources/external.mo", "resources/startscript.sh")
printStatus(status, 0)

status = oms_addTLMBus("model.tlm.external.tlmbus", oms_tlm_domain_mechanical, 1, oms_tlm_no_interpolation)

src, status = oms_list("model.tlm")
print(src)

status = oms_delete("model")
printStatus(status, 0)

-- Result:
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- <?xml version="1.0"?>
-- <ssd:System name="tlm">
-- 	<ssd:Annotations>
-- 		<ssc:Annotation type="org.openmodelica">
-- 			<oms:SimulationInformation>
-- 				<oms:TlmMaster ip="" managerport="0" monitorport="0" />
-- 			</oms:SimulationInformation>
-- 		</ssc:Annotation>
-- 	</ssd:Annotations>
-- 	<ssd:Elements>
-- 		<ssd:Component name="external" source="resources/external.mo">
-- 			<ssd:Annotations>
-- 				<ssc:Annotation type="org.openmodelica">
-- 					<oms:Bus name="tlmbus" type="tlm" domain="mechanical" dimensions="1" interpolation="none">
-- 						<oms:Signals />
-- 					</oms:Bus>
-- 					<oms:SimulationInformation>
-- 						<oms:ExternalModel startscript="resources/startscript.sh" />
-- 					</oms:SimulationInformation>
-- 				</ssc:Annotation>
-- 			</ssd:Annotations>
-- 		</ssd:Component>
-- 	</ssd:Elements>
-- </ssd:System>
--
-- status:  [correct] ok
-- endResult
