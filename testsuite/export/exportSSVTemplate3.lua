-- status: correct
-- teardown_command: rm -rf exportSSVTemplate_03_lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: no
-- asan: yes

function readFile(filename)
    local f = assert(io.open(filename, "r"))
    local content = f:read("*all")
    print(content)
    f:close()
end


oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./exportSSVTemplate_03_lua/")
oms_setWorkingDirectory("./exportSSVTemplate_03_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.sine", "../../resources/Modelica.Blocks.Sources.Sine.fmu")
oms_addSubModel("model.root.gain", "../../resources/Modelica.Blocks.Math.Gain.fmu")

-- add resources to submodule
oms_newResources("model.root.sine:sine.ssv")
oms_newResources("model.root.gain:gain.ssv")

oms_setReal("model.root.sine.phase", 27)
oms_setReal("model.root.sine.amplitude", -100)
oms_setReal("model.root.sine.f", -300)
oms_setReal("model.root.gain.k", -200)

oms_addSystem("model.root.subsystem", oms_system_sc)

oms_addSubModel("model.root.subsystem.sine1", "../../resources/Modelica.Blocks.Sources.Sine.fmu")
oms_addSubModel("model.root.subsystem.gain1", "../../resources/Modelica.Blocks.Math.Gain.fmu")

oms_newResources("model.root.subsystem.sine1:sine1.ssv")
oms_newResources("model.root.subsystem.gain1:gain1.ssv")

oms_setReal("model.root.subsystem.sine1.phase", 50)
oms_setReal("model.root.subsystem.sine1.amplitude", 60)
oms_setReal("model.root.subsystem.sine1.f", 70)
oms_setReal("model.root.subsystem.gain1.k", -150)

oms_setResultFile("model", "")

-- query whole model or top level system (e.g) model or model.root
oms_exportSSVTemplate("model", "root.ssv")
readFile("root.ssv")

-- query single component in top level system
oms_exportSSVTemplate("model.root.sine", "sine.ssv")
readFile("sine.ssv")

-- query single component in top level system
oms_exportSSVTemplate("model.root.gain", "gain.ssv")
readFile("gain.ssv")

-- query entire subsystem
oms_exportSSVTemplate("model.root.subsystem", "subsytem.ssv")
readFile("subsytem.ssv")

-- query single component in subsystem
oms_exportSSVTemplate("model.root.subsystem.sine1", "sine1.ssv")
readFile("sine1.ssv")

-- query single component in subsystem
oms_exportSSVTemplate("model.root.subsystem.gain1", "gain1.ssv")
readFile("gain1.ssv")

oms_terminate("model")
oms_delete("model")


-- Result:
-- <?xml version="1.0" encoding="UTF-8"?>
-- <ssv:ParameterSet
--   xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--   xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--   version="2.0"
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
--         value="27"
--         unit="rad" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine.offset">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine.f">
--       <ssv:Real
--         value="-300"
--         unit="Hz" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine.amplitude">
--       <ssv:Real
--         value="-100" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="gain.u">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="gain.k">
--       <ssv:Real
--         value="-200"
--         unit="1" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine1.startTime">
--       <ssv:Real
--         value="0"
--         unit="s" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine1.phase">
--       <ssv:Real
--         value="50"
--         unit="rad" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine1.offset">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine1.f">
--       <ssv:Real
--         value="70"
--         unit="Hz" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine1.amplitude">
--       <ssv:Real
--         value="60" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="gain1.u">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="gain1.k">
--       <ssv:Real
--         value="-150"
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
--   version="2.0"
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
--         value="27"
--         unit="rad" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine.offset">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine.f">
--       <ssv:Real
--         value="-300"
--         unit="Hz" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine.amplitude">
--       <ssv:Real
--         value="-100" />
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
--   </ssv:Units>
-- </ssv:ParameterSet>
--
-- <?xml version="1.0" encoding="UTF-8"?>
-- <ssv:ParameterSet
--   xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--   xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--   version="2.0"
--   name="modelDescriptionStartValues">
--   <ssv:Parameters>
--     <ssv:Parameter
--       name="gain.u">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="gain.k">
--       <ssv:Real
--         value="-200"
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
--   version="2.0"
--   name="modelDescriptionStartValues">
--   <ssv:Parameters>
--     <ssv:Parameter
--       name="sine1.startTime">
--       <ssv:Real
--         value="0"
--         unit="s" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine1.phase">
--       <ssv:Real
--         value="50"
--         unit="rad" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine1.offset">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine1.f">
--       <ssv:Real
--         value="70"
--         unit="Hz" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine1.amplitude">
--       <ssv:Real
--         value="60" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="gain1.u">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="gain1.k">
--       <ssv:Real
--         value="-150"
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
--   version="2.0"
--   name="modelDescriptionStartValues">
--   <ssv:Parameters>
--     <ssv:Parameter
--       name="sine1.startTime">
--       <ssv:Real
--         value="0"
--         unit="s" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine1.phase">
--       <ssv:Real
--         value="50"
--         unit="rad" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine1.offset">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine1.f">
--       <ssv:Real
--         value="70"
--         unit="Hz" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="sine1.amplitude">
--       <ssv:Real
--         value="60" />
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
--   </ssv:Units>
-- </ssv:ParameterSet>
--
-- <?xml version="1.0" encoding="UTF-8"?>
-- <ssv:ParameterSet
--   xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--   xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--   version="2.0"
--   name="modelDescriptionStartValues">
--   <ssv:Parameters>
--     <ssv:Parameter
--       name="gain1.u">
--       <ssv:Real
--         value="0" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="gain1.k">
--       <ssv:Real
--         value="-150"
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
-- endResult
