## status: correct
## teardown_command: rm -rf embrace2_res.mat
## linux: yes
## ucrt64: no
## win: no
## mac: no

from OMSimulator import SSP, Settings, CRef

Settings.suppressPath = True

model = SSP('../resources/embrace_TwoConf.ssp')
model.addResource("../resources/embrace_TwoConf/CONOPS.csv", new_name = "resources/CONOPS.csv")

model.removeSSVReference(CRef('root'), 'resources/ECS_Conf2.ssv')
model.addSSVReference(CRef('root'), 'resources/ECS_Conf1.ssv', 'resources/ECS_HW.ssm')

model.addComponent(CRef('root', 'BC'), "resources/CONOPS.csv")

model.setValue(CRef('root', 'ECS_HW', 'eCS.MaxCoolPower.k'), 5)
model.setValue(CRef('root', 'ECS_HW', 'coolinPackAir.looptype'), 2)
model.setValue(CRef('root', 'ECS_HW', 'phSrc.looptype'), 2)
model.setValue(CRef('root', 'ECS_HW', 'pipeB.looptype'), 2)
model.setValue(CRef('root', 'ECS_HW', 'pipeA.looptype'), 2)


#----------------- Connections to BC files -----------------
#BC -> Consumer
model.addConnection(CRef('root', 'Consumer', 'Alt'), CRef('root', 'BC', 'Alt'))
model.addConnection(CRef('root', 'Consumer', 'Mach'), CRef('root', 'BC', 'Mach'))
model.addConnection(CRef('root', 'Consumer', 'heatLoad'), CRef('root', 'BC', 'heatLoad'))

#BC -> ECS_HW
model.addConnection(CRef('root', 'ECS_HW', 'Alt'), CRef('root', 'BC', 'Alt'))
model.addConnection(CRef('root', 'ECS_HW', 'Mach'), CRef('root', 'BC', 'Mach'))

#BC -> ECS_SW
model.addConnection(CRef('root', 'BC', 'Aircraft_state'), CRef('root', 'AdaptionUnit.Aircraft_State'))

#BC -> Atmos
model.addConnection(CRef('root', 'Atmos', 'Alt'), CRef('root', 'BC', 'Alt'))
model.addConnection(CRef('root', 'Atmos', 'Ma'), CRef('root', 'BC', 'Mach'))


instantiated_model = model.instantiate() ## internally generate the json file and also set the model state like virgin,

## simulation settings
instantiated_model.setResultFile("embrace2_res.mat")
instantiated_model.setStopTime(12.0)
instantiated_model.setStepSize(1e-3)
instantiated_model.setLoggingInterval(1)

instantiated_model.initialize()
print("info:    Initialize:")

print(f"info:      embrace.root.ECS_HW.coolinPackAir.looptype:  {instantiated_model.getValue(CRef('root', 'ECS_HW', 'coolinPackAir.looptype'))}")
print(f"info:      embrace.root.ECS_HW.eCS.MaxCoolPower.k    :  {instantiated_model.getValue(CRef('root', 'ECS_HW', 'eCS.MaxCoolPower.k'))}")
print(f"info:      embrace.root.ECS_HW.pipeC.L               :  {instantiated_model.getValue(CRef('root', 'ECS_HW', 'pipeC.L'))}")
print(f"info:      embrace.root.ECS_HW.pipeB.L               :  {instantiated_model.getValue(CRef('root', 'ECS_HW', 'pipeB.L'))}")
print(f"info:      embrace.root.ECS_HW.pipeA.L               :  {instantiated_model.getValue(CRef('root', 'ECS_HW', 'pipeA.L'))}")

instantiated_model.simulate()
instantiated_model.terminate()
instantiated_model.delete()

## Result:
## UserWarning: Failed to validate ECS_Conf2.ssv against schemafile SystemStructureParameterValues.xsd
## ERROR (line 2, column 0): Element '{http://ssp-standard.org/SSP1/SystemStructureParameterValues}ParameterSet': Character content other than whitespace is not allowed because the content type is 'element-only'.
## ERROR (line 1540, column 0): Element '{http://ssp-standard.org/SSP1/SystemStructureCommon}BaseUnit', attribute 'deg': The attribute 'deg' is not allowed.
## ERROR (line 1541, column 0): Element '{http://ssp-standard.org/SSP1/SystemStructureCommon}DisplayUnit': This element is not expected. Expected is ( {http://ssp-standard.org/SSP1/SystemStructureCommon}Annotations ).
## UserWarning: Failed to validate ECS_Conf1.ssv against schemafile SystemStructureParameterValues.xsd
## ERROR (line 2668, column 0): Element '{http://ssp-standard.org/SSP1/SystemStructureCommon}BaseUnit', attribute 'deg': The attribute 'deg' is not allowed.
## ERROR (line 2669, column 0): Element '{http://ssp-standard.org/SSP1/SystemStructureCommon}DisplayUnit': This element is not expected. Expected is ( {http://ssp-standard.org/SSP1/SystemStructureCommon}Annotations ).
## UserWarning: Failed to validate SystemStructure.ssd against schemafile SystemStructureDescription.xsd
## ERROR (line 17, column 0): Element '{http://ssp-standard.org/SSP1/SystemStructureCommon}Enumeration': The attribute 'name' is required but missing.
## info:    Result file: embrace2_res.mat (bufferSize=1)
## info:    Initialize:
## info:      embrace.root.ECS_HW.coolinPackAir.looptype:  2
## info:      embrace.root.ECS_HW.eCS.MaxCoolPower.k    :  5.0
## info:      embrace.root.ECS_HW.pipeC.L               :  1.0
## info:      embrace.root.ECS_HW.pipeB.L               :  7.41248272578546
## info:      embrace.root.ECS_HW.pipeA.L               :  7.39290438403619
## endResult
