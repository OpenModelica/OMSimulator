-- status: correct
-- teardown_command: rm -rf exportSSVTemplate_01_lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: yes
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./exportSSVTemplate_01_lua/")
oms_setWorkingDirectory("./exportSSVTemplate_01_lua/")

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

oms_addSubModel("exportSSVTemplate.Root.sine", "../../resources/Modelica.Blocks.Sources.Sine.fmu")

oms_exportSSVTemplate("exportSSVTemplate", "template.ssv")
readFile("template.ssv")

oms_exportSSVTemplate("exportSSVTemplate.Root.add", "add.ssv")
readFile("add.ssv")

oms_exportSSVTemplate("exportSSVTemplate.Root.Gain", "gain.ssv")
readFile("gain.ssv")

-- override units in sine.fmu
oms_setUnit("exportSSVTemplate.Root.sine.amplitude", "m")
oms_setUnit("exportSSVTemplate.Root.sine.phase", "m")

oms_exportSSVTemplate("exportSSVTemplate.Root.sine", "sine.ssv")
readFile("sine.ssv")

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
--       name="sine.startTime">
--       <ssv:Real
--         value="0"
--         unit="s" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine.phase">
--       <ssv:Real
--         value="0"
--         unit="rad" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine.offset">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine.freqHz">
--       <ssv:Real
--         value="1"
--         unit="Hz" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine.amplitude">
--       <ssv:Real
--         value="1" />
--     </ssv:Parameter>
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
--         value="27"
--         unit="1" />
--     </ssv:Parameter>
--   </ssv:Parameters>
--   <ssv:Units>
--     <ssc:Unit
--       name="s">
--       <ssc:BaseUnit
--         s="1" />
--     </ssc:Unit>
--     <ssc:Unit
--       name="rad">
--       <ssc:BaseUnit />
--     </ssc:Unit>
--     <ssc:Unit
--       name="Hz">
--       <ssc:BaseUnit
--         s="-1" />
--     </ssc:Unit>
--     <ssc:Unit
--       name="1">
--       <ssc:BaseUnit />
--     </ssc:Unit>
--   </ssv:Units>
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
--         value="27"
--         unit="1" />
--     </ssv:Parameter>
--   </ssv:Parameters>
--   <ssv:Units>
--     <ssc:Unit
--       name="1">
--       <ssc:BaseUnit />
--     </ssc:Unit>
--   </ssv:Units>
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
--       name="sine.startTime">
--       <ssv:Real
--         value="0"
--         unit="s" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine.phase">
--       <ssv:Real
--         value="0"
--         unit="m" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine.offset">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine.freqHz">
--       <ssv:Real
--         value="1"
--         unit="Hz" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine.amplitude">
--       <ssv:Real
--         value="1"
--         unit="m" />
--     </ssv:Parameter>
--   </ssv:Parameters>
--   <ssv:Units>
--     <ssc:Unit
--       name="s">
--       <ssc:BaseUnit
--         s="1" />
--     </ssc:Unit>
--     <ssc:Unit
--       name="m">
--       <ssc:BaseUnit />
--     </ssc:Unit>
--     <ssc:Unit
--       name="Hz">
--       <ssc:BaseUnit
--         s="-1" />
--     </ssc:Unit>
--   </ssv:Units>
-- </ssv:ParameterSet>
--
-- endResult
