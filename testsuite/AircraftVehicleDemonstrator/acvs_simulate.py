## status: correct
## teardown_command: rm -rf acvs_res.csv
## linux: yes
## ucrt64: no
## win: no
## mac: no

from OMSimulator import SSP, Settings, CRef, Capi

Settings.suppressPath = True
Capi.setCommandLineOption("--wallTime=true --ignoreInitialUnknowns=true")

model = SSP("../resources/acvs_compositeModel.ssp")
# model.addResource("bc_files/CONOPS.csv", new_name="resources/CONOPS.csv")
# model.addComponent(CRef("root", "bc"), "resources/CONOPS.csv")

# model.addConnection(CRef("root", "bc", "Alt"), CRef("root", "consumer", "Alt"))
# model.addConnection(CRef("root", "bc", "Mach"), CRef("root", "consumer", "Mach"))
# model.addConnection(CRef("root", "bc", "heatLoad"), CRef("root", "consumer", "heatLoad"))

# model.addConnection(CRef("root", "bc", "Alt"), CRef("root", "ecs_hw", "Alt"))
# model.addConnection(CRef("root", "bc", "Mach"), CRef("root", "ecs_hw", "Mach"))

# model.addConnection(CRef("root", "bc", "Aircraft_state"), CRef("root", "adaption", "Aircraft_State"))

# model.addConnection(CRef("root", "bc", "Alt"), CRef("root", "atmos", "Alt"))
# model.addConnection(CRef("root", "bc", "Mach"), CRef("root", "atmos", "Ma"))

# model.addConnection(CRef("root", "bc", "Mach"), CRef("root", "fuel", "Mach"))
# model.addConnection(CRef("root", "bc", "eng_mflow"), CRef("root", "fuel", "eng_mflow"))

instantiated = model.instantiate()

instantiated.setResultFile("acvs_res.csv")
instantiated.setStopTime(1.0)
instantiated.setFixedStepSize(0.01)
instantiated.setLoggingInterval(1)
instantiated.setTolerance(1e-5)

instantiated.initialize()
instantiated.simulate()
instantiated.terminate()
instantiated.delete()

## Result:
## warning: [consumer: resources/0005_consumer.fmu] The dependencies of the initial unknowns defined in the FMU are ignored because the flag --ignoreInitialUnknowns is active. Instead, all the initial unknowns will depend on all inputs.
## warning: [ecs_hw: resources/0006_ecs_hw.fmu] The dependencies of the initial unknowns defined in the FMU are ignored because the flag --ignoreInitialUnknowns is active. Instead, all the initial unknowns will depend on all inputs.
## info:    Alg. loop (size 13/50)
##            model.root.fuel
##            model.root.ecs_sw
##            model.root.ecs_hw
##            model.root.consumer
##            model.root.adaption
## info:    Result file: acvs_res.csv (bufferSize=1)
## info:    2 warnings
## info:    0 errors
## endResult
