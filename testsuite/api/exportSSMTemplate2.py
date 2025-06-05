## status: correct
## teardown_command: rm -rf startValuesSSVTemplate.ssm
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import FMU, Settings

Settings.suppressPath = True

# This example imports a fmu and exports parameters to ssm template


def read_file(filename):
  with open(filename, "r") as file:
    content = file.read()
    print(content)
    file.close()


fmu = FMU('../resources/Modelica.Blocks.Math.Add.fmu')

## export the start values in fmu to ssv template
fmu.exportSSMTemplate('startValuesSSVTemplate.ssm')
read_file('startValuesSSVTemplate.ssm')


## Result:
## <?xml version='1.0' encoding='utf-8'?>
## <ssm:ParameterMapping xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" version="2.0">
##   <ssm:MappingEntry source="" target="u1"/>
##   <ssm:MappingEntry source="" target="u2"/>
##   <ssm:MappingEntry source="" target="k1"/>
##   <ssm:MappingEntry source="" target="k2"/>
## </ssm:ParameterMapping>
##
## endResult
