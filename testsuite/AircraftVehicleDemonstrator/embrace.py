## status: correct
## teardown_command: rm SystemStructure.ssd event.dot init.dot sim_results.mat simulation.dot
## linux: yes
## mingw: no
## win: no
## mac: no

"""
Created on Mon Dec 21 10:30:26 2020

@author: robha67
"""
import OMSimulator

oms = OMSimulator.OMSimulator()
oms.setCommandLineOption("--suppressPath=true --wallTime=true --ignoreInitialUnknowns=true")

oms.importFile("../resources/embrace.ssp")
oms.addSubModel("model.root.BC", "../resources/CONOPS.csv")

#----------------- Connections to BC files -----------------
#BC -> Consumer
oms.addConnection("model.root.Consumer.Alt", "model.root.BC.Alt")
oms.addConnection("model.root.Consumer.Mach", "model.root.BC.Mach")
oms.addConnection("model.root.Consumer.heatLoad", "model.root.BC.heatLoad")

#BC -> ECS_HW
oms.addConnection("model.root.ECS_HW.Alt", "model.root.BC.Alt")
oms.addConnection("model.root.ECS_HW.Mach", "model.root.BC.Mach")

#BC -> ECS_SW
oms.addConnection("model.root.BC.Aircraft_state", "model.root.AdaptionUnit.Aircraft_State")

#BC -> Atmos
oms.addConnection("model.root.Atmos.Alt", "model.root.BC.Alt")
oms.addConnection("model.root.Atmos.Ma", "model.root.BC.Mach")
#-----------------------------------------------------------

#set params
oms.setReal("model.root.ECS_HW.eCS.MaxCoolPower.k", 5)
oms.setInteger("model.root.ECS_HW.coolinPackAir.looptype", 2)
oms.setInteger("model.root.ECS_HW.phSrc.looptype", 2)
oms.setInteger("model.root.ECS_HW.pipeB.looptype", 2)
oms.setInteger("model.root.ECS_HW.pipeA.looptype", 2)

#simulation settings
oms.setResultFile("model", "sim_results.mat")
oms.setStopTime("model", 12)
oms.setFixedStepSize("model", 1e-3)
oms.setLoggingInterval("model", 1)
oms.exportDependencyGraphs("model.root", "init.dot", "event.dot", "simulation.dot")

oms.instantiate("model")
oms.initialize("model")
print("info:    Initialize:")
print("info:      model.root.ECS_HW.coolinPackAir.looptype: ", oms.getInteger("model.root.ECS_HW.coolinPackAir.looptype")[0])
print("info:      model.root.ECS_HW.eCS.MaxCoolPower.k    : ", oms.getReal("model.root.ECS_HW.eCS.MaxCoolPower.k")[0])
print("info:      model.root.ECS_HW.pipeC.L               : ", oms.getReal("model.root.ECS_HW.pipeC.L")[0])
print("info:      model.root.ECS_HW.pipeB.L               : ", oms.getReal("model.root.ECS_HW.pipeB.L")[0])
print("info:      model.root.ECS_HW.pipeA.L               : ", oms.getReal("model.root.ECS_HW.pipeA.L")[0])

oms.simulate("model")
oms.terminate("model")
oms.delete("model")

## Result:
## warning: [ECS_SW: resources/0002_ECS_SW.fmu] The dependencies of the initial unknowns defined in the FMU are ignored because the flag --ignoreInitialUnknowns is active. Instead, all the initial unknowns will depend on all inputs.
## info:    Result file: sim_results.mat (bufferSize=1)
## info:    Initialize:
## info:      model.root.ECS_HW.coolinPackAir.looptype:  2
## info:      model.root.ECS_HW.eCS.MaxCoolPower.k    :  5.0
## info:      model.root.ECS_HW.pipeC.L               :  1.0
## info:      model.root.ECS_HW.pipeB.L               :  0.976535328081166
## info:      model.root.ECS_HW.pipeA.L               :  0.976535328081166
## info:    1 warnings
## info:    0 errors
## endResult
