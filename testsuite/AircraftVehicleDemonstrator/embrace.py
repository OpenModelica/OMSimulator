## status: correct
## teardown_command:
## linux: yes
## mingw: yes
## win: yes
## mac: yes
"""
Created on Mon Dec 21 10:30:26 2020

@author: robha67
"""
import OMSimulator

#OMSimulator.OMSimulator()
oms=OMSimulator.OMSimulator()
a=oms.getVersion()
#print(a)
oms.setCommandLineOption("--suppressPath=true  --wallTime=true")


oms.importFile("../resources/embrace.ssp")
oms.addSubModel("model.root.BC", "../resources/CONOPS.csv")

##----------------- Connections to BC files ----------------------------
#BC -> Consumer
oms.addConnection("model.root.Consumer.Alt","model.root.BC.Alt")
oms.addConnection("model.root.Consumer.Mach","model.root.BC.Mach")
oms.addConnection("model.root.Consumer.heatLoad","model.root.BC.heatLoad")

# #BC -> ECS_HW
oms.addConnection("model.root.ECS_HW.Alt","model.root.BC.Alt")
oms.addConnection("model.root.ECS_HW.Mach","model.root.BC.Mach")

# #BC -> ECS_SW
oms.addConnection("model.root.ECS_SW.WGT_ON_WHL","model.root.BC.WGT_ON_WHL")

# #BC -> Atmos
oms.addConnection("model.root.Atmos.Alt","model.root.BC.Alt")
oms.addConnection("model.root.Atmos.Ma","model.root.BC.Mach")
# #-----------------------------------------------------------------------

#set params
oms.setReal("model.root.ECS_SW.PID.k", 10)
oms.setReal("model.root.ECS_HW.eCS.MaxCoolPower.k", 5)
oms.setInteger("model.root.ECS_HW.coolinPackAir.looptype", 2)
oms.setInteger("model.root.ECS_HW.phSrc.looptype", 2)
oms.setInteger("model.root.ECS_HW.pipeB1.looptype", 2)

# #simulation settings
#oms.setResultFile("model", "ReferenceResults_L=5_CONOPS.mat")
oms.setResultFile("model", "sim_results.mat")
oms.setStopTime("model", 1200)
oms.setFixedStepSize("model", 1e-3)
oms.setLoggingInterval("model", 1)
oms.exportDependencyGraphs("model.root","init.dot","event.dot","simulation.dot")

oms.instantiate("model")
oms.initialize("model")
print("info:    Initialize:")
print("info:      model.root.ECS_HW.coolinPackAir.looptype: " , oms.getInteger("model.root.ECS_HW.coolinPackAir.looptype")[0])
print("info:      model.root.ECS_SW.PID.k                 : " , oms.getReal("model.root.ECS_HW.eCS.MaxCoolPower.k")[0])
print("info:      model.root.ECS_SW.PID.k                 : " , oms.getReal("model.root.ECS_SW.PID.k")[0])
print("info:      model.root.ECS_HW.pipeB1.L              : " , oms.getReal("model.root.ECS_HW.pipeB1.L")[0])
print("info:      model.root.ECS_HW.pipeB.L               : " , oms.getReal("model.root.ECS_HW.pipeB.L")[0])
print("info:      model.root.ECS_HW.pipeA.L               : " , oms.getReal("model.root.ECS_HW.pipeA.L")[0])

oms.simulate("model")
oms.terminate("model")
oms.delete("model")

## Result:
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.parameterSet_inputParameters.parameter_pipeRadius"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.parameterSet_inputParameters.parameter_pipeThickness"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.parameterSet_inputParameters.parameter_bendRadius"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.parameterSet_inputParameters.parameter_testReal"
## error:   [setInteger] Unknown signal "model.root.product_subAssembly1.part_testPipe1.parameterSet_inputParameters.parameter_testInteger"
## error:   [setBoolean] Unknown signal "model.root.product_subAssembly1.part_testPipe1.parameterSet_inputParameters.parameter_testBoolean"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_1.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_1.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_1.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_2.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_2.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_2.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_3.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_3.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_3.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_4.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_4.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_4.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_5.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_5.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_5.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_6.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_6.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_6.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_7.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_7.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_7.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_8.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_8.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_8.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_9.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_9.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_9.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_10.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_10.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_Point_10.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_1.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_1.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_2.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_2.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_3.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_3.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_4.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_4.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_5.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_5.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_6.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_6.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_7.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_7.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_8.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_8.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeLength.parameter_pipeLength"
## error:   [setInteger] Unknown signal "model.root.product_subAssembly1.part_testPipe1.geometricalSet_systemsInfoExp.parameterSet_pipeLength.parameter_noOfBends"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.parameterSet_inputParameters.parameter_pipeRadius"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.parameterSet_inputParameters.parameter_pipeThickness"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.parameterSet_inputParameters.parameter_bendRadius"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.parameterSet_outputParameters.parameter_pipeLength"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_1.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_1.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_1.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_2.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_2.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_2.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_3.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_3.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_3.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_4.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_4.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_4.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_5.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_5.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_5.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_6.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_6.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_6.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_7.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_7.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_7.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_8.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_8.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_8.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_9.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_9.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_9.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_10.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_10.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_Point_10.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_1.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_1.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_2.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_2.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_3.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_3.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_4.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_4.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_5.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_5.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_6.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_6.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_7.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_7.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_8.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_8.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeLength.parameter_pipeLength"
## error:   [setInteger] Unknown signal "model.root.product_subAssembly1.part_testPipe2.geometricalSet_systemsInfoExp.parameterSet_pipeLength.parameter_noOfBends"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_valve1.parameterSet_inputParameters.parameter_pipeRadius"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_valve1.parameterSet_inputParameters.parameter_pipeThickness"
## error:   [setReal] Unknown signal "model.root.product_subAssembly1.part_valve1.parameterSet_outputParameters.parameter_valveLength"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.parameterSet_inputParameters.parameter_pipeRadius"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.parameterSet_inputParameters.parameter_pipeThickness"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.parameterSet_inputParameters.parameter_bendRadius"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.parameterSet_inputParameters.parameter_testReal"
## error:   [setInteger] Unknown signal "model.root.product_subAssembly2.part_testPipe.parameterSet_inputParameters.parameter_testInteger"
## error:   [setBoolean] Unknown signal "model.root.product_subAssembly2.part_testPipe.parameterSet_inputParameters.parameter_testBoolean"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.parameterSet_outputParameters.parameter_pipeLength"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_1.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_1.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_1.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_2.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_2.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_2.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_3.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_3.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_3.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_4.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_4.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_4.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_5.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_5.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_5.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_6.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_6.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_6.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_7.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_7.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_7.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_8.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_8.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_8.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_9.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_9.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_9.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_10.parameter_coordinateX"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_10.parameter_coordinateY"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_Point_10.parameter_coordinateZ"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_1.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_1.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_2.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_2.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_3.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_3.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_4.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_4.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_5.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_5.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_6.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_6.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_7.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_7.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_8.parameter_innerAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeBendAngle_8.parameter_absoluteAngle"
## error:   [setReal] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeLength.parameter_pipeLength"
## error:   [setInteger] Unknown signal "model.root.product_subAssembly2.part_testPipe.geometricalSet_systemsInfoExp.parameterSet_pipeLength.parameter_noOfBends"
## error:   [setInteger] Unknown signal "model.root.ECS_HW.pipeB1.looptype"
## info:    Result file: sim_results.mat (bufferSize=1)
## error:   [getReal] Unknown signal "model.root.ECS_HW.pipeB1.L"
## info:    0 warnings
## info:    166 errors
## endResult
