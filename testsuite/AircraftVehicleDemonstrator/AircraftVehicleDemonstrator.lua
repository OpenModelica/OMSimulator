-- status: correct
-- teardown_command: rm -rf AircraftVehicleDemonstrator_tmp/ AircraftVehicleDemonstrator.log AircraftVehicleDemonstrator*.dot AircraftVehicleDemonstrator_res.mat
-- linux: no
-- mingw32: no
-- mingw64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true --ignoreInitialUnknowns=true")
oms_setTempDirectory("./AircraftVehicleDemonstrator_tmp/")

oms_newModel("AircraftVehicleDemonstrator")
oms_addSystem("AircraftVehicleDemonstrator.root", oms_system_wc)

-- instantiate FMUs
oms_addSubModel("AircraftVehicleDemonstrator.root.atmosphere_Model_FMU_sf", "../resources/AircraftVehicleDemonstrator.Atmosphere_Model_FMU_sf.fmu")
oms_addSubModel("AircraftVehicleDemonstrator.root.bC", "../resources/AircraftVehicleDemonstrator.bC.fmu")
oms_addSubModel("AircraftVehicleDemonstrator.root.cockpit", "../resources/AircraftVehicleDemonstrator.cockpit.fmu")
oms_addSubModel("AircraftVehicleDemonstrator.root.consumer_A", "../resources/AircraftVehicleDemonstrator.consumer_A.fmu")
oms_addSubModel("AircraftVehicleDemonstrator.root.consumer_B", "../resources/AircraftVehicleDemonstrator.consumer_B.fmu")
oms_addSubModel("AircraftVehicleDemonstrator.root.eCS_Generic_Export", "../resources/AircraftVehicleDemonstrator.eCS_Generic_Export.fmu")
oms_addSubModel("AircraftVehicleDemonstrator.root.eCS_SW", "../resources/AircraftVehicleDemonstrator.eCS_SW.fmu")
oms_addSubModel("AircraftVehicleDemonstrator.root.engine", "../resources/AircraftVehicleDemonstrator.engine.fmu")

