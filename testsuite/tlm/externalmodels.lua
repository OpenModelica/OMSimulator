-- status: correct
-- teardown_command: rm -rf externalmodels-lua/
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

status = oms_setTempDirectory("./externalmodels-lua/")
printStatus(status, 0)

status = oms_newModel("model")
printStatus(status, 0)

status = oms_addSystem("model.tlm", oms_system_tlm)
printStatus(status, 0)

status = oms_addExternalModel("model.tlm.external", "resources/external.mo", "resources/startscript.sh")
printStatus(status, 0)

status = oms_addTLMBus("model.tlm.external.tlmbus", oms_tlm_domain_mechanical, 1, default)

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
-- 	<ssd:SimulationInformation>
-- 		<ssd:Annotations>
-- 			<ssd:Annotation type="org.openmodelica">
-- 				<oms:TlmMaster ip="" managerport="0" monitorport="0" />
-- 			</ssd:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:SimulationInformation>
-- 	<ssd:Elements>
-- 		<ssd:Component name="external" source="resources/external.mo">
-- 			<ssd:Annotations>
-- 				<ssd:Annotation type="org.openmodelica">
-- 					<oms:Bus name="tlmbus" type="tlm" domain="mechanical" dimensions="1" interpolation="none">
-- 						<oms:Signals />
-- 					</oms:Bus>
-- 				</ssd:Annotation>
-- 			</ssd:Annotations>
-- 			<ssd:SimulationInformation>
-- 				<ssd:Annotations>
-- 					<ssd:Annotation type="org.openmodelica">
-- 						<oms:ExternalModel startscript="resources/startscript.sh" />
-- 					</ssd:Annotation>
-- 				</ssd:Annotations>
-- 			</ssd:SimulationInformation>
-- 		</ssd:Component>
-- 	</ssd:Elements>
-- 	<ssd:Connectors />
-- 	<ssd:Connections />
-- </ssd:System>
--
-- status:  [correct] ok
-- endResult
