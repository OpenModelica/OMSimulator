## status: correct
## teardown_command: rm -rf embrace1_res.mat
## linux: no
## ucrt64: yes
## win: no
## mac: no

from OMSimulator import SSP, Settings, CRef, Capi

Settings.suppressPath = True
Capi.setCommandLineOption("--ignoreInitialUnknowns=true")

model = SSP()

model.addResource("../resources/AircraftVehicleDemonstrator.Atmosphere_Model_FMU_sf.fmu", new_name="resources/AircraftVehicleDemonstrator.Atmosphere_Model_FMU_sf.fmu")
model.addResource("../resources/AircraftVehicleDemonstrator.bC.fmu", new_name="resources/AircraftVehicleDemonstrator.bC.fmu")
model.addResource("../resources/AircraftVehicleDemonstrator.cockpit.fmu", new_name="resources/AircraftVehicleDemonstrator.cockpit.fmu")
model.addResource("../resources/AircraftVehicleDemonstrator.consumer_A.fmu", new_name="resources/AircraftVehicleDemonstrator.consumer_A.fmu")
model.addResource("../resources/AircraftVehicleDemonstrator.consumer_B.fmu", new_name="resources/AircraftVehicleDemonstrator.consumer_B.fmu")
model.addResource("../resources/AircraftVehicleDemonstrator.eCS_Generic_Export.fmu", new_name="resources/AircraftVehicleDemonstrator.eCS_Generic_Export.fmu")
model.addResource("../resources/AircraftVehicleDemonstrator.eCS_SW.fmu", new_name="resources/AircraftVehicleDemonstrator.eCS_SW.fmu")
model.addResource("../resources/AircraftVehicleDemonstrator.engine.fmu", new_name="resources/AircraftVehicleDemonstrator.engine.fmu")


## add component
model.addComponent(CRef('default', 'atmosphere_Model_FMU_sf'), 'resources/AircraftVehicleDemonstrator.Atmosphere_Model_FMU_sf.fmu')
model.addComponent(CRef('default', 'bC'), 'resources/AircraftVehicleDemonstrator.bC.fmu')
model.addComponent(CRef('default', 'cockpit'), "resources/AircraftVehicleDemonstrator.cockpit.fmu")
model.addComponent(CRef('default', 'consumer_A'), "resources/AircraftVehicleDemonstrator.consumer_A.fmu")
model.addComponent(CRef('default', 'consumer_B'), "resources/AircraftVehicleDemonstrator.consumer_B.fmu")
model.addComponent(CRef('default', 'eCS_Generic_Export'), "resources/AircraftVehicleDemonstrator.eCS_Generic_Export.fmu")
model.addComponent(CRef('default', 'eCS_SW'), "resources/AircraftVehicleDemonstrator.eCS_SW.fmu")
model.addComponent(CRef('default', 'engine'), "resources/AircraftVehicleDemonstrator.engine.fmu")

# connections (input to eCS_Generic_Export)

