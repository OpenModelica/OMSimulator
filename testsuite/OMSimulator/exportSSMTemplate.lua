-- status: correct
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

oms_exportSSMTemplate("test", "template.ssm")
readFile("template.ssm")

oms_exportSSMTemplate("test.Root.add", "add.ssm")
readFile("add.ssm")

oms_exportSSMTemplate("test.Root.Gain", "gain.ssm")
readFile("gain.ssm")


-- Result:
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
-- endResult
