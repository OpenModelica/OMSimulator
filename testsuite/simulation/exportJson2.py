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

model = SSD.importFromFile('../resources/dcmotor/SystemStructure.ssd')
model.list()

## Result:
##  <class 'OMSimulator.ssd.SSD'>
##  Variant "DC-Motor": <hidden>
##  |-- System: DC-Motor 'A simple simulation model of a DC-Motor that has to accelerate against a Load MLoad = 1 Nm in 1s from 0 to 1000 rad/s, Voltage U = 48 V '
##  |-- |-- Connectors:
##  |-- |-- |-- (M_mot, Causality.output, SignalType.Real, Nm, 'None')
##  |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.472716)
##  |-- |-- |-- (wB, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.575601)
##  |-- |-- |-- (U, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.802381)
##  |-- |-- |-- (I, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.391695)
##  |-- |-- |-- (M_load, Causality.output, SignalType.Real, Nm, 'None')
##  |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.236601)
##  |-- |-- Elements:
##  |-- |-- |-- System: SuT 'System under Test (DC-Motor model)'
##  |-- |-- |-- |-- Connectors:
##  |-- |-- |-- |-- |-- (wB, Causality.output, SignalType.Real, rad/s, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.741903)
##  |-- |-- |-- |-- |-- (M_load, Causality.input, SignalType.Real, Nm, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.0, y:0.434438)
##  |-- |-- |-- |-- |-- (phiB, Causality.output, SignalType.Real, rad, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.589313)
##  |-- |-- |-- |-- |-- (U, Causality.input, SignalType.Real, V, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.0, y:0.666667)
##  |-- |-- |-- |-- |-- (M_mot, Causality.output, SignalType.Real, Nm, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.413207)
##  |-- |-- |-- |-- |-- (I, Causality.output, SignalType.Real, A, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.151703)
##  |-- |-- |-- |-- ElementGeometry:
##  |-- |-- |-- |-- |-- (x1:310.0, y1:-183.0, x2:510.0, y2:-2.0, rotation:0.0, icon_source:None, icon_rotation:0.0, icon_flip:False, icon_fixed_aspect_ratio:False)
##  |-- |-- |-- |-- Elements:
##  |-- |-- |-- |-- |-- FMU: edrive_mass 'Mechanical part of the DC motor model'
##  |-- |-- |-- |-- |-- |-- path: resources/edrive_mass.fmu
##  |-- |-- |-- |-- |-- |-- Connectors:
##  |-- |-- |-- |-- |-- |-- |-- (M_A, Causality.input, SignalType.Real, N.m, 'None')
##  |-- |-- |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.0, y:0.586667)
##  |-- |-- |-- |-- |-- |-- |-- (phiB, Causality.output, SignalType.Real, rad, 'None')
##  |-- |-- |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.2)
##  |-- |-- |-- |-- |-- |-- |-- (wB, Causality.output, SignalType.Real, rad/s, 'None')
##  |-- |-- |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.4)
##  |-- |-- |-- |-- |-- |-- |-- (wA, Causality.output, SignalType.Real, rad/s, 'None')
##  |-- |-- |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.8)
##  |-- |-- |-- |-- |-- |-- |-- (M_B, Causality.input, SignalType.Real, N.m, 'None')
##  |-- |-- |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.0, y:0.333333)
##  |-- |-- |-- |-- |-- |-- |-- (phiA, Causality.output, SignalType.Real, rad, 'None')
##  |-- |-- |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.6)
##  |-- |-- |-- |-- |-- |-- ElementGeometry:
##  |-- |-- |-- |-- |-- |-- |-- (x1:318.194184, y1:-778.573683, x2:518.194184, y2:-578.573683, rotation:0.0, icon_source:None, icon_rotation:0.0, icon_flip:False, icon_fixed_aspect_ratio:False)
##  |-- |-- |-- |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- |-- |-- |-- (Real inertia.J, 0.002, kg.m2, 'Moment of inertia')
##  |-- |-- |-- |-- |-- |-- |-- (Real damper.d, 0.001, N.m.s/rad, 'Damping constant')
##  |-- |-- |-- |-- |-- |-- |-- UnitDefinitions:
##  |-- |-- |-- |-- |-- |-- |-- |-- Unit: N.m.s/rad
##  |-- |-- |-- |-- |-- |-- |-- |-- |-- BaseUnit: kg: 1, m: 2, s: -1, A: 0, K: 0, mol: 0, cd: 0, rad: -1, factor: 1.0, offset: 0.0
##  |-- |-- |-- |-- |-- |-- |-- |-- Unit: kg.m2
##  |-- |-- |-- |-- |-- |-- |-- |-- |-- BaseUnit: kg: 1, m: 2, s: 0, A: 0, K: 0, mol: 0, cd: 0, rad: 0, factor: 1.0, offset: 0.0
##  |-- |-- |-- |-- |-- FMU: emachine_model 'electrical part of the DC-Motor model'
##  |-- |-- |-- |-- |-- |-- path: resources/emachine_model.fmu
##  |-- |-- |-- |-- |-- |-- Connectors:
##  |-- |-- |-- |-- |-- |-- |-- (M, Causality.output, SignalType.Real, N.m, 'None')
##  |-- |-- |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.6)
##  |-- |-- |-- |-- |-- |-- |-- (U, Causality.input, SignalType.Real, V, 'None')
##  |-- |-- |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.0, y:0.25)
##  |-- |-- |-- |-- |-- |-- |-- (w, Causality.input, SignalType.Real, rad/s, 'None')
##  |-- |-- |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.0, y:0.75)
##  |-- |-- |-- |-- |-- |-- |-- (phi, Causality.input, SignalType.Real, rad, 'None')
##  |-- |-- |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:0.0, y:0.5)
##  |-- |-- |-- |-- |-- |-- |-- (I, Causality.output, SignalType.Real, A, 'None')
##  |-- |-- |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.1725)
##  |-- |-- |-- |-- |-- |-- ElementGeometry:
##  |-- |-- |-- |-- |-- |-- |-- (x1:-81.369197, y1:-781.383542, x2:118.630803, y2:-581.383542, rotation:0.0, icon_source:None, icon_rotation:0.0, icon_flip:False, icon_fixed_aspect_ratio:False)
##  |-- |-- |-- |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- |-- |-- |-- (Real resistor.T_ref, 300.15, K, 'Reference temperature')
##  |-- |-- |-- |-- |-- |-- |-- (Real resistor.T, 300.15, K, 'Fixed device temperature if useHeatPort = false')
##  |-- |-- |-- |-- |-- |-- |-- (Real resistor.alpha, 0.0, 1/K, 'Temperature coefficient of resistance (R_actual = R*(1 + alpha*(T_heatPort - T_ref))')
##  |-- |-- |-- |-- |-- |-- |-- (Real inductor.L, 0.001, H, 'Inductance')
##  |-- |-- |-- |-- |-- |-- |-- (Real emf.k, 0.1, N.m/A, 'Transformation coefficient')
##  |-- |-- |-- |-- |-- |-- |-- (Real resistor.R, 0.5, Ohm, 'Resistance at temperature T_ref')
##  |-- |-- |-- |-- |-- |-- |-- UnitDefinitions:
##  |-- |-- |-- |-- |-- |-- |-- |-- Unit: N.m/A
##  |-- |-- |-- |-- |-- |-- |-- |-- |-- BaseUnit: kg: 1, m: 2, s: -2, A: -1, K: 0, mol: 0, cd: 0, rad: 0, factor: 1.0, offset: 0.0
##  |-- |-- |-- |-- |-- |-- |-- |-- Unit: K
##  |-- |-- |-- |-- |-- |-- |-- |-- |-- BaseUnit: kg: 0, m: 0, s: 0, A: 0, K: 1, mol: 0, cd: 0, rad: 0, factor: 1.0, offset: 0.0
##  |-- |-- |-- |-- |-- |-- |-- |-- Unit: Ohm
##  |-- |-- |-- |-- |-- |-- |-- |-- |-- BaseUnit: kg: 1, m: 2, s: -3, A: -2, K: 0, mol: 0, cd: 0, rad: 0, factor: 1.0, offset: 0.0
##  |-- |-- |-- |-- |-- |-- |-- |-- Unit: H
##  |-- |-- |-- |-- |-- |-- |-- |-- |-- BaseUnit: kg: 1, m: 2, s: -2, A: -2, K: 0, mol: 0, cd: 0, rad: 0, factor: 1.0, offset: 0.0
##  |-- |-- |-- |-- |-- |-- |-- |-- Unit: 1/K
##  |-- |-- |-- |-- |-- |-- |-- |-- |-- BaseUnit: kg: 0, m: 0, s: 0, A: 0, K: -1, mol: 0, cd: 0, rad: 0, factor: 1.0, offset: 0.0
##  |-- |-- |-- |-- Connections:
##  |-- |-- |-- |-- |-- edrive_mass.wB -> .wB
##  |-- |-- |-- |-- |-- |-- ConnectionGeometry: (pointsX: [640.0, 640.0], pointsY: [-699.0, -534.157099])
##  |-- |-- |-- |-- |-- .M_load -> edrive_mass.M_B
##  |-- |-- |-- |-- |-- |-- ConnectionGeometry: (pointsX: [-215.0, -215.0, 242.0, 242.0], pointsY: [-719.500032, -856.0, -856.0, -712.0])
##  |-- |-- |-- |-- |-- edrive_mass.phiB -> .phiB
##  |-- |-- |-- |-- |-- |-- ConnectionGeometry: (pointsX: [666.0, 666.0], pointsY: [-739.0, -626.139856])
##  |-- |-- |-- |-- |-- .U -> emachine_model.U
##  |-- |-- |-- |-- |-- |-- ConnectionGeometry: (pointsX: [-188.0, -188.0], pointsY: [-579.510102, -731.5])
##  |-- |-- |-- |-- |-- emachine_model.M -> .M_mot
##  |-- |-- |-- |-- |-- |-- ConnectionGeometry: (pointsX: [193.0, 193.0, 684.0, 684.0], pointsY: [-661.0, -814.5, -815.0, -732.298289])
##  |-- |-- |-- |-- |-- emachine_model.I -> .I
##  |-- |-- |-- |-- |-- |-- ConnectionGeometry: (pointsX: [145.0, 145.0], pointsY: [-747.0, -889.935478])
##  |-- |-- |-- |-- |-- emachine_model.M -> edrive_mass.M_A
##  |-- |-- |-- |-- |-- edrive_mass.wA -> emachine_model.w
##  |-- |-- |-- |-- |-- |-- ConnectionGeometry: (pointsX: [565.0, 565.0, -119.0, -119.0], pointsY: [-619.0, -539.0, -538.5, -631.5])
##  |-- |-- |-- |-- |-- edrive_mass.phiA -> emachine_model.phi
##  |-- |-- |-- |-- |-- |-- ConnectionGeometry: (pointsX: [612.0, 611.5, -146.0, -146.0], pointsY: [-659.0, -504.0, -504.0, -681.0])
##  |-- |-- |-- |-- SystemGeometry:
##  |-- |-- |-- |-- |-- (x1:-281.369197, y1:-981.383542, x2:718.194184, y2:-378.573683)
##  |-- |-- |-- FMU: stimuli_model ''
##  |-- |-- |-- |-- path: resources/stimuli_model.fmu
##  |-- |-- |-- |-- Connectors:
##  |-- |-- |-- |-- |-- (U, Causality.output, SignalType.Real, V, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.544915)
##  |-- |-- |-- |-- |-- (M_load, Causality.output, SignalType.Real, N.m, 'None')
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (x:1.0, y:0.164406)
##  |-- |-- |-- |-- ElementGeometry:
##  |-- |-- |-- |-- |-- (x1:-159.5, y1:-124.0, x2:40.5, y2:-6.0, rotation:0.0, icon_source:None, icon_rotation:0.0, icon_flip:False, icon_fixed_aspect_ratio:False)
##  |-- |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- |-- (Real Voltage_step.offset, 0.0, None, 'Offset of output signal y')
##  |-- |-- |-- |-- |-- (Real Voltage_step.startTime, 0.0, s, 'Output y = offset for time < startTime')
##  |-- |-- |-- |-- |-- (Real Voltage_step.height, 12.0, V, 'Height of step')
##  |-- |-- |-- |-- |-- (Real MLoad.k, -0.5, N.m, 'Constant output value')
##  |-- |-- |-- |-- |-- UnitDefinitions:
##  |-- |-- |-- |-- |-- |-- Unit: V
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: kg: 1, m: 2, s: -3, A: -1, K: 0, mol: 0, cd: 0, rad: 0, factor: 1.0, offset: 0.0
##  |-- |-- |-- |-- |-- |-- Unit: N.m
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: kg: 1, m: 2, s: -2, A: 0, K: 0, mol: 0, cd: 0, rad: 0, factor: 1.0, offset: 0.0
##  |-- |-- |-- |-- |-- |-- Unit: s
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: kg: 0, m: 0, s: 1, A: 0, K: 0, mol: 0, cd: 0, rad: 0, factor: 1.0, offset: 0.0
##  |-- |-- Connections:
##  |-- |-- |-- SuT.M_mot -> .M_mot
##  |-- |-- |-- SuT.wB -> .wB
##  |-- |-- |-- stimuli_model.U -> .U
##  |-- |-- |-- |-- ConnectionGeometry: (pointsX: [128.0, 128.0], pointsY: [-60.0, 83.183361])
##  |-- |-- |-- SuT.I -> .I
##  |-- |-- |-- stimuli_model.M_load -> SuT.M_load
##  |-- |-- |-- stimuli_model.U -> SuT.U
##  |-- |-- |-- .M_load -> stimuli_model.M_load
##  |-- |-- |-- |-- ConnectionGeometry: (pointsX: [131.0, 131.0], pointsY: [-245.534819, -105.0])
##  |-- |-- SystemGeometry:
##  |-- |-- |-- (x1:-359.5, y1:-383.0, x2:710.0, y2:198.0)
##  UnitDefinitions:
##  |-- Unit: Nm
##  |-- |-- BaseUnit: kg: 1, m: 2, s: -2, A: 0, K: 0, mol: 0, cd: 0, rad: 0, factor: 1.0, offset: 0.0
##  |-- Unit: rad/s
##  |-- |-- BaseUnit: kg: 0, m: 0, s: -1, A: 0, K: 0, mol: 0, cd: 0, rad: 1, factor: 1.0, offset: 0.0
##  |-- Unit: V
##  |-- |-- BaseUnit: kg: 1, m: 2, s: -3, A: -1, K: 0, mol: 0, cd: 0, rad: 0, factor: 1.0, offset: 0.0
##  |-- Unit: rad
##  |-- |-- BaseUnit: kg: 0, m: 0, s: 0, A: 0, K: 0, mol: 0, cd: 0, rad: 1, factor: 1.0, offset: 0.0
##  |-- Unit: N.m
##  |-- |-- BaseUnit: kg: 1, m: 2, s: -2, A: 0, K: 0, mol: 0, cd: 0, rad: 0, factor: 1.0, offset: 0.0
##  |-- Unit: A
##  |-- |-- BaseUnit: kg: 0, m: 0, s: 0, A: 1, K: 0, mol: 0, cd: 0, rad: 0, factor: 1.0, offset: 0.0
##  DefaultExperiment
##  |-- startTime: 0.0
##  |-- stopTime: 1.0
## endResult