model.addConnection(CRef('default', 'eCS_Generic_Export', 'CABIN_TEMP_REQ_POS'), CRef('default', 'eCS_SW', 'CABIN_TEMP_REQ_POS'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'CABIN_TEMP_CMD_INH'), CRef('default', 'eCS_SW', 'CABIN_TEMP_CMD_INH'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'DistrA_REQ_POS'), CRef('default', 'eCS_SW', 'DistrA_REQ_POS'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'DistrA_CMD_INH'), CRef('default', 'eCS_SW', 'DistrA_CMD_INH'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'CABIN_FLOW_REQ_POS'), CRef('default', 'eCS_SW', 'CABIN_FLOW_REQ_POS'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'CABIN_FLOW_CMD_INH'), CRef('default', 'eCS_SW', 'CABIN_FLOW_CMD_INH'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PACK_TEMP_REQ_POS'), CRef('default', 'eCS_SW', 'PACK_TEMP_REQ_POS'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PACK_TEMP_CMD_INH'), CRef('default', 'eCS_SW', 'PACK_TEMP_CMD_INH'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'EJECTOR_V_CMD'), CRef('default', 'eCS_SW', 'EJECTOR_V_CMD'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'EJECTOR_CMD_INH'), CRef('default', 'eCS_SW', 'EJECTOR_CMD_INH'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PACK_PRESS_REQ_POS'), CRef('default', 'eCS_SW', 'PACK_PRESS_REQ_POS'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PACK_PRESS_CMD_INH'), CRef('default', 'eCS_SW', 'PACK_PRESS_CMD_INH'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'DistrB_V_CMD_OPEN'), CRef('default', 'eCS_SW', 'DistrB_V_CMD_OPEN'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'DistrB_V_CMD_CLOSE'), CRef('default', 'eCS_SW', 'DistrB_V_CMD_CLOSE'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PRSOV_CMD_OPEN_HOLD'), CRef('default', 'eCS_SW','PRSOV_CMD_OPEN_HOLD'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'RAMAIR_V_CMD_CLOSE'), CRef('default', 'eCS_SW', 'RAMAIR_V_CMD_CLOSE'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'Alt'), CRef('default', 'bC', 'Alt'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'Mach'), CRef('default', 'bC', 'Mach'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'pBleedAPU'), CRef('default', 'bC', 'pBleedAPU'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'TbleedAPU'), CRef('default', 'bC', 'TBleedAPU'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'CanopyClosed'), CRef('default', 'bC', 'CanopyClosed'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'Tamb'), CRef('default', 'atmosphere_Model_FMU_sf', 'Out_t_oat'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'Pamb'), CRef('default', 'atmosphere_Model_FMU_sf', 'Out_p_oap'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'Tfrict'), CRef('default', 'atmosphere_Model_FMU_sf', 'Out_t_stag'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'Psat'), CRef('default', 'atmosphere_Model_FMU_sf', 'Out_psat_a'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortCockpitA.m'), CRef('default', 'cockpit', 'portA.m'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortCockpitA.hn'), CRef('default', 'cockpit', 'portA.hn'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortCockpitA.xsn'), CRef('default', 'cockpit', 'portA.xsn'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortCockpitA.xwn'), CRef('default', 'cockpit', 'portA.xwn'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortCockpitB.m'), CRef('default', 'cockpit', 'portB.m'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortCockpitB.hn'), CRef('default', 'cockpit', 'portB.hn'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortCockpitB.xsn'), CRef('default', 'cockpit', 'portB.xsn'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortCockpitB.xwn'), CRef('default', 'cockpit', 'portB.xwn'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortConsumerA.m'), CRef('default', 'consumer_A', 'pin.m'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortConsumerA.hn'), CRef('default', 'consumer_A', 'pin.hn'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortConsumerA.xsn'), CRef('default', 'consumer_A', 'pin.xsn'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortConsumerA.xwn'), CRef('default', 'consumer_A', 'pin.xwn'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortConsumerB.m'), CRef('default', 'consumer_B', 'pin.m'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortConsumerB.hn'), CRef('default', 'consumer_B', 'pin.hn'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortConsumerB.xsn'), CRef('default', 'consumer_B', 'pin.xsn'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortConsumerB.xwn'), CRef('default', 'consumer_B', 'pin.xwn'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortBleedA.m'), CRef('default', 'engine', 'pB.m'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortBleedA.hn'), CRef('default', 'engine', 'pB.hn'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortBleedA.xsn'), CRef('default', 'engine', 'pB.xsn'))
model.addConnection(CRef('default', 'eCS_Generic_Export', 'PortBleedA.xwn'), CRef('default', 'engine', 'pB.xwn'))

