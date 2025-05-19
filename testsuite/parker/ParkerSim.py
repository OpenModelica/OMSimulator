## status: correct
## teardown_command: rm -rf tmp-ParkerSim/
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

import logging
logging.basicConfig(level=logging.INFO)

from OMSimulator import SSD, SSP, Settings

Settings.suppressPath = True

model = SSP('../resources/ParkerSimPack.ssp', temp_dir="./tmp-ParkerSim/")

print('\nAvailable variants:')
print(sorted(model.variants.keys()))

print('\nActive variant:')
model.activeVariant.list()

print('\nVariant D16_aero_stimuli:')
model.variants['D16_aero_stimuli'].list()

## Result:
## INFO:OMSimulator.ssp:Temporary directory created: <hidden>
##
## Available variants:
## ['D16_aero', 'D16_aero_stimuli', 'D16_auto', 'D16_auto_stimuli']
##
## Active variant:
##  <class 'OMSimulator.ssd.SSD'>
##  Variant "D16_auto": <hidden>
##  |-- System: D16_auto 'Demonstrator 1.6 automotive model (for integration in vehicle model)'
##  |-- |-- Connectors:
##  |-- |-- |-- (theta, Causality.output, SignalType.Real, deg, 'Wheel angle')
##  |-- |-- |-- (theta_ref, Causality.input, SignalType.Real, deg, 'Wheel angle (reference)')
##  |-- |-- |-- (Debug, Causality.output, SignalType.Real, -, 'Valve debug channel (0 = OK)')
##  |-- |-- Elements:
##  |-- |-- |-- FMU: interface_model 'None'
##  |-- |-- |-- |-- path: resources/FMU_16_interface.fmu
##  |-- |-- |-- |-- Connectors:
##  |-- |-- |-- |-- |-- (pA.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (pB.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (pLin.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (pLmax.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (pP.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (pPS.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (pTA.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (pTB.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (p_TP.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (qA.out.y, Causality.input, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (qB.out.y, Causality.input, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (theta.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (theta_ref.out.y, Causality.input, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (uA.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (uB.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- |-- (Real timestep, 0.001, s, 'Hopsan time step')
##  |-- |-- |-- |-- |-- (Real s_cyl, 1.0, m, 'cylinder stroke')
##  |-- |-- |-- |-- |-- (Real th_min, -15.0, deg, 'Min angle')
##  |-- |-- |-- |-- |-- (Real th_max, 15.0, deg, 'Max angle')
##  |-- |-- |-- |-- |-- (Real k_p, 130.0, -, 'Controller gain')
##  |-- |-- |-- |-- |-- (Real pT, 1.0, bar, 'Tank pressure')
##  |-- |-- |-- |-- |-- (Real pPump, 100.0, bar, 'Pump pressure')
##  |-- |-- |-- |-- |-- UnitDefinitions:
##  |-- |-- |-- |-- |-- |-- Unit: s
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: s: 1, factor: 1.0, offset: 0.0
##  |-- |-- |-- |-- |-- |-- Unit: m
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: m: 1, factor: 1e-3, offset: 0.0
##  |-- |-- |-- |-- |-- |-- Unit: deg
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: rad: 1, factor: 57.29746, offset: 0.0
##  |-- |-- |-- |-- |-- |-- Unit: -
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: factor: 1, offset: 0.0
##  |-- |-- |-- |-- |-- |-- Unit: bar
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: kg: 1, m: -2, factor: 1e5, offset: 0.0
##  |-- |-- |-- FMU: valve_model 'None'
##  |-- |-- |-- |-- path: resources/L90LS_UD_OS_OS.fmu
##  |-- |-- |-- |-- Connectors:
##  |-- |-- |-- |-- |-- (uA, Causality.input, SignalType.Real, mA, 'Valve input A')
##  |-- |-- |-- |-- |-- (uB, Causality.input, SignalType.Real, mA, 'Valve input B')
##  |-- |-- |-- |-- |-- (pP, Causality.input, SignalType.Real, bar, 'Pump pressure')
##  |-- |-- |-- |-- |-- (pTA, Causality.input, SignalType.Real, bar, 'Tank port A pressure')
##  |-- |-- |-- |-- |-- (pTB, Causality.input, SignalType.Real, bar, 'Tank port B pressure')
##  |-- |-- |-- |-- |-- (pA, Causality.input, SignalType.Real, bar, 'Port A pressure')
##  |-- |-- |-- |-- |-- (pB, Causality.input, SignalType.Real, bar, 'Port B pressure')
##  |-- |-- |-- |-- |-- (pLin, Causality.input, SignalType.Real, bar, 'Input load pressure')
##  |-- |-- |-- |-- |-- (pLmax, Causality.input, SignalType.Real, bar, 'Max load pressure')
##  |-- |-- |-- |-- |-- (qA, Causality.output, SignalType.Real, L/min, 'Port A flow')
##  |-- |-- |-- |-- |-- (qB, Causality.output, SignalType.Real, L/min, 'Port B flow')
##  |-- |-- |-- |-- |-- (qP, Causality.output, SignalType.Real, L/min, 'Pump flow')
##  |-- |-- |-- |-- |-- (qTA, Causality.output, SignalType.Real, L/min, 'Tankport A flow')
##  |-- |-- |-- |-- |-- (qTB, Causality.output, SignalType.Real, L/min, 'Tankport B flow')
##  |-- |-- |-- |-- |-- (pL, Causality.output, SignalType.Real, bar, 'Load pressure')
##  |-- |-- |-- |-- |-- (p_TP, Causality.input, SignalType.Real, bar, 'Pilot tank pressure')
##  |-- |-- |-- |-- |-- (p_PS, Causality.input, SignalType.Real, bar, 'Pilot pressure supply')
##  |-- |-- |-- |-- |-- (Debug, Causality.output, SignalType.Real, None, 'Debug channel (0 = OK)')
##  |-- |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- |-- (Real d_A, 2.0, mm, 'Pilot A damping orifice diameter')
##  |-- |-- |-- |-- |-- (Real d_B, 2.0, mm, 'Pilot B damping orifice diameter')
##  |-- |-- |-- |-- |-- (Real KqMO_PABT_gain, 1.0, -, 'Flow trim coefficient Meter-out P->A')
##  |-- |-- |-- |-- |-- (Real KqMI_PABT_gain, 1.0, -, 'Flow trim coefficient Meter-in P->A')
##  |-- |-- |-- |-- |-- (Real KqMO_PBAT_gain, 1.0, -, 'Flow trim coefficient Meter-out P->B')
##  |-- |-- |-- |-- |-- (Real KqMI_PBAT_gain, 1.0, -, 'Flow trim coefficient Meter-in P->B')
##  |-- |-- |-- |-- |-- UnitDefinitions:
##  |-- |-- |-- |-- |-- |-- Unit: mm
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: m: 1, factor: 1e-3
##  |-- |-- |-- |-- |-- |-- Unit: -
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: factor: 1, offset: 0
##  |-- |-- Connections:
##  |-- |-- |-- interface_model.pLin.in.y -> valve_model.pLin
##  |-- |-- |-- interface_model.pLmax.in.y -> valve_model.pLmax
##  |-- |-- |-- interface_model.pP.in.y -> valve_model.pP
##  |-- |-- |-- interface_model.pA.in.y -> valve_model.pA
##  |-- |-- |-- interface_model.pB.in.y -> valve_model.pB
##  |-- |-- |-- interface_model.pPS.in.y -> valve_model.p_PS
##  |-- |-- |-- interface_model.pTA.in.y -> valve_model.pTA
##  |-- |-- |-- interface_model.pTB.in.y -> valve_model.pTB
##  |-- |-- |-- interface_model.p_TP.in.y -> valve_model.p_TP
##  |-- |-- |-- interface_model.uA.in.y -> valve_model.uA
##  |-- |-- |-- interface_model.uB.in.y -> valve_model.uB
##  |-- |-- |-- valve_model.qA -> interface_model.qA.out.y
##  |-- |-- |-- valve_model.qB -> interface_model.qB.out.y
##  |-- |-- |-- .theta_ref -> interface_model.theta_ref.out.y
##  |-- |-- |-- valve_model.Debug -> .Debug
##  |-- |-- |-- interface_model.theta.in.y -> .theta
##  |-- |-- Solver Settings:
##  |-- |-- |-- ()
##  UnitDefinitions:
##  |-- Unit: mA
##  |-- |-- BaseUnit: A: 1, factor: 1e-3
##  |-- Unit: bar
##  |-- |-- BaseUnit: kg: 1, m: -1, s: -2, factor: 1e5, offset: 0
##  |-- Unit: L/min
##  |-- |-- BaseUnit: m: 3, s: -1, factor: 6e-4
##  |-- Unit: deg
##  |-- |-- BaseUnit: rad: 1, factor: 57.29746, offset: 0.0
##  |-- Unit: -
##  |-- |-- BaseUnit: factor: 1, offset: 0.0
##  DefaultExperiment
##  |-- startTime: 0.0
##  |-- stopTime: 10.0
##
## Variant D16_aero_stimuli:
##  <class 'OMSimulator.ssd.SSD'>
##  Variant "D16_aero_stimuli": <hidden>
##  |-- System: D16_aero_stimuli 'Demonstrator 1.6 aerospace test model (ramp in input signal)'
##  |-- |-- Connectors:
##  |-- |-- |-- (m_dot_TB, Causality.output, SignalType.Real, kgps, 'Valve mass flow')
##  |-- |-- |-- (u, Causality.output, SignalType.Real, -, 'Valve input signal')
##  |-- |-- |-- (p1, Causality.output, SignalType.Real, bar, 'Valve upstream pressure')
##  |-- |-- |-- (Debug, Causality.output, SignalType.Real, -, 'Valve debug channel (0 = OK)')
##  |-- |-- Elements:
##  |-- |-- |-- FMU: stimuli_model 'None'
##  |-- |-- |-- |-- path: resources/FMU_16_aero_stimuli.fmu
##  |-- |-- |-- |-- Connectors:
##  |-- |-- |-- |-- |-- (p1.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (p2.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (u.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- |-- (Real timestep, 0.001, s, 'Hopsan time step')
##  |-- |-- |-- |-- |-- (Real u_0, 0.0, -, 'u ramp base value')
##  |-- |-- |-- |-- |-- (Real u_max, 1.0, -, 'u ramp end value')
##  |-- |-- |-- |-- |-- (Real p1_max, 100.0, bar, 'p1 Ramp max value')
##  |-- |-- |-- |-- |-- (Real p2_const, 1.0, bar, 'p2 pressure (constant)')
##  |-- |-- |-- |-- |-- UnitDefinitions:
##  |-- |-- |-- |-- |-- |-- Unit: s
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: s: 1, factor: 1.0, offset: 0.0
##  |-- |-- |-- |-- |-- |-- Unit: -
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: factor: 1, offset: 0.0
##  |-- |-- |-- |-- |-- |-- Unit: bar
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: kg: 1, m: -2, factor: 1e5, offset: 0.0
##  |-- |-- |-- FMU: interface_model 'None'
##  |-- |-- |-- |-- path: resources/FMU_16_aero_interface.fmu
##  |-- |-- |-- |-- Connectors:
##  |-- |-- |-- |-- |-- (m_dot_TB.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (pA.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (pLin.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (pLmax.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (p_P.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (p_PS.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (p_TA.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (p_TP.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (qTB.out.y, Causality.input, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (u.out.y, Causality.input, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (uA.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- |-- (uB.in.y, Causality.output, SignalType.Real, None, 'None')
##  |-- |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- |-- (Real timestep, 0.001, s, 'Hopsan time step')
##  |-- |-- |-- |-- |-- (Real pT, 1.0, bar, 'Tank pressure')
##  |-- |-- |-- |-- |-- (Real pP, 1.0, bar, 'Pump pressure')
##  |-- |-- |-- |-- |-- (Real rho, 1000.0, kg/m3, 'Liquid density')
##  |-- |-- |-- |-- |-- (Real uB_const, 0.0, mA, 'Valve input B')
##  |-- |-- |-- |-- |-- (Real p_PS_const, 35.0, bar, 'Pilot supply pressure')
##  |-- |-- |-- |-- |-- UnitDefinitions:
##  |-- |-- |-- |-- |-- |-- Unit: s
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: s: 1, factor: 1.0, offset: 0.0
##  |-- |-- |-- |-- |-- |-- Unit: bar
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: kg: 1, m: -2, factor: 1e5, offset: 0.0
##  |-- |-- |-- |-- |-- |-- Unit: kg/m3
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: kg: 1, m: -3, factor: 1, offset: 0.0
##  |-- |-- |-- |-- |-- |-- Unit: mA
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: A: 1, factor: 1e-3, offset: 0.0
##  |-- |-- |-- FMU: valve_model 'None'
##  |-- |-- |-- |-- path: resources/L90LS_UD_OS_OS.fmu
##  |-- |-- |-- |-- Connectors:
##  |-- |-- |-- |-- |-- (uA, Causality.input, SignalType.Real, mA, 'Valve input A')
##  |-- |-- |-- |-- |-- (uB, Causality.input, SignalType.Real, mA, 'Valve input B')
##  |-- |-- |-- |-- |-- (pP, Causality.input, SignalType.Real, bar, 'Pump pressure')
##  |-- |-- |-- |-- |-- (pTA, Causality.input, SignalType.Real, bar, 'Tank port A pressure')
##  |-- |-- |-- |-- |-- (pTB, Causality.input, SignalType.Real, bar, 'Tank port B pressure')
##  |-- |-- |-- |-- |-- (pA, Causality.input, SignalType.Real, bar, 'Port A pressure')
##  |-- |-- |-- |-- |-- (pB, Causality.input, SignalType.Real, bar, 'Port B pressure')
##  |-- |-- |-- |-- |-- (pLin, Causality.input, SignalType.Real, bar, 'Input load pressure')
##  |-- |-- |-- |-- |-- (pLmax, Causality.input, SignalType.Real, bar, 'Max load pressure')
##  |-- |-- |-- |-- |-- (qA, Causality.output, SignalType.Real, L/min, 'Port A flow')
##  |-- |-- |-- |-- |-- (qB, Causality.output, SignalType.Real, L/min, 'Port B flow')
##  |-- |-- |-- |-- |-- (qP, Causality.output, SignalType.Real, L/min, 'Pump flow')
##  |-- |-- |-- |-- |-- (qTA, Causality.output, SignalType.Real, L/min, 'Tankport A flow')
##  |-- |-- |-- |-- |-- (qTB, Causality.output, SignalType.Real, L/min, 'Tankport B flow')
##  |-- |-- |-- |-- |-- (pL, Causality.output, SignalType.Real, bar, 'Load pressure')
##  |-- |-- |-- |-- |-- (p_TP, Causality.input, SignalType.Real, bar, 'Pilot tank pressure')
##  |-- |-- |-- |-- |-- (p_PS, Causality.input, SignalType.Real, bar, 'Pilot pressure supply')
##  |-- |-- |-- |-- |-- (Debug, Causality.output, SignalType.Real, None, 'Debug channel (0 = OK)')
##  |-- |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- |-- (Real d_A, 2.0, mm, 'Pilot A damping orifice diameter')
##  |-- |-- |-- |-- |-- (Real d_B, 2.0, mm, 'Pilot B damping orifice diameter')
##  |-- |-- |-- |-- |-- (Real KqMO_PABT_gain, 1.0, -, 'Flow trim coefficient Meter-out P->A')
##  |-- |-- |-- |-- |-- (Real KqMI_PABT_gain, 1.0, -, 'Flow trim coefficient Meter-in P->A')
##  |-- |-- |-- |-- |-- (Real KqMO_PBAT_gain, 1.0, -, 'Flow trim coefficient Meter-out P->B')
##  |-- |-- |-- |-- |-- (Real KqMI_PBAT_gain, 1.0, -, 'Flow trim coefficient Meter-in P->B')
##  |-- |-- |-- |-- |-- UnitDefinitions:
##  |-- |-- |-- |-- |-- |-- Unit: mm
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: m: 1, factor: 1e-3
##  |-- |-- |-- |-- |-- |-- Unit: -
##  |-- |-- |-- |-- |-- |-- |-- BaseUnit: factor: 1, offset: 0
##  |-- |-- Connections:
##  |-- |-- |-- interface_model.pA.in.y -> valve_model.pA
##  |-- |-- |-- interface_model.pLmax.in.y -> valve_model.pLmax
##  |-- |-- |-- interface_model.pLin.in.y -> valve_model.pLin
##  |-- |-- |-- interface_model.p_P.in.y -> valve_model.pP
##  |-- |-- |-- interface_model.p_PS.in.y -> valve_model.p_PS
##  |-- |-- |-- interface_model.p_TA.in.y -> valve_model.pTA
##  |-- |-- |-- interface_model.p_TP.in.y -> valve_model.p_TP
##  |-- |-- |-- interface_model.uA.in.y -> valve_model.uA
##  |-- |-- |-- interface_model.uB.in.y -> valve_model.uB
##  |-- |-- |-- valve_model.qTB -> interface_model.qTB.out.y
##  |-- |-- |-- interface_model.m_dot_TB.in.y -> .m_dot_TB
##  |-- |-- |-- stimuli_model.u.in.y -> interface_model.u.out.y
##  |-- |-- |-- stimuli_model.p1.in.y -> valve_model.pB
##  |-- |-- |-- stimuli_model.p2.in.y -> valve_model.pTB
##  |-- |-- |-- valve_model.Debug -> .Debug
##  |-- |-- |-- stimuli_model.u.in.y -> .u
##  |-- |-- |-- stimuli_model.p1.in.y -> .p1
##  |-- |-- Solver Settings:
##  |-- |-- |-- ()
##  UnitDefinitions:
##  |-- Unit: mA
##  |-- |-- BaseUnit: A: 1, factor: 1e-3
##  |-- Unit: bar
##  |-- |-- BaseUnit: kg: 1, m: -1, s: -2, factor: 1e5, offset: 0
##  |-- Unit: L/min
##  |-- |-- BaseUnit: m: 3, s: -1, factor: 6e-4
##  |-- Unit: kgps
##  |-- |-- BaseUnit: kg: 1, s: -1, factor: 1, offset: 0.0
##  |-- Unit: -
##  |-- |-- BaseUnit: factor: 1, offset: 0.0
##  DefaultExperiment
##  |-- startTime: 0.0
##  |-- stopTime: 20.0
## INFO:OMSimulator.ssp:Temporary directory removed: <hidden>
## endResult
