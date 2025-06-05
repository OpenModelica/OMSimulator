## status: correct
## teardown_command: rm -rf startValuesSSVTemplate.ssv
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import FMU, Settings

Settings.suppressPath = True

# This example imports a fmu and exports modeldescription start values to ssv template


def read_file(filename):
  with open(filename, "r") as file:
    content = file.read()
    print(content)
    file.close()


fmu = FMU('../resources/Modelica.Blocks.Math.Add.fmu')

## export the start values in fmu to ssv template
fmu.exportSSVTemplate('startValuesSSVTemplate.ssv')
read_file('startValuesSSVTemplate.ssv')


## Result:
## <?xml version='1.0' encoding='utf-8'?>
## <ssv:ParameterSet xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" version="2.0" name="parameters">
##   <ssv:Parameters>
##     <ssv:Parameter name="u1" description="Connector of Real input signal 1">
##       <ssv:Real value="0.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="u2" description="Connector of Real input signal 2">
##       <ssv:Real value="0.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="k1" description="Gain of input signal 1">
##       <ssv:Real value="1.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="k2" description="Gain of input signal 2">
##       <ssv:Real value="1.0"/>
##     </ssv:Parameter>
##   </ssv:Parameters>
## </ssv:ParameterSet>
##
## endResult
