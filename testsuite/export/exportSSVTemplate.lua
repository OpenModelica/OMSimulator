-- status: correct
-- teardown_command: rm -rf exportSSVTemplate_lua/
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: yes
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./exportSSVTemplate_lua/")
oms_setWorkingDirectory("./exportSSVTemplate_lua/")

function readFile(filename)
  local f = assert(io.open(filename, "r"))
  local content = f:read("*all")
  print(content)
  f:close()
end

oms_newModel("exportSSVTemplate")

oms_addSystem("exportSSVTemplate.Root", oms_system_wc)

oms_addSubModel("exportSSVTemplate.Root.Gain", "../../resources/Modelica.Blocks.Math.Gain.fmu")
oms_setReal("exportSSVTemplate.Root.Gain.k", 27)

oms_addSubModel("exportSSVTemplate.Root.add", "../../resources/Modelica.Blocks.Math.Add.fmu")
oms_setReal("exportSSVTemplate.Root.add.k1", -20)

oms_exportSSVTemplate("exportSSVTemplate", "template.ssv")
readFile("template.ssv")

oms_exportSSVTemplate("exportSSVTemplate.Root.add", "add.ssv")
readFile("add.ssv")

oms_exportSSVTemplate("exportSSVTemplate.Root.Gain", "gain.ssv")
readFile("gain.ssv")

oms_delete("exportSSVTemplate")

-- Result:
-- <?xml version="1.0" encoding="UTF-8"?>
-- <ssv:ParameterSet
--   xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--   xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--   version="1.0"
--   name="modelDescriptionStartValues">
--   <ssv:Parameters>
--     <ssv:Parameter
--       name="add.u2">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="add.u1">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="add.k2">
--       <ssv:Real
--         value="1" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="add.k1">
--       <ssv:Real
--         value="-20" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="Gain.u">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="Gain.k">
--       <ssv:Real
--         value="27" />
--     </ssv:Parameter>
--   </ssv:Parameters>
-- </ssv:ParameterSet>
--
-- <?xml version="1.0" encoding="UTF-8"?>
-- <ssv:ParameterSet
--   xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--   xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--   version="1.0"
--   name="modelDescriptionStartValues">
--   <ssv:Parameters>
--     <ssv:Parameter
--       name="add.u2">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="add.u1">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="add.k2">
--       <ssv:Real
--         value="1" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="add.k1">
--       <ssv:Real
--         value="-20" />
--     </ssv:Parameter>
--   </ssv:Parameters>
-- </ssv:ParameterSet>
--
-- <?xml version="1.0" encoding="UTF-8"?>
-- <ssv:ParameterSet
--   xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--   xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--   version="1.0"
--   name="modelDescriptionStartValues">
--   <ssv:Parameters>
--     <ssv:Parameter
--       name="Gain.u">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="Gain.k">
--       <ssv:Real
--         value="27" />
--     </ssv:Parameter>
--   </ssv:Parameters>
-- </ssv:ParameterSet>
--
-- endResult
