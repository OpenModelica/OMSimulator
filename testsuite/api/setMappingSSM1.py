## status: correct
## teardown_command: rm mapping1.ssv mapping2.ssm
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, SSV, SSM

Settings.suppressPath = True

# This example creates a new SSV file and new SSM file set some parameter values and export it to filesystem

def read_file(filename):
  with open(filename, "r") as file:
    content = file.read()
    print(content)
    file.close()

ssv1 = SSV()
ssv1.setValue("Input1", 2.0)
ssv1.setValue("param1", 3.0)
ssv1.setValue("param2", 3)
ssv1.export("mapping1.ssv")
read_file("mapping1.ssv")

## set parameter mapping
ssm1 = SSM()
ssm1.map("Input1", "Add1.u1")
ssm1.map("Input1", "u2")
ssm1.map("Input1", "u3")
ssm1.map("param1", "k1")
ssm1.map("param2", "k2")
ssm1.export("mapping2.ssm")
read_file("mapping2.ssm")

## Result:
## <?xml version='1.0' encoding='utf-8'?>
## <ssv:ParameterSet xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" version="2.0" name="parameters">
##   <ssv:Parameters>
##     <ssv:Parameter name="Input1">
##       <ssv:Real value="2.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="param1">
##       <ssv:Real value="3.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="param2">
##       <ssv:Integer value="3"/>
##     </ssv:Parameter>
##   </ssv:Parameters>
## </ssv:ParameterSet>
##
## <?xml version='1.0' encoding='utf-8'?>
## <ssm:ParameterMapping xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" version="2.0">
##   <ssm:MappingEntry source="Input1" target="Add1.u1"/>
##   <ssm:MappingEntry source="Input1" target="u2"/>
##   <ssm:MappingEntry source="Input1" target="u3"/>
##   <ssm:MappingEntry source="param1" target="k1"/>
##   <ssm:MappingEntry source="param2" target="k2"/>
## </ssm:ParameterMapping>
##
## endResult
