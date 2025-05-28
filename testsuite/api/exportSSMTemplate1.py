## status: correct
## teardown_command: rm -rf exportSSMTemplate1.ssp exportSSMTemplate*.ssm
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings, Connector, Causality, SignalType

Settings.suppressPath = True

# This example creates a new SSP file with an FMU's instantiated as a component and
# and export SSM templates files to whole system or specific sub-system or component
# the source attribute in ssm template will be empty and should be set by user

def read_file(filename):
  with open(filename, "r") as file:
    content = file.read()
    print(content)
    file.close()

model = SSP()

## add top level system connector
model.activeVariant.system.addConnector(Connector('param1', Causality.parameter, SignalType.Real))
model.activeVariant.system.addConnector(Connector('input1', Causality.input, SignalType.Real))

model.addSystem(CRef('default', 'sub-system'))
## add top level sub-system connector
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('param2', Causality.parameter, SignalType.Real))
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('input2', Causality.input, SignalType.Real))


model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='resources/Add.fmu')
model.addResource('../resources/Modelica.Blocks.Math.Gain.fmu', new_name='resources/Gain.fmu')

model.addComponent(CRef('default', 'Add1'), 'resources/Add.fmu')
model.addComponent(CRef('default', 'Gain1'), 'resources/Gain.fmu')


model.export('exportSSMTemplate1.ssp')

model2 = SSP('exportSSMTemplate1.ssp')

## query whole ssp

model2.exportSSMTemplate(CRef("default"), 'exportSSMTemplate1.ssm')
print(f"Full System")
print("============")
read_file('exportSSMTemplate1.ssm')
## query sub-system
model2.exportSSMTemplate(CRef("default","sub-system"), 'exportSSMTemplate2.ssm')
print(f"Sub System")
print("============")
read_file('exportSSMTemplate2.ssm')
## query component Add1
model2.exportSSMTemplate(CRef("default", "Add1"), 'exportSSMTemplate3.ssm')
print(f"Component Add1")
print("===============")
read_file('exportSSMTemplate3.ssm')
## query component Gain1
model2.exportSSMTemplate(CRef("default", "Gain1"), 'exportSSMTemplate4.ssm')
print(f"Component Gain1")
print("===============")
read_file('exportSSMTemplate4.ssm')


## Result:
## Full System
## ============
## <?xml version='1.0' encoding='utf-8'?>
## <ssm:ParameterMapping xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" version="2.0">
##   <ssm:MappingEntry source="" target="param1"/>
##   <ssm:MappingEntry source="" target="input1"/>
##   <ssm:MappingEntry source="" target="sub-system.param2"/>
##   <ssm:MappingEntry source="" target="sub-system.input2"/>
##   <ssm:MappingEntry source="" target="Add1.u1"/>
##   <ssm:MappingEntry source="" target="Add1.u2"/>
##   <ssm:MappingEntry source="" target="Add1.y"/>
##   <ssm:MappingEntry source="" target="Add1.k1"/>
##   <ssm:MappingEntry source="" target="Add1.k2"/>
##   <ssm:MappingEntry source="" target="Gain1.u"/>
##   <ssm:MappingEntry source="" target="Gain1.y"/>
##   <ssm:MappingEntry source="" target="Gain1.k"/>
## </ssm:ParameterMapping>
##
## Sub System
## ============
## <?xml version='1.0' encoding='utf-8'?>
## <ssm:ParameterMapping xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" version="2.0">
##   <ssm:MappingEntry source="" target="param2"/>
##   <ssm:MappingEntry source="" target="input2"/>
## </ssm:ParameterMapping>
##
## Component Add1
## ===============
## <?xml version='1.0' encoding='utf-8'?>
## <ssm:ParameterMapping xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" version="2.0">
##   <ssm:MappingEntry source="" target="u1"/>
##   <ssm:MappingEntry source="" target="u2"/>
##   <ssm:MappingEntry source="" target="y"/>
##   <ssm:MappingEntry source="" target="k1"/>
##   <ssm:MappingEntry source="" target="k2"/>
## </ssm:ParameterMapping>
##
## Component Gain1
## ===============
## <?xml version='1.0' encoding='utf-8'?>
## <ssm:ParameterMapping xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" version="2.0">
##   <ssm:MappingEntry source="" target="u"/>
##   <ssm:MappingEntry source="" target="y"/>
##   <ssm:MappingEntry source="" target="k"/>
## </ssm:ParameterMapping>
##
## endResult