## connections (input to eCS_SW)
model.addConnection(CRef('default', 'eCS_SW', 'Meas_PACK_TEMP_V_POS_OPEN'), CRef('default', 'eCS_Generic_Export', 'Meas_PACK_TEMP_V_POS_OPEN'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_PACK_TEMP_V_POS'), CRef('default', 'eCS_Generic_Export', 'Meas_PACK_TEMP_V_POS'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_CABIN_FLOW_V_POS_CLOSED'), CRef('default', 'eCS_Generic_Export', 'Meas_CABIN_FLOW_V_POS_CLOSED'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_CABIN_FLOW_V_POS_OPEN'), CRef('default', 'eCS_Generic_Export', 'Meas_CABIN_FLOW_V_POS_OPEN'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_CABIN_FLOW_V_POS'), CRef('default', 'eCS_Generic_Export', 'Meas_CABIN_FLOW_V_POS'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_DistrA_FLOW_V_POS_CLOSED'), CRef('default', 'eCS_Generic_Export', 'Meas_DistrA_FLOW_V_POS_CLOSED'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_DistrA_FLOW_V_POS_OPEN'), CRef('default', 'eCS_Generic_Export', 'Meas_DistrA_FLOW_V_POS_OPEN'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_CABIN_TEMP_V_POS_CLOSED'), CRef('default', 'eCS_Generic_Export', 'Meas_CABIN_TEMP_V_POS_CLOSED'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_CABIN_TEMP_V_POS_OPEN'), CRef('default', 'eCS_Generic_Export', 'Meas_CABIN_TEMP_V_POS_OPEN'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_CABIN_TEMP_V_POS'), CRef('default', 'eCS_Generic_Export', 'Meas_CABIN_TEMP_V_POS'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_COND_LO_DIFF_PRESS'), CRef('default', 'eCS_Generic_Export', 'Meas_COND_LO_DIFF_PRESS'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_COND_HIGH_DIFF_PRESS'), CRef('default', 'eCS_Generic_Export', 'Meas_COND_HIGH_DIFF_PRESS'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_OVERPRESS_SWITCH'), CRef('default', 'eCS_Generic_Export', 'Meas_OVERPRESS_SWITCH'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_PACK_PRESS'), CRef('default', 'eCS_Generic_Export', 'Meas_PACK_PRESS'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_DistrA_FLOW_V_POS'), CRef('default', 'eCS_Generic_Export', 'Meas_DistrA_FLOW_V_POS'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_PACK_PRESS_V_POS_OPEN'), CRef('default', 'eCS_Generic_Export', 'Meas_PACK_PRESS_V_POS_OPEN'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_PACK_PRESS_V_POS'), CRef('default', 'eCS_Generic_Export', 'Meas_PACK_PRESS_V_POS'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_EJECTOR_V_POS_CLOSED'), CRef('default', 'eCS_Generic_Export', 'Meas_EJECTOR_V_POS_CLOSED'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_EJECTOR_V_POS_OPEN'), CRef('default', 'eCS_Generic_Export', 'Meas_EJECTOR_V_POS_OPEN'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_EJECTOR_V_POS'), CRef('default', 'eCS_Generic_Export', 'Meas_EJECTOR_V_POS'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_PACK_TEMP_V_POS_CLOSED'), CRef('default', 'eCS_Generic_Export', 'Meas_PACK_TEMP_V_POS_CLOSED'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_COMPR_INLET_TEMP'), CRef('default', 'eCS_Generic_Export', 'Meas_COMPR_INLET_TEMP'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_DistrA_INLET_OVERTEMP'), CRef('default', 'eCS_Generic_Export', 'Meas_DistrA_INLET_OVERTEMP'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_CAB_INLET_TEMP'), CRef('default', 'eCS_Generic_Export', 'Meas_CAB_INLET_TEMP'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_DistrA_INLET_TEMP'), CRef('default', 'eCS_Generic_Export', 'Meas_DistrA_INLET_TEMP'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_PACK_TEMP'), CRef('default', 'eCS_Generic_Export', 'Meas_PACK_TEMP'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_CABIN_FLOW_SIGN'), CRef('default', 'eCS_Generic_Export', 'Meas_CABIN_FLOW_SIGN'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_CABIN_PRESS'), CRef('default', 'eCS_Generic_Export', 'Meas_CABIN_PRESS'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_DisrtA_FLOW'), CRef('default', 'eCS_Generic_Export', 'Meas_DisrtA_FLOW'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_RAMAIR_V_POS_OPEN'), CRef('default', 'eCS_Generic_Export', 'Meas_RAMAIR_V_POS_OPEN'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_DistrB_V_POS_CLOSED'), CRef('default', 'eCS_Generic_Export', 'Meas_DistrB_V_POS_CLOSED'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_DistrB_V_POS_OPEN'), CRef('default', 'eCS_Generic_Export', 'Meas_DistrB_V_POS_OPEN'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_PACK_PRESS_V_POS_CLOSED'), CRef('default', 'eCS_Generic_Export', 'Meas_PACK_PRESS_V_POS_CLOSED'))
model.addConnection(CRef('default', 'eCS_SW', 'Meas_RAMAIR_V_POS_CLOSED'), CRef('default', 'eCS_Generic_Export', 'Meas_RAMAIR_V_POS_CLOSED'))
model.addConnection(CRef('default', 'eCS_SW', 'Altitude'), CRef('default', 'bC', 'Alt'))
model.addConnection(CRef('default', 'eCS_SW', 'Mach'), CRef('default', 'bC', 'Mach'))

