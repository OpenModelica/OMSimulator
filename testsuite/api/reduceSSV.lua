-- status: correct
-- teardown_command: rm -rf reducessv_01_lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: no


function readFile(filename)
    local f = assert(io.open(filename, "r"))
    local content = f:read("*all")
    print(content)
    f:close()
  end

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./reducessv_01_lua/")
oms_setWorkingDirectory("./reducessv_01_lua/")

oms_newModel("model")

oms_reduceSSV("model", "../../resources/importParameterMapping/resources/import_parameter_mapping.ssv", "../../resources/importParameterMapping/resources/import_parameter_mapping.ssm")

readFile("../../resources/importParameterMapping/resources/import_parameter_mapping.ssv")
readFile("reduced.ssv")


-- Result:
-- <?xml version="1.0" encoding="UTF-8"?>
-- <ssv:ParameterSet xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" version="1.0" name="parameters">
--     <ssv:Parameters>
--         <ssv:Parameter name="cosim_input">
--             <ssv:Real value="20" />
--         </ssv:Parameter>
--          <ssv:Parameter name="Input_3">
--             <ssv:Real value="50" />
--         </ssv:Parameter>
--         <ssv:Parameter name="cosim_parameters">
--             <ssv:Real value="-30" />
--         </ssv:Parameter>
--         <ssv:Parameter name="parameter_2">
--             <ssv:Real value="-40" />
--         </ssv:Parameter>
--     </ssv:Parameters>
-- </ssv:ParameterSet>
--
-- <?xml version="1.0" encoding="UTF-8"?>
-- <ssv:ParameterSet
--   xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--   xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--   version="1.0"
--   name="reducedSSV">
--   <ssv:Parameters>
--     <ssv:Parameter
--       name="cosim_parameters">
--       <ssv:Real
--         value="-30" />
--     </ssv:Parameter>
--     <ssv:Parameter
--       name="cosim_input">
--       <ssv:Real
--         value="20" />
--     </ssv:Parameter>
--   </ssv:Parameters>
-- </ssv:ParameterSet>
--
-- endResult
