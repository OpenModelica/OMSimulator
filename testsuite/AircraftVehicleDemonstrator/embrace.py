## status: correct
## teardown_command: rm -rf embrace/
## linux: yes
## mingw32: no
## mingw64: no
## win: no
## mac: no

"""
Created on Mon Dec 21 10:30:26 2020

@author: robha67
"""
import OMSimulator

oms = OMSimulator.OMSimulator()
oms.setCommandLineOption("--suppressPath=true --wallTime=true --ignoreInitialUnknowns=false")
oms.setTempDirectory("./embrace/")
oms.setWorkingDirectory("./embrace/")

oms.importFile("../../resources/embrace.ssp")
oms.addSubModel("embrace.root.BC", "../../resources/CONOPS.csv")

#----------------- Connections to BC files -----------------
#BC -> Consumer
oms.addConnection("embrace.root.Consumer.Alt", "embrace.root.BC.Alt")
oms.addConnection("embrace.root.Consumer.Mach", "embrace.root.BC.Mach")
oms.addConnection("embrace.root.Consumer.heatLoad", "embrace.root.BC.heatLoad")

#BC -> ECS_HW
oms.addConnection("embrace.root.ECS_HW.Alt", "embrace.root.BC.Alt")
oms.addConnection("embrace.root.ECS_HW.Mach", "embrace.root.BC.Mach")

#BC -> ECS_SW
oms.addConnection("embrace.root.BC.Aircraft_state", "embrace.root.AdaptionUnit.Aircraft_State")

#BC -> Atmos
oms.addConnection("embrace.root.Atmos.Alt", "embrace.root.BC.Alt")
oms.addConnection("embrace.root.Atmos.Ma", "embrace.root.BC.Mach")
#-----------------------------------------------------------

#set params
oms.setReal("embrace.root.ECS_HW.eCS.MaxCoolPower.k", 5)
oms.setInteger("embrace.root.ECS_HW.coolinPackAir.looptype", 2)
oms.setInteger("embrace.root.ECS_HW.phSrc.looptype", 2)
oms.setInteger("embrace.root.ECS_HW.pipeB.looptype", 2)
oms.setInteger("embrace.root.ECS_HW.pipeA.looptype", 2)

#simulation settings
oms.setResultFile("embrace", "sim_results.mat")
oms.setStopTime("embrace", 12)
oms.setFixedStepSize("embrace", 1e-3)
oms.setLoggingInterval("embrace", 1)
oms.exportDependencyGraphs("embrace.root", "init.dot", "event.dot", "simulation.dot")

oms.instantiate("embrace")
oms.initialize("embrace")
print("info:    Initialize:")
print("info:      embrace.root.ECS_HW.coolinPackAir.looptype: ", oms.getInteger("embrace.root.ECS_HW.coolinPackAir.looptype")[0])
print("info:      embrace.root.ECS_HW.eCS.MaxCoolPower.k    : ", oms.getReal("embrace.root.ECS_HW.eCS.MaxCoolPower.k")[0])
print("info:      embrace.root.ECS_HW.pipeC.L               : ", oms.getReal("embrace.root.ECS_HW.pipeC.L")[0])
print("info:      embrace.root.ECS_HW.pipeB.L               : ", oms.getReal("embrace.root.ECS_HW.pipeB.L")[0])
print("info:      embrace.root.ECS_HW.pipeA.L               : ", oms.getReal("embrace.root.ECS_HW.pipeA.L")[0])

oms.simulate("embrace")
oms.terminate("embrace")
oms.delete("embrace")

## Result:
## info:    Result file: sim_results.mat (bufferSize=1)
## info:    Initialize:
## info:      embrace.root.ECS_HW.coolinPackAir.looptype:  2
## info:      embrace.root.ECS_HW.eCS.MaxCoolPower.k    :  5.0
## info:      embrace.root.ECS_HW.pipeC.L               :  1.0
## info:      embrace.root.ECS_HW.pipeB.L               :  0.976535328081166
## info:      embrace.root.ECS_HW.pipeA.L               :  0.976535328081166
## endResult
