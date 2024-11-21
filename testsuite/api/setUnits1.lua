-- status: correct
-- teardown_command: rm -rf setunits_01_lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: no
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./setunits_01_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.sine", "../resources/Modelica.Blocks.Sources.Sine.fmu")

-- add resources to submodule
oms_newResources("model.root.sine:sine.ssv")

oms_setReal("model.root.sine.phase", 27)
oms_setReal("model.root.sine.amplitude", -100)
oms_setReal("model.root.sine.freqHz", -300)

oms_setResultFile("model", "")

src, status = oms_exportSnapshot("model:resources/sine.ssv")
print(src)

-- change the unit to m
oms_setUnit("model.root.sine.phase", "m")
oms_setUnit("model.root.sine.amplitude", "m")

src, status = oms_exportSnapshot("model:resources/sine.ssv")
print(src)

oms_terminate("model")
oms_delete("model")



-- Result:
-- <?xml version="1.0"?>
-- <oms:snapshot
--   xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
--   partial="true">
--   <oms:file
--     name="resources/sine.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="phase">
--           <ssv:Real
--             value="27"
--             unit="rad" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="freqHz">
--           <ssv:Real
--             value="-300"
--             unit="Hz" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="amplitude">
--           <ssv:Real
--             value="-100" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--       <ssv:Units>
--         <ssc:Unit
--           name="rad">
--           <ssc:BaseUnit />
--         </ssc:Unit>
--         <ssc:Unit
--           name="Hz">
--           <ssc:BaseUnit
--             s="-1" />
--         </ssc:Unit>
--       </ssv:Units>
--     </ssv:ParameterSet>
--   </oms:file>
-- </oms:snapshot>
--
-- <?xml version="1.0"?>
-- <oms:snapshot
--   xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
--   partial="true">
--   <oms:file
--     name="resources/sine.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="phase">
--           <ssv:Real
--             value="27"
--             unit="m" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="freqHz">
--           <ssv:Real
--             value="-300"
--             unit="Hz" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="amplitude">
--           <ssv:Real
--             value="-100"
--             unit="m" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--       <ssv:Units>
--         <ssc:Unit
--           name="m">
--           <ssc:BaseUnit />
--         </ssc:Unit>
--         <ssc:Unit
--           name="Hz">
--           <ssc:BaseUnit
--             s="-1" />
--         </ssc:Unit>
--       </ssv:Units>
--     </ssv:ParameterSet>
--   </oms:file>
-- </oms:snapshot>
--
-- endResult
