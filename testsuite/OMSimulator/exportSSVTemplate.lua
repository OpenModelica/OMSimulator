-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./exportssvtemplate_lua/")

oms_newModel("test")

oms_addSystem("test.Root", oms_system_wc)

oms_addSubModel("test.Root.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms_addSubModel("test.Root.add", "../resources/Modelica.Blocks.Math.Add.fmu")

oms_exportSSVTemplate("test", "template.ssv")

local f = assert(io.open("template.ssv", "r"))
local content = f:read("*all")
print(content)
f:close()


-- Result:
-- <?xml version="1.0" encoding="UTF-8"?>
-- <ssv:ParameterSet xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" version="1.0" name="modelDescriptionStartValues">
-- 	<ssv:Parameters>
-- 		<ssv:Parameter name="add.u2">
-- 			<ssv:Real value="0" />
-- 		</ssv:Parameter>
-- 		<ssv:Parameter name="add.u1">
-- 			<ssv:Real value="0" />
-- 		</ssv:Parameter>
-- 		<ssv:Parameter name="add.k2">
-- 			<ssv:Real value="1" />
-- 		</ssv:Parameter>
-- 		<ssv:Parameter name="add.k1">
-- 			<ssv:Real value="1" />
-- 		</ssv:Parameter>
-- 		<ssv:Parameter name="Gain.u">
-- 			<ssv:Real value="0" />
-- 		</ssv:Parameter>
-- 		<ssv:Parameter name="Gain.k">
-- 			<ssv:Real value="1" />
-- 		</ssv:Parameter>
-- 	</ssv:Parameters>
-- </ssv:ParameterSet>
-- 
-- endResult
