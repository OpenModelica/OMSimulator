## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: yes

from OMSimulator import SSP, CRef, Settings, Connector, Causality, SignalType

Settings.suppressPath = True

# This example creates a new SSP file with an FMU instantiated as a component and
# set parameter values from the root level and export the start values to ssv file

def read_file(filename):
  with open(filename, "r") as file:
    content = file.read()
    print(content)
    file.close()

model = SSP()

## add top level system connector
model.activeVariant.system.addConnector(Connector('param1', Causality.parameter, SignalType.Real))
model.activeVariant.system.addConnector(Connector('input1', Causality.input, SignalType.Real))

model.setValue(CRef('default','param1'), 200.0)
model.setValue(CRef('default','input1'), 300.0)

model.addSystem(CRef('default', 'sub-system'))
## add top level sub-system connector
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('param2', Causality.parameter, SignalType.Real))
model.activeVariant.system.elements[CRef('sub-system')].addConnector(Connector('input2', Causality.input, SignalType.Real))
model.setValue(CRef('default', 'sub-system', 'param2'), 400.0)
model.setValue(CRef('default', 'sub-system', 'input2'), 500.0)

model.addResource('../resources/Modelica.Blocks.Math.Add.fmu', new_name='Add.fmu')
model.addComponent(CRef('default', 'Add1'), 'Add.fmu')
model.addComponent(CRef('default', 'Add2'), 'Add.fmu')

## query whole ssp
model.exportSSVTemplate(CRef("default"), 'exportSSVTemplate_1.ssv')
read_file('exportSSVTemplate_1.ssv')
## query sub-system
model.exportSSVTemplate(CRef("default","sub-system"), 'exportSSVTemplate_2.ssv')
read_file('exportSSVTemplate_2.ssv')
## query component Add1
model.exportSSVTemplate(CRef("default", "Add1"), 'exportSSVTemplate_3.ssv')
read_file('exportSSVTemplate_3.ssv')
## query component Add2
model.exportSSVTemplate(CRef("default", "Add2"), 'exportSSVTemplate_4.ssv')
read_file('exportSSVTemplate_4.ssv')


## Result:
## <?xml version='1.0' encoding='utf-8'?>
## <ssv:ParameterSet xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" version="2.0" name="parameters">
##   <ssv:Parameters>
##     <ssv:Parameter name="param1">
##       <ssv:Real value="200.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="input1">
##       <ssv:Real value="300.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="sub-system.param2">
##       <ssv:Real value="400.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="sub-system.input2">
##       <ssv:Real value="500.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="Add1.u1" description="Connector of Real input signal 1">
##       <ssv:Real value="0.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="Add1.u2" description="Connector of Real input signal 2">
##       <ssv:Real value="0.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="Add1.k1" description="Gain of input signal 1">
##       <ssv:Real value="1.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="Add1.k2" description="Gain of input signal 2">
##       <ssv:Real value="1.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="Add2.u1" description="Connector of Real input signal 1">
##       <ssv:Real value="0.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="Add2.u2" description="Connector of Real input signal 2">
##       <ssv:Real value="0.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="Add2.k1" description="Gain of input signal 1">
##       <ssv:Real value="1.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="Add2.k2" description="Gain of input signal 2">
##       <ssv:Real value="1.0"/>
##     </ssv:Parameter>
##   </ssv:Parameters>
## </ssv:ParameterSet>
##
## <?xml version='1.0' encoding='utf-8'?>
## <ssv:ParameterSet xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" version="2.0" name="parameters">
##   <ssv:Parameters>
##     <ssv:Parameter name="param2">
##       <ssv:Real value="400.0"/>
##     </ssv:Parameter>
##     <ssv:Parameter name="input2">
##       <ssv:Real value="500.0"/>
##     </ssv:Parameter>
##   </ssv:Parameters>
## </ssv:ParameterSet>
##
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
