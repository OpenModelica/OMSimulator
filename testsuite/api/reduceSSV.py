## status: correct
## teardown_command: rm -rf reducessv_01_py/
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: no


def readFile(filename):
  f = open(filename, "r")
  content = f.read()
  print(content)
  f:close()

from OMSimulator import OMSimulator
oms = OMSimulator()


oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./reducessv_01_py/")
oms.setWorkingDirectory("./reducessv_01_py/")

oms.newModel("model")

oms.reduceSSV("model", "../../resources/importParameterMapping/resources/import_parameter_mapping.ssv", "../../resources/importParameterMapping/resources/import_parameter_mapping.ssm")

readFile("../../resources/importParameterMapping/resources/import_parameter_mapping.ssv")
readFile("reduced.ssv")


## Result:
## <?xml version="1.0" encoding="UTF-8"?>
## <ssv:ParameterSet xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" version="2.0" name="parameters">
##     <ssv:Parameters>
##         <ssv:Parameter name="cosim_input">
##             <ssv:Real value="20" />
##         </ssv:Parameter>
##          <ssv:Parameter name="Input_3">
##             <ssv:Real value="50" />
##         </ssv:Parameter>
##         <ssv:Parameter name="cosim_parameters">
##             <ssv:Real value="-30" />
##         </ssv:Parameter>
##         <ssv:Parameter name="parameter_2">
##             <ssv:Real value="-40" />
##         </ssv:Parameter>
##     </ssv:Parameters>
## </ssv:ParameterSet>
##
## <?xml version="1.0" encoding="UTF-8"?>
## <ssv:ParameterSet
##   xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##   xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##   version="2.0"
##   name="reducedSSV">
##   <ssv:Parameters>
##     <ssv:Parameter
##       name="cosim_parameters">
##       <ssv:Real
##         value="-30" />
##     </ssv:Parameter>
##     <ssv:Parameter
##       name="cosim_input">
##       <ssv:Real
##         value="20" />
##     </ssv:Parameter>
##   </ssv:Parameters>
## </ssv:ParameterSet>
##
## endResult
