-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./exportssvtemplate_lua/")


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

oms_exportSSVTemplate("test", "template.ssv")
readFile("template.ssv")

oms_exportSSVTemplate("test.Root.add", "add.ssv")
readFile("add.ssv")

oms_exportSSVTemplate("test.Root.Gain", "gain.ssv")
readFile("gain.ssv")


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
--         value="1" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="Gain.u">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="Gain.k">
--       <ssv:Real
--         value="1" />
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
--         value="1" />
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
--         value="1" />
--     </ssv:Parameter>
--   </ssv:Parameters>
-- </ssv:ParameterSet>
--
-- endResult
