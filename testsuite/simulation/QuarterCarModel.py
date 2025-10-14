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
instantiated = model.instantiate()

# Simulation settings
instantiated.setResultFile("QuarterCarModel.DisplacementDisplacement_res.mat")
instantiated.setStopTime(1.0)
instantiated.setTolerance(1e-4)
instantiated.setFixedStepSize(1e-3)

# Run simulation
instantiated.initialize()
instantiated.simulate()
instantiated.terminate()
instantiated.delete()


## Result:
## info:    Result file: QuarterCarModel.DisplacementDisplacement_res.mat (bufferSize=1)
## endResult
