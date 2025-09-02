## status: correct
## teardown_command:
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, SSD, CRef, Settings

Settings.suppressPath = True


# This example imports a  SSD file and then generates the internal json structure to be used in the simulation
# The SSD file is located in the resources folder of the repository
model = SSD.importFromFile('../resources/LOC/SystemStructure.ssd')
model.list()

## Result:
##  <class 'OMSimulator.ssd.SSD'>
##  Variant "LOCtest": <hidden>
##  |-- System: Root 'None'
##  |-- |-- Connectors:
##  |-- |-- Elements:
##  |-- |-- |-- FMU: System 'None'
##  |-- |-- |-- |-- path: resources/LOC_SYSTEM_linux.fmu
##  |-- |-- |-- |-- Connectors:
##  |-- |-- |-- |-- |-- (OUTPUT_temperature_lube_oil, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.9468744742359121, y:0.07873826375555514)
##  |-- |-- |-- |-- |-- (INPUT_massflow_cold_circuit, Causality.input, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.018902735641236172, y:0.0837066301679081)
##  |-- |-- |-- |-- |-- (INPUT_temperature_cold_circuit_inlet, Causality.input, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.023085590598697303, y:0.2651035501726224)
##  |-- |-- |-- |-- |-- (OUTPUT_massflow_cold_circuit, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.9686528977150072, y:0.36619396579495367)
##  |-- |-- |-- |-- |-- (INPUT_control_valve_position, Causality.input, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.012749496570096453, y:0.5584297564694264)
##  |-- |-- |-- |-- |-- (OUTPUT_temperature_cold_circuit_outlet, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.9561043328426237, y:0.7303559285820126)
##  |-- |-- |-- |-- |-- (INPUT_engine_load_0_1, Causality.input, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.03618727318202619, y:0.9160562234930608)
##  |-- |-- |-- |-- ElementGeometry:
##  |-- |-- |-- |-- |-- (x1:21.872849011536445, y1:33.03006284420393, x2:37.80181659712771, y2:48.07419782170827, rotation:0.0, icon_source:None, icon_rotation:0.0, icon_flip:False, icon_fixed_aspect_ratio:False)
##  |-- |-- |-- FMU: Control 'None'
##  |-- |-- |-- |-- path: resources/LOC_CNTRL_custom_linux.fmu
##  |-- |-- |-- |-- Connectors:
##  |-- |-- |-- |-- |-- (INPUT_temperature_lube_oil, Causality.input, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.03316604317951874, y:0.1548984805770274)
##  |-- |-- |-- |-- |-- (OUTPUT_control_valve_position, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.9672064193507739, y:0.5241030484171063)
##  |-- |-- |-- |-- |-- (SETPOINT_temperature_lube_oil, Causality.input, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.03883032824361354, y:0.8109305350494118)
##  |-- |-- |-- |-- ElementGeometry:
##  |-- |-- |-- |-- |-- (x1:57.236339801230514, y1:35.36514059188931, x2:72.72282267983405, y2:46.16015354186137, rotation:0.0, icon_source:None, icon_rotation:0.0, icon_flip:False, icon_fixed_aspect_ratio:False)
##  |-- |-- Connections:
##  |-- |-- |-- System.OUTPUT_temperature_lube_oil -> Control.INPUT_temperature_lube_oil
##  |-- |-- |-- Control.OUTPUT_control_valve_position -> System.INPUT_control_valve_position
##  |-- |-- |-- |-- ConnectionGeometry: (pointsX: [81.77574387675406, 82.38066420731379, 13.192901399544702, 13.797821730104433], pointsY: [40.98528769796143, 30.77725711976599, 30.62602703712606, 41.136517780601366])
##  |-- |-- SystemGeometry:
##  |-- |-- |-- (x1:0.0, y1:0.0, x2:100.0, y2:100.0)
##  |-- |-- Solver Settings:
##  |-- |-- |-- ()
##  UnitDefinitions:
##  |-- Unit: Kg
##  |-- |-- BaseUnit: kg: 1
##  DefaultExperiment
##  |-- startTime: 0.0
##  |-- stopTime: 1.0
## endResult