-- connections (input to eCS_Generic_Export)
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.CABIN_TEMP_REQ_POS", "AircraftVehicleDemonstrator.root.eCS_SW.CABIN_TEMP_REQ_POS")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.CABIN_TEMP_CMD_INH", "AircraftVehicleDemonstrator.root.eCS_SW.CABIN_TEMP_CMD_INH")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.DistrA_REQ_POS", "AircraftVehicleDemonstrator.root.eCS_SW.DistrA_REQ_POS")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.DistrA_CMD_INH", "AircraftVehicleDemonstrator.root.eCS_SW.DistrA_CMD_INH")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.CABIN_FLOW_REQ_POS", "AircraftVehicleDemonstrator.root.eCS_SW.CABIN_FLOW_REQ_POS")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.CABIN_FLOW_CMD_INH", "AircraftVehicleDemonstrator.root.eCS_SW.CABIN_FLOW_CMD_INH")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PACK_TEMP_REQ_POS", "AircraftVehicleDemonstrator.root.eCS_SW.PACK_TEMP_REQ_POS")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PACK_TEMP_CMD_INH", "AircraftVehicleDemonstrator.root.eCS_SW.PACK_TEMP_CMD_INH")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.EJECTOR_V_CMD", "AircraftVehicleDemonstrator.root.eCS_SW.EJECTOR_V_CMD")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.EJECTOR_CMD_INH", "AircraftVehicleDemonstrator.root.eCS_SW.EJECTOR_CMD_INH")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PACK_PRESS_REQ_POS", "AircraftVehicleDemonstrator.root.eCS_SW.PACK_PRESS_REQ_POS")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PACK_PRESS_CMD_INH", "AircraftVehicleDemonstrator.root.eCS_SW.PACK_PRESS_CMD_INH")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.DistrB_V_CMD_OPEN", "AircraftVehicleDemonstrator.root.eCS_SW.DistrB_V_CMD_OPEN")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.DistrB_V_CMD_CLOSE", "AircraftVehicleDemonstrator.root.eCS_SW.DistrB_V_CMD_CLOSE")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PRSOV_CMD_OPEN_HOLD", "AircraftVehicleDemonstrator.root.eCS_SW.PRSOV_CMD_OPEN_HOLD")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.RAMAIR_V_CMD_CLOSE", "AircraftVehicleDemonstrator.root.eCS_SW.RAMAIR_V_CMD_CLOSE")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.Alt", "AircraftVehicleDemonstrator.root.bC.Alt")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.Mach", "AircraftVehicleDemonstrator.root.bC.Mach")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.pBleedAPU", "AircraftVehicleDemonstrator.root.bC.pBleedAPU")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.TbleedAPU", "AircraftVehicleDemonstrator.root.bC.TBleedAPU")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.CanopyClosed", "AircraftVehicleDemonstrator.root.bC.CanopyClosed")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.Tamb", "AircraftVehicleDemonstrator.root.atmosphere_Model_FMU_sf.Out_t_oat")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.Pamb", "AircraftVehicleDemonstrator.root.atmosphere_Model_FMU_sf.Out_p_oap")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.Tfrict", "AircraftVehicleDemonstrator.root.atmosphere_Model_FMU_sf.Out_t_stag")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.Psat", "AircraftVehicleDemonstrator.root.atmosphere_Model_FMU_sf.Out_psat_a")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortCockpitA.m", "AircraftVehicleDemonstrator.root.cockpit.portA.m")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortCockpitA.hn", "AircraftVehicleDemonstrator.root.cockpit.portA.hn")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortCockpitA.xsn", "AircraftVehicleDemonstrator.root.cockpit.portA.xsn")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortCockpitA.xwn", "AircraftVehicleDemonstrator.root.cockpit.portA.xwn")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortCockpitB.m", "AircraftVehicleDemonstrator.root.cockpit.portB.m")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortCockpitB.hn", "AircraftVehicleDemonstrator.root.cockpit.portB.hn")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortCockpitB.xsn", "AircraftVehicleDemonstrator.root.cockpit.portB.xsn")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortCockpitB.xwn", "AircraftVehicleDemonstrator.root.cockpit.portB.xwn")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortConsumerA.m", "AircraftVehicleDemonstrator.root.consumer_A.pin.m")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortConsumerA.hn", "AircraftVehicleDemonstrator.root.consumer_A.pin.hn")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortConsumerA.xsn", "AircraftVehicleDemonstrator.root.consumer_A.pin.xsn")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortConsumerA.xwn", "AircraftVehicleDemonstrator.root.consumer_A.pin.xwn")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortConsumerB.m", "AircraftVehicleDemonstrator.root.consumer_B.pin.m")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortConsumerB.hn", "AircraftVehicleDemonstrator.root.consumer_B.pin.hn")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortConsumerB.xsn", "AircraftVehicleDemonstrator.root.consumer_B.pin.xsn")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortConsumerB.xwn", "AircraftVehicleDemonstrator.root.consumer_B.pin.xwn")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortBleedA.m", "AircraftVehicleDemonstrator.root.engine.pB.m")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortBleedA.hn", "AircraftVehicleDemonstrator.root.engine.pB.hn")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortBleedA.xsn", "AircraftVehicleDemonstrator.root.engine.pB.xsn")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortBleedA.xwn", "AircraftVehicleDemonstrator.root.engine.pB.xwn")

