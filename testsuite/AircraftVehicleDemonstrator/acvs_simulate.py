from OMSimulator.ssp import SSP, CRef

model = SSP("acvs_compositeModel.ssp")
model.addResource("bc_files/CONOPS.csv", new_name="resources/CONOPS.csv")
model.addComponent(CRef("root", "bc"), "resources/CONOPS.csv")

model.addConnection(CRef("root", "bc", "Alt"), CRef("root", "consumer", "Alt"))
model.addConnection(CRef("root", "bc", "Mach"), CRef("root", "consumer", "Mach"))
model.addConnection(CRef("root", "bc", "heat_load"), CRef("root", "consumer", "heatLoad"))

model.addConnection(CRef("root", "bc", "Alt"), CRef("root", "ecs_hw", "Alt"))
model.addConnection(CRef("root", "bc", "Mach"), CRef("root", "ecs_hw", "Mach"))

model.addConnection(CRef("root", "bc", "Aircraft_state"), CRef("root", "adaption", "Aircraft_State"))

model.addConnection(CRef("root", "bc", "Alt"), CRef("root", "atmos", "Alt"))
model.addConnection(CRef("root", "bc", "Mach"), CRef("root", "atmos", "Ma"))

model.addConnection(CRef("root", "bc", "Mach"), CRef("root", "fuel", "Mach"))
model.addConnection(CRef("root", "bc", "eng_mflow"), CRef("root", "fuel", "eng_mflow"))

instantiated = model.instantiate()

instantiated.setResultFile("result.csv")
instantiated.setStopTime(1200)
instantiated.setFixedStepSize(0.01)
instantiated.setLoggingInterval(1)
instantiated.setTolerance(1e-5)

instantiated.initialize()
instantiated.simulate()
instantiated.terminate()
instantiated.delete()
