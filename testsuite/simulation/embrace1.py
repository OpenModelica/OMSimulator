## status: correct
## teardown_command:
## linux: yes
## ucrt64: no
## win: no
## mac: no

from OMSimulator import SSP, Settings, CRef

Settings.suppressPath = True

model = SSP('../resources/embrace.ssp')
model.addResource("../resources/embrace/CONOPS.csv", new_name = "resources/CONOPS.csv")
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
instantiated_model.setResultFile("embrace1_res.mat")
instantiated_model.setStopTime(12.0)
instantiated_model.setStepSize(1e-3)
instantiated_model.setLoggingInterval(1)

instantiated_model.initialize()
print("info:    Initialize:")

print("info:      embrace.root.ECS_HW.coolinPackAir.looptype: " instantiated_model.getValue(CRef('root', 'ECS_HW', 'coolinPackAir.looptype')))
print("info:      embrace.root.ECS_HW.eCS.MaxCoolPower.k    : " instantiated_model.getValue(CRef('root', 'ECS_HW', 'eCS.MaxCoolPower.k')))
print("info:      embrace.root.ECS_HW.pipeC.L               : " instantiated_model.getValue(CRef('root', 'ECS_HW', 'pipeC.L')))
print("info:      embrace.root.ECS_HW.pipeB.L               : " instantiated_model.getValue(CRef('root', 'ECS_HW', 'pipeB.L')))
print("info:      embrace.root.ECS_HW.pipeA.L               : " instantiated_model.getValue(CRef('root', 'ECS_HW', 'pipeA.L')))

instantiated_model.simulate()
instantiated_model.terminate()
instantiated_model.delete()