-- connections (input to eCS_SW)
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_PACK_TEMP_V_POS_OPEN", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_PACK_TEMP_V_POS_OPEN")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_PACK_TEMP_V_POS", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_PACK_TEMP_V_POS")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_CABIN_FLOW_V_POS_CLOSED", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_CABIN_FLOW_V_POS_CLOSED")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_CABIN_FLOW_V_POS_OPEN", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_CABIN_FLOW_V_POS_OPEN")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_CABIN_FLOW_V_POS", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_CABIN_FLOW_V_POS")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_DistrA_FLOW_V_POS_CLOSED", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_DistrA_FLOW_V_POS_CLOSED")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_DistrA_FLOW_V_POS_OPEN", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_DistrA_FLOW_V_POS_OPEN")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_CABIN_TEMP_V_POS_CLOSED", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_CABIN_TEMP_V_POS_CLOSED")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_CABIN_TEMP_V_POS_OPEN", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_CABIN_TEMP_V_POS_OPEN")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_CABIN_TEMP_V_POS", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_CABIN_TEMP_V_POS")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_COND_LO_DIFF_PRESS", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_COND_LO_DIFF_PRESS")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_COND_HIGH_DIFF_PRESS", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_COND_HIGH_DIFF_PRESS")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_OVERPRESS_SWITCH", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_OVERPRESS_SWITCH")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_PACK_PRESS", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_PACK_PRESS")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_DistrA_FLOW_V_POS", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_DistrA_FLOW_V_POS")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_PACK_PRESS_V_POS_OPEN", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_PACK_PRESS_V_POS_OPEN")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_PACK_PRESS_V_POS", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_PACK_PRESS_V_POS")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_EJECTOR_V_POS_CLOSED", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_EJECTOR_V_POS_CLOSED")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_EJECTOR_V_POS_OPEN", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_EJECTOR_V_POS_OPEN")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_EJECTOR_V_POS", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_EJECTOR_V_POS")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_PACK_TEMP_V_POS_CLOSED", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_PACK_TEMP_V_POS_CLOSED")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_COMPR_INLET_TEMP", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_COMPR_INLET_TEMP")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_DistrA_INLET_OVERTEMP", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_DistrA_INLET_OVERTEMP")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_CAB_INLET_TEMP", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_CAB_INLET_TEMP")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_DistrA_INLET_TEMP", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_DistrA_INLET_TEMP")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_PACK_TEMP", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_PACK_TEMP")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_CABIN_FLOW_SIGN", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_CABIN_FLOW_SIGN")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_CABIN_PRESS", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_CABIN_PRESS")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_DisrtA_FLOW", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_DisrtA_FLOW")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_RAMAIR_V_POS_OPEN", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_RAMAIR_V_POS_OPEN")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_DistrB_V_POS_CLOSED", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_DistrB_V_POS_CLOSED")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_DistrB_V_POS_OPEN", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_DistrB_V_POS_OPEN")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_PACK_PRESS_V_POS_CLOSED", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_PACK_PRESS_V_POS_CLOSED")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Meas_RAMAIR_V_POS_CLOSED", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_RAMAIR_V_POS_CLOSED")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Altitude", "AircraftVehicleDemonstrator.root.bC.Alt")
oms_addConnection("AircraftVehicleDemonstrator.root.eCS_SW.Mach", "AircraftVehicleDemonstrator.root.bC.Mach")