## connections (input to engine)
model.addConnection(CRef('default', 'engine', 'Mach'), CRef('default', 'bC', 'Mach'))
model.addConnection(CRef('default', 'engine', 'Altitude'), CRef('default', 'bC', 'Alt'))
model.addConnection(CRef('default', 'engine', 'Tamb'), CRef('default', 'atmosphere_Model_FMU_sf', 'Out_t_oat'))
model.addConnection(CRef('default', 'engine', 'Pamb'), CRef('default', 'atmosphere_Model_FMU_sf', 'Out_p_oap'))
model.addConnection(CRef('default', 'engine', 'Tfrict'), CRef('default', 'atmosphere_Model_FMU_sf', 'Out_t_stag'))
model.addConnection(CRef('default', 'engine', 'Psat'), CRef('default', 'atmosphere_Model_FMU_sf', 'Out_psat_a'))
model.addConnection(CRef('default', 'engine', 'pB.p'), CRef('default', 'eCS_Generic_Export' ,'PortBleedA.p'))
model.addConnection(CRef('default', 'engine', 'pB.h'), CRef('default', 'eCS_Generic_Export' ,'PortBleedA.h'))
model.addConnection(CRef('default', 'engine', 'pB.xs'), CRef('default', 'eCS_Generic_Export','PortBleedA.xs'))
model.addConnection(CRef('default', 'engine', 'pB.xw'), CRef('default', 'eCS_Generic_Export','PortBleedA.xw'))

## connections (input to consumer_A)

model.addConnection(CRef('default', 'consumer_A', 'pin.p'), CRef('default', 'eCS_Generic_Export', 'PortConsumerA.p'))
model.addConnection(CRef('default', 'consumer_A', 'pin.h'), CRef('default', 'eCS_Generic_Export', 'PortConsumerA.h'))
model.addConnection(CRef('default', 'consumer_A', 'pin.xs'), CRef('default', 'eCS_Generic_Export', 'PortConsumerA.xs'))
model.addConnection(CRef('default', 'consumer_A', 'pin.xw'), CRef('default', 'eCS_Generic_Export', 'PortConsumerA.xw'))

## connections (input to consumer_B)

model.addConnection(CRef('default', 'consumer_B', 'pin.p'), CRef('default', 'eCS_Generic_Export', 'PortConsumerB.p'))
model.addConnection(CRef('default', 'consumer_B', 'pin.h'), CRef('default', 'eCS_Generic_Export', 'PortConsumerB.h'))
model.addConnection(CRef('default', 'consumer_B', 'pin.xs'), CRef('default', 'eCS_Generic_Export', 'PortConsumerB.xs'))
model.addConnection(CRef('default', 'consumer_B', 'pin.xw'), CRef('default', 'eCS_Generic_Export', 'PortConsumerB.xw'))

## connections (input to cockpit)

