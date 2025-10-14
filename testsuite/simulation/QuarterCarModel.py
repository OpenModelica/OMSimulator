## status: correct
## teardown_command: rm -rf QuarterCarModel.DisplacementDisplacement_res.mat
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# Create the top-level SSP model
model = SSP()

# Add FMU and CSV resources
model.addResource("../resources/QuarterCarModel.DisplacementDisplacement.Chassis.fmu", new_name="resources/Chassis.fmu")
model.addResource("../resources/QuarterCarModel.DisplacementDisplacement.Wheel.fmu", new_name="resources/Wheel.fmu")
model.addResource("../resources/road.csv", new_name="resources/road.csv")

# Add components to the system
model.addComponent(CRef("default", "Chassis"),"resources/Chassis.fmu")
model.addComponent(CRef("default", "Wheel"), "resources/Wheel.fmu")
model.addComponent(CRef("default", "BC"), "resources/road.csv")

# Add connections (signal mapping)

model.addConnection(CRef("default", "Chassis", "z_c"), CRef("default", "Wheel", "z_c"))
model.addConnection(CRef("default", "Chassis", "der_z_c"), CRef("default", "Wheel", "der_z_c"))
model.addConnection(CRef("default", "Chassis", "z_w"), CRef("default", "Wheel", "z_w"))
model.addConnection(CRef("default", "Chassis", "der_z_w"), CRef("default", "Wheel", "der_z_w"))
model.addConnection(CRef("default", "BC", "road"), CRef("default", "Wheel", "z"))

# Instantiate model
instantiated_model = model.instantiate()

# Simulation settings
instantiated_model.setResultFile("QuarterCarModel.DisplacementDisplacement_res.mat")

# instantiated_model.setSolver("oms_solver_wc_mav")
# instantiated_model.setVariableStepSize(1e-2, 1e-2, 1e-2)

instantiated_model.setStopTime(1.0)
instantiated_model.setTolerance(1e-4)
instantiated_model.setFixedStepSize(1e-3)

# Run simulation
instantiated_model.initialize()
instantiated_model.simulate()
instantiated_model.terminate()
instantiated_model.delete()


## Result:
## info:    Result file: QuarterCarModel.DisplacementDisplacement_res.mat (bufferSize=1)
## endResult