-- connections (input to engine)
oms_addConnection("AircraftVehicleDemonstrator.root.engine.Mach", "AircraftVehicleDemonstrator.root.bC.Mach")
oms_addConnection("AircraftVehicleDemonstrator.root.engine.Altitude", "AircraftVehicleDemonstrator.root.bC.Alt")
oms_addConnection("AircraftVehicleDemonstrator.root.engine.Tamb", "AircraftVehicleDemonstrator.root.atmosphere_Model_FMU_sf.Out_t_oat")
oms_addConnection("AircraftVehicleDemonstrator.root.engine.Pamb", "AircraftVehicleDemonstrator.root.atmosphere_Model_FMU_sf.Out_p_oap")
oms_addConnection("AircraftVehicleDemonstrator.root.engine.Tfrict", "AircraftVehicleDemonstrator.root.atmosphere_Model_FMU_sf.Out_t_stag")
oms_addConnection("AircraftVehicleDemonstrator.root.engine.Psat", "AircraftVehicleDemonstrator.root.atmosphere_Model_FMU_sf.Out_psat_a")
oms_addConnection("AircraftVehicleDemonstrator.root.engine.pB.p", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortBleedA.p")
oms_addConnection("AircraftVehicleDemonstrator.root.engine.pB.h", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortBleedA.h")
oms_addConnection("AircraftVehicleDemonstrator.root.engine.pB.xs", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortBleedA.xs")
oms_addConnection("AircraftVehicleDemonstrator.root.engine.pB.xw", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortBleedA.xw")

-- connections (input to consumer_A)
oms_addConnection("AircraftVehicleDemonstrator.root.consumer_A.pin.p", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortConsumerA.p")
oms_addConnection("AircraftVehicleDemonstrator.root.consumer_A.pin.h", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortConsumerA.h")
oms_addConnection("AircraftVehicleDemonstrator.root.consumer_A.pin.xs", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortConsumerA.xs")
oms_addConnection("AircraftVehicleDemonstrator.root.consumer_A.pin.xw", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortConsumerA.xw")

-- connections (input to consumer_B)
oms_addConnection("AircraftVehicleDemonstrator.root.consumer_B.pin.p", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortConsumerB.p")
oms_addConnection("AircraftVehicleDemonstrator.root.consumer_B.pin.h", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortConsumerB.h")
oms_addConnection("AircraftVehicleDemonstrator.root.consumer_B.pin.xs", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortConsumerB.xs")
oms_addConnection("AircraftVehicleDemonstrator.root.consumer_B.pin.xw", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortConsumerB.xw")

-- connections (input to cockpit)
oms_addConnection("AircraftVehicleDemonstrator.root.cockpit.portA.p", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortCockpitA.p")
oms_addConnection("AircraftVehicleDemonstrator.root.cockpit.portA.h", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortCockpitA.h")
oms_addConnection("AircraftVehicleDemonstrator.root.cockpit.portA.xs", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortCockpitA.xs")
oms_addConnection("AircraftVehicleDemonstrator.root.cockpit.portA.xw", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortCockpitA.xw")
oms_addConnection("AircraftVehicleDemonstrator.root.cockpit.portB.p", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortCockpitB.p")
oms_addConnection("AircraftVehicleDemonstrator.root.cockpit.portB.h", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortCockpitB.h")
oms_addConnection("AircraftVehicleDemonstrator.root.cockpit.portB.xs", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortCockpitB.xs")
oms_addConnection("AircraftVehicleDemonstrator.root.cockpit.portB.xw", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.PortCockpitB.xw")

-- connections (input to atmosphere_Model_FMU_sf)
oms_addConnection("AircraftVehicleDemonstrator.root.atmosphere_Model_FMU_sf.Mach", "AircraftVehicleDemonstrator.root.bC.Mach")
oms_addConnection("AircraftVehicleDemonstrator.root.atmosphere_Model_FMU_sf.Alt", "AircraftVehicleDemonstrator.root.bC.Alt")

-- simulation settings
oms_setTolerance("AircraftVehicleDemonstrator.root", 1e-6)
oms_setStopTime("AircraftVehicleDemonstrator", 1)

oms_instantiate("AircraftVehicleDemonstrator")

-- set parameters
oms_setReal("AircraftVehicleDemonstrator.root.atmosphere_Model_FMU_sf.Parameters.Atmos.Value", 6) -- Hot climate

oms_removeSignalsFromResults("AircraftVehicleDemonstrator", ".*")
oms_addSignalsToResults("AircraftVehicleDemonstrator", "AircraftVehicleDemonstrator\\.root\\.(engine\\.pB\\.p|eCS_Generic_Export\\.Meas_PACK_TEMP|eCS_Generic_Export\\.Meas_PACK_PRESS)")
oms_initialize("AircraftVehicleDemonstrator")

oms_setFixedStepSize("AircraftVehicleDemonstrator.root", 1e-4)
oms_stepUntil("AircraftVehicleDemonstrator", 1e-2)
oms_setFixedStepSize("AircraftVehicleDemonstrator.root", 1e-3)
oms_simulate("AircraftVehicleDemonstrator")

oms_terminate("AircraftVehicleDemonstrator")
oms_delete("AircraftVehicleDemonstrator")

vars = {"AircraftVehicleDemonstrator.root.engine.pB.p", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_PACK_TEMP", "AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_PACK_PRESS"}
for _,var in ipairs(vars) do
  if 1 == oms_compareSimulationResults("AircraftVehicleDemonstrator_res.mat", "../references/AircraftVehicleDemonstrator.mat", var, 1e-2, 1e-4) then
    print("info:    " .. var .. " is equal")
  else
    print("warning: " .. var .. " is not equal")
  end
end

-- Result:
-- warning: [cockpit: resources/0003_cockpit.fmu] The FMU lists 16 state derivatives but actually exposes 18 state derivatives.
--          The following derivatives are missing: 33: concealed15, 37: concealed19
-- warning: [cockpit: resources/0003_cockpit.fmu] The dependencies of the initial unknowns defined in the FMU are ignored because the flag --ignoreInitialUnknowns is active. Instead, all the initial unknowns will depend on all inputs.
-- warning: [consumer_A: resources/0004_consumer_A.fmu] The dependencies of the initial unknowns defined in the FMU are ignored because the flag --ignoreInitialUnknowns is active. Instead, all the initial unknowns will depend on all inputs.
-- warning: [consumer_B: resources/0005_consumer_B.fmu] The dependencies of the initial unknowns defined in the FMU are ignored because the flag --ignoreInitialUnknowns is active. Instead, all the initial unknowns will depend on all inputs.
-- warning: [eCS_Generic_Export: resources/0006_eCS_Generic_Export.fmu] The FMU lists 738 state derivatives but actually exposes 811 state derivatives.
--          The following derivatives are missing: 135: concealed19, 147: concealed31, 169: concealed53, 179: concealed63, 189: concealed73, 225: concealed109, 241: concealed125, 253: concealed137, 275: concealed159, 329: concealed213, 377: concealed261, 385: concealed269, 395: concealed279, 411: concealed295, 431: concealed315, 441: concealed325, 451: concealed335, 457: concealed341, 473: concealed357, 483: concealed367, 491: concealed375, 503: concealed387, 515: concealed399, 525: concealed409, 579: concealed463, 619: concealed503, 667: concealed551, 675: concealed559, 685: concealed569, 701: concealed585, 721: concealed605, 757: concealed641, 767: concealed651, 809: concealed693, 851: concealed735, 861: concealed745, 893: concealed777, 923: concealed807, 931: concealed815, 959: concealed843, 979: concealed863, 1021: concealed905, 1061: concealed945, 1069: concealed953, 1079: concealed963, 1095: concealed979, 1109: concealed993, 1123: concealed1007, 1137: concealed1021, 1151: concealed1035, 1165: concealed1049, 1175: concealed1059, 1185: concealed1069, 1227: concealed1111, 1261: concealed1145, 1269: concealed1153, 1281: concealed1165, 1291: concealed1175, 1311: concealed1195, 1323: concealed1207, 1333: concealed1217, 1373: concealed1257, 1403: concealed1287, 1415: concealed1299, 1441: concealed1325, 1457: concealed1341, 1487: concealed1371, 1507: concealed1391, 1549: concealed1433, 1559: concealed1443, 1569: concealed1453, 1579: concealed1463, 1589: concealed1473
-- warning: [eCS_Generic_Export: resources/0006_eCS_Generic_Export.fmu] The dependencies of the initial unknowns defined in the FMU are ignored because the flag --ignoreInitialUnknowns is active. Instead, all the initial unknowns will depend on all inputs.
-- warning: [eCS_SW: resources/0007_eCS_SW.fmu] The dependencies of the initial unknowns defined in the FMU are ignored because the flag --ignoreInitialUnknowns is active. Instead, all the initial unknowns will depend on all inputs.
-- warning: [engine: resources/0008_engine.fmu] The dependencies of the initial unknowns defined in the FMU are ignored because the flag --ignoreInitialUnknowns is active. Instead, all the initial unknowns will depend on all inputs.
-- warning: Alg. loop (size 80)
-- info:    Result file: AircraftVehicleDemonstrator_res.mat (bufferSize=10)
-- info:    AircraftVehicleDemonstrator.root.engine.pB.p is equal
-- info:    AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_PACK_TEMP is equal
-- info:    AircraftVehicleDemonstrator.root.eCS_Generic_Export.Meas_PACK_PRESS is equal
-- info:    9 warnings
-- info:    0 errors
-- endResult