model.addConnection(CRef('default', 'cockpit', 'portA.p'), CRef('default', 'eCS_Generic_Export', 'PortCockpitA.p'))
model.addConnection(CRef('default', 'cockpit', 'portA.h'), CRef('default', 'eCS_Generic_Export', 'PortCockpitA.h'))
model.addConnection(CRef('default', 'cockpit', 'portA.xs'), CRef('default', 'eCS_Generic_Export', 'PortCockpitA.xs'))
model.addConnection(CRef('default', 'cockpit', 'portA.xw'), CRef('default', 'eCS_Generic_Export', 'PortCockpitA.xw'))
model.addConnection(CRef('default', 'cockpit', 'portB.p'), CRef('default', 'eCS_Generic_Export', 'PortCockpitB.p'))
model.addConnection(CRef('default', 'cockpit', 'portB.h'), CRef('default', 'eCS_Generic_Export', 'PortCockpitB.h'))
model.addConnection(CRef('default', 'cockpit', 'portB.xs'), CRef('default', 'eCS_Generic_Export', 'PortCockpitB.xs'))
model.addConnection(CRef('default', 'cockpit', 'portB.xw'), CRef('default', 'eCS_Generic_Export', 'PortCockpitB.xw'))

## connections (input to atmosphere_Model_FMU_sf)

model.addConnection(CRef('default', 'atmosphere_Model_FMU_sf', 'Mach'), CRef('default', 'bC', 'Mach'))
model.addConnection(CRef('default', 'atmosphere_Model_FMU_sf', 'Alt'), CRef('default', 'bC', 'Alt'))

instantiated_model = model.instantiate() ## internally generate the json file and also set the model state like virgin,
instantiated_model.setResultFile("AircraftVehicleDemonstrator_res.mat")

instantiated_model.setValue(CRef('default', 'atmosphere_Model_FMU_sf', 'Parameters.Atmos.Value'), 6.0) ## Hot climate

instantiated_model.removeSignalsFromResults(".*")
instantiated_model.addSignalsToResults("default.(engine.pB.*|eCS_Generic_Export.(Meas_PACK_TEMP|Meas_PACK_PRESS))")

## TODO add regex with CRef
# instantiated_model.addSignalsToResults(CRef('default','engine','pB\\..*'))

## simulation settings
instantiated_model.setStopTime(1.0)
instantiated_model.setTolerance(1e-6)
instantiated_model.setStepSize(1e-4)

instantiated_model.initialize()
instantiated_model.simulate()
instantiated_model.terminate()
instantiated_model.delete()

## Result:
## warning: [cockpit: resources/0003_cockpit.fmu] The dependencies of the initial unknowns defined in the FMU are ignored because the flag --ignoreInitialUnknowns is active. Instead, all the initial unknowns will depend on all inputs.
## warning: [consumer_A: resources/0004_consumer_A.fmu] The dependencies of the initial unknowns defined in the FMU are ignored because the flag --ignoreInitialUnknowns is active. Instead, all the initial unknowns will depend on all inputs.
## warning: [consumer_B: resources/0005_consumer_B.fmu] The dependencies of the initial unknowns defined in the FMU are ignored because the flag --ignoreInitialUnknowns is active. Instead, all the initial unknowns will depend on all inputs.
## warning: [eCS_Generic_Export: resources/0006_eCS_Generic_Export.fmu] The dependencies of the initial unknowns defined in the FMU are ignored because the flag --ignoreInitialUnknowns is active. Instead, all the initial unknowns will depend on all inputs.
## warning: [eCS_SW: resources/0007_eCS_SW.fmu] The dependencies of the initial unknowns defined in the FMU are ignored because the flag --ignoreInitialUnknowns is active. Instead, all the initial unknowns will depend on all inputs.
## warning: [engine: resources/0008_engine.fmu] The dependencies of the initial unknowns defined in the FMU are ignored because the flag --ignoreInitialUnknowns is active. Instead, all the initial unknowns will depend on all inputs.
## info:    Alg. loop (size 80/1800)
##            model.root.engine
##            model.root.eCS_SW
##            model.root.eCS_Generic_Export
##            model.root.consumer_B
##            model.root.consumer_A
##            model.root.cockpit
## info:    Result file: AircraftVehicleDemonstrator_res.mat (bufferSize=1)
## info:    6 warnings
## info:    0 errors
## endResult
