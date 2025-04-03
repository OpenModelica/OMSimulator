## status: correct
## teardown_command:
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSD

ssd = SSD.importFromFile('../resources/LOC/SystemStructure.ssd')
ssd.list()

## Result:
##  <class 'OMSimulator.ssd.SSD'>
##  Inactive variant "LOCtest": /home/lochel/OMSimulator/testsuite/resources/LOC/SystemStructure.ssd
##  |-- System: Root
##  |-- Connectors:
##  |-- Elements:
##  |-- |-- FMU: (System)
##  |-- |-- |-- path: resources/LOC_SYSTEM_linux.fmu
##  |-- |-- |-- Connectors:
##  |-- |-- |-- |-- (OUTPUT_temperature_lube_oil, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- |-- ConnectorGeometry: (0.9468744742359121, 0.07873826375555514)
##  |-- |-- |-- |-- (INPUT_massflow_cold_circuit, Causality.input, SignalType.Real, None)
##  |-- |-- |-- |-- |-- ConnectorGeometry: (0.018902735641236172, 0.0837066301679081)
##  |-- |-- |-- |-- (INPUT_temperature_cold_circuit_inlet, Causality.input, SignalType.Real, None)
##  |-- |-- |-- |-- |-- ConnectorGeometry: (0.023085590598697303, 0.2651035501726224)
##  |-- |-- |-- |-- (OUTPUT_massflow_cold_circuit, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- |-- ConnectorGeometry: (0.9686528977150072, 0.36619396579495367)
##  |-- |-- |-- |-- (INPUT_control_valve_position, Causality.input, SignalType.Real, None)
##  |-- |-- |-- |-- |-- ConnectorGeometry: (0.012749496570096453, 0.5584297564694264)
##  |-- |-- |-- |-- (OUTPUT_temperature_cold_circuit_outlet, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- |-- ConnectorGeometry: (0.9561043328426237, 0.7303559285820126)
##  |-- |-- |-- |-- (INPUT_engine_load_0_1, Causality.input, SignalType.Real, None)
##  |-- |-- |-- |-- |-- ConnectorGeometry: (0.03618727318202619, 0.9160562234930608)
##  |-- |-- FMU: (Control)
##  |-- |-- |-- path: resources/LOC_CNTRL_custom_linux.fmu
##  |-- |-- |-- Connectors:
##  |-- |-- |-- |-- (INPUT_temperature_lube_oil, Causality.input, SignalType.Real, None)
##  |-- |-- |-- |-- |-- ConnectorGeometry: (0.03316604317951874, 0.1548984805770274)
##  |-- |-- |-- |-- (OUTPUT_control_valve_position, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- |-- ConnectorGeometry: (0.9672064193507739, 0.5241030484171063)
##  |-- |-- |-- |-- (SETPOINT_temperature_lube_oil, Causality.input, SignalType.Real, None)
##  |-- |-- |-- |-- |-- ConnectorGeometry: (0.03883032824361354, 0.8109305350494118)
##  |-- Connections:
##  |-- |-- System.OUTPUT_temperature_lube_oil -> Control.INPUT_temperature_lube_oil
##  |-- |-- Control.OUTPUT_control_valve_position -> System.INPUT_control_valve_position
##  UnitDefinitions:
##  |-- Unit: Kg
##  |-- |-- BaseUnit: kg: 1
##  DefaultExperiment
##  |-- startTime: 0.0
##  |-- stopTime: 1.0
## endResult
