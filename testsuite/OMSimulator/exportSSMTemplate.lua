-- status: correct
-- teardown_command: rm -rf exportssmtemplate_lua/ add1.ssm gain1.ssm template1.ssm
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./exportssmtemplate_lua/")


function readFile(filename)
  local f = assert(io.open(filename, "r"))
  local content = f:read("*all")
  print(content)
  f:close()
end


oms_newModel("test")

oms_addSystem("test.Root", oms_system_wc)

oms_addSubModel("test.Root.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms_addSubModel("test.Root.add", "../resources/Modelica.Blocks.Math.Add.fmu")

oms_exportSSMTemplate("test", "template1.ssm")
readFile("template1.ssm")

oms_exportSSMTemplate("test.Root.add", "add1.ssm")
readFile("add1.ssm")

oms_exportSSMTemplate("test.Root.Gain", "gain1.ssm")
readFile("gain1.ssm")


-- Result:
-- warning: [Gain: resources/0001_Gain.fmu] The FMU lists 0 initial unknowns but actually contains 1 initial unknowns as per the variable definitions.
-- info:    [Gain: resources/0001_Gain.fmu] The FMU contains bad initial unknowns. This might cause problems, e.g. wrong simulation results.
-- <?xml version="1.0" encoding="UTF-8"?>
-- <ssm:ParameterMapping xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" version="1.0">
-- 	<ssm:MappingEntry source="" target="add.u2" />
-- 	<ssm:MappingEntry source="" target="add.u1" />
-- 	<ssm:MappingEntry source="" target="add.k2" />
-- 	<ssm:MappingEntry source="" target="add.k1" />
-- 	<ssm:MappingEntry source="" target="Gain.u" />
-- 	<ssm:MappingEntry source="" target="Gain.k" />
-- </ssm:ParameterMapping>
--
-- <?xml version="1.0" encoding="UTF-8"?>
-- <ssm:ParameterMapping xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" version="1.0">
-- 	<ssm:MappingEntry source="" target="add.u2" />
-- 	<ssm:MappingEntry source="" target="add.u1" />
-- 	<ssm:MappingEntry source="" target="add.k2" />
-- 	<ssm:MappingEntry source="" target="add.k1" />
-- </ssm:ParameterMapping>
--
-- <?xml version="1.0" encoding="UTF-8"?>
-- <ssm:ParameterMapping xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" version="1.0">
-- 	<ssm:MappingEntry source="" target="Gain.u" />
-- 	<ssm:MappingEntry source="" target="Gain.k" />
-- </ssm:ParameterMapping>
--
-- info:    1 warnings
-- info:    0 errors
-- endResult
