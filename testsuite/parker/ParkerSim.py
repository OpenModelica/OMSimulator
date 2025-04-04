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
##  Active variant "D16_auto": <hidden>
##  |-- System: D16_auto
##  |-- Connectors:
##  |-- |-- (theta, Causality.output, SignalType.Real, deg)
##  |-- |-- (theta_ref, Causality.input, SignalType.Real, deg)
##  |-- |-- (Debug, Causality.output, SignalType.Real, -)
##  |-- Elements:
##  |-- |-- FMU: (interface_model)
##  |-- |-- |-- path: resources/FMU_16_interface.fmu
##  |-- |-- |-- Connectors:
##  |-- |-- |-- |-- (pA.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (pB.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (pLin.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (pLmax.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (pP.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (pPS.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (pTA.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (pTB.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (p_TP.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (qA.out.y, Causality.input, SignalType.Real, None)
##  |-- |-- |-- |-- (qB.out.y, Causality.input, SignalType.Real, None)
##  |-- |-- |-- |-- (theta.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (theta_ref.out.y, Causality.input, SignalType.Real, None)
##  |-- |-- |-- |-- (uA.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (uB.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- (Real timestep, 0.001, s)
##  |-- |-- |-- |-- (Real s_cyl, 1.0, m)
##  |-- |-- |-- |-- (Real th_min, -15.0, deg)
##  |-- |-- |-- |-- (Real th_max, 15.0, deg)
##  |-- |-- |-- |-- (Real k_p, 130.0, -)
##  |-- |-- |-- |-- (Real pT, 1.0, bar)
##  |-- |-- |-- |-- (Real pPump, 100.0, bar)
##  |-- |-- |-- |-- UnitDefinitions:
##  |-- |-- |-- |-- |-- Unit: s
##  |-- |-- |-- |-- |-- |-- BaseUnit: s: 1, factor: 1.0, offset: 0.0
##  |-- |-- |-- |-- |-- Unit: m
##  |-- |-- |-- |-- |-- |-- BaseUnit: m: 1, factor: 1e-3, offset: 0.0
##  |-- |-- |-- |-- |-- Unit: deg
##  |-- |-- |-- |-- |-- |-- BaseUnit: rad: 1, factor: 57.29746, offset: 0.0
##  |-- |-- |-- |-- |-- Unit: -
##  |-- |-- |-- |-- |-- |-- BaseUnit: factor: 1, offset: 0.0
##  |-- |-- |-- |-- |-- Unit: bar
##  |-- |-- |-- |-- |-- |-- BaseUnit: kg: 1, m: -2, factor: 1e5, offset: 0.0
##  |-- |-- FMU: (valve_model)
##  |-- |-- |-- path: resources/L90LS_UD_OS_OS.fmu
##  |-- |-- |-- Connectors:
##  |-- |-- |-- |-- (uA, Causality.input, SignalType.Real, mA)
##  |-- |-- |-- |-- (uB, Causality.input, SignalType.Real, mA)
##  |-- |-- |-- |-- (pP, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (pTA, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (pTB, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (pA, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (pB, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (pLin, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (pLmax, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (qA, Causality.output, SignalType.Real, L/min)
##  |-- |-- |-- |-- (qB, Causality.output, SignalType.Real, L/min)
##  |-- |-- |-- |-- (qP, Causality.output, SignalType.Real, L/min)
##  |-- |-- |-- |-- (qTA, Causality.output, SignalType.Real, L/min)
##  |-- |-- |-- |-- (qTB, Causality.output, SignalType.Real, L/min)
##  |-- |-- |-- |-- (pL, Causality.output, SignalType.Real, bar)
##  |-- |-- |-- |-- (p_TP, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (p_PS, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (Debug, Causality.output, SignalType.Real, None)
##  |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- (Real d_A, 2.0, mm)
##  |-- |-- |-- |-- (Real d_B, 2.0, mm)
##  |-- |-- |-- |-- (Real KqMO_PABT_gain, 1.0, -)
##  |-- |-- |-- |-- (Real KqMI_PABT_gain, 1.0, -)
##  |-- |-- |-- |-- (Real KqMO_PBAT_gain, 1.0, -)
##  |-- |-- |-- |-- (Real KqMI_PBAT_gain, 1.0, -)
##  |-- |-- |-- |-- UnitDefinitions:
##  |-- |-- |-- |-- |-- Unit: mm
##  |-- |-- |-- |-- |-- |-- BaseUnit: m: 1, factor: 1e-3
##  |-- |-- |-- |-- |-- Unit: -
##  |-- |-- |-- |-- |-- |-- BaseUnit: factor: 1, offset: 0
##  |-- Connections:
##  |-- |-- interface_model.pLin.in.y -> valve_model.pLin
##  |-- |-- interface_model.pLmax.in.y -> valve_model.pLmax
##  |-- |-- interface_model.pP.in.y -> valve_model.pP
##  |-- |-- interface_model.pA.in.y -> valve_model.pA
##  |-- |-- interface_model.pB.in.y -> valve_model.pB
##  |-- |-- interface_model.pPS.in.y -> valve_model.p_PS
##  |-- |-- interface_model.pTA.in.y -> valve_model.pTA
##  |-- |-- interface_model.pTB.in.y -> valve_model.pTB
##  |-- |-- interface_model.p_TP.in.y -> valve_model.p_TP
##  |-- |-- interface_model.uA.in.y -> valve_model.uA
##  |-- |-- interface_model.uB.in.y -> valve_model.uB
##  |-- |-- valve_model.qA -> interface_model.qA.out.y
##  |-- |-- valve_model.qB -> interface_model.qB.out.y
##  |-- |-- .theta_ref -> interface_model.theta_ref.out.y
##  |-- |-- valve_model.Debug -> .Debug
##  |-- |-- interface_model.theta.in.y -> .theta
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
##  Inactive variant "D16_aero_stimuli": <hidden>
##  |-- System: D16_aero_stimuli
##  |-- Connectors:
##  |-- |-- (m_dot_TB, Causality.output, SignalType.Real, kgps)
##  |-- |-- (u, Causality.output, SignalType.Real, -)
##  |-- |-- (p1, Causality.output, SignalType.Real, bar)
##  |-- |-- (Debug, Causality.output, SignalType.Real, -)
##  |-- Elements:
##  |-- |-- FMU: (stimuli_model)
##  |-- |-- |-- path: resources/FMU_16_aero_stimuli.fmu
##  |-- |-- |-- Connectors:
##  |-- |-- |-- |-- (p1.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (p2.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (u.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- (Real timestep, 0.001, s)
##  |-- |-- |-- |-- (Real u_0, 0.0, -)
##  |-- |-- |-- |-- (Real u_max, 1.0, -)
##  |-- |-- |-- |-- (Real p1_max, 100.0, bar)
##  |-- |-- |-- |-- (Real p2_const, 1.0, bar)
##  |-- |-- |-- |-- UnitDefinitions:
##  |-- |-- |-- |-- |-- Unit: s
##  |-- |-- |-- |-- |-- |-- BaseUnit: s: 1, factor: 1.0, offset: 0.0
##  |-- |-- |-- |-- |-- Unit: -
##  |-- |-- |-- |-- |-- |-- BaseUnit: factor: 1, offset: 0.0
##  |-- |-- |-- |-- |-- Unit: bar
##  |-- |-- |-- |-- |-- |-- BaseUnit: kg: 1, m: -2, factor: 1e5, offset: 0.0
##  |-- |-- FMU: (interface_model)
##  |-- |-- |-- path: resources/FMU_16_aero_interface.fmu
##  |-- |-- |-- Connectors:
##  |-- |-- |-- |-- (m_dot_TB.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (pA.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (pLin.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (pLmax.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (p_P.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (p_PS.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (p_TA.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (p_TP.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (qTB.out.y, Causality.input, SignalType.Real, None)
##  |-- |-- |-- |-- (u.out.y, Causality.input, SignalType.Real, None)
##  |-- |-- |-- |-- (uA.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- |-- (uB.in.y, Causality.output, SignalType.Real, None)
##  |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- (Real timestep, 0.001, s)
##  |-- |-- |-- |-- (Real pT, 1.0, bar)
##  |-- |-- |-- |-- (Real pP, 1.0, bar)
##  |-- |-- |-- |-- (Real rho, 1000.0, kg/m3)
##  |-- |-- |-- |-- (Real uB_const, 0.0, mA)
##  |-- |-- |-- |-- (Real p_PS_const, 35.0, bar)
##  |-- |-- |-- |-- UnitDefinitions:
##  |-- |-- |-- |-- |-- Unit: s
##  |-- |-- |-- |-- |-- |-- BaseUnit: s: 1, factor: 1.0, offset: 0.0
##  |-- |-- |-- |-- |-- Unit: bar
##  |-- |-- |-- |-- |-- |-- BaseUnit: kg: 1, m: -2, factor: 1e5, offset: 0.0
##  |-- |-- |-- |-- |-- Unit: kg/m3
##  |-- |-- |-- |-- |-- |-- BaseUnit: kg: 1, m: -3, factor: 1, offset: 0.0
##  |-- |-- |-- |-- |-- Unit: mA
##  |-- |-- |-- |-- |-- |-- BaseUnit: A: 1, factor: 1e-3, offset: 0.0
##  |-- |-- FMU: (valve_model)
##  |-- |-- |-- path: resources/L90LS_UD_OS_OS.fmu
##  |-- |-- |-- Connectors:
##  |-- |-- |-- |-- (uA, Causality.input, SignalType.Real, mA)
##  |-- |-- |-- |-- (uB, Causality.input, SignalType.Real, mA)
##  |-- |-- |-- |-- (pP, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (pTA, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (pTB, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (pA, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (pB, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (pLin, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (pLmax, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (qA, Causality.output, SignalType.Real, L/min)
##  |-- |-- |-- |-- (qB, Causality.output, SignalType.Real, L/min)
##  |-- |-- |-- |-- (qP, Causality.output, SignalType.Real, L/min)
##  |-- |-- |-- |-- (qTA, Causality.output, SignalType.Real, L/min)
##  |-- |-- |-- |-- (qTB, Causality.output, SignalType.Real, L/min)
##  |-- |-- |-- |-- (pL, Causality.output, SignalType.Real, bar)
##  |-- |-- |-- |-- (p_TP, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (p_PS, Causality.input, SignalType.Real, bar)
##  |-- |-- |-- |-- (Debug, Causality.output, SignalType.Real, None)
##  |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- (Real d_A, 2.0, mm)
##  |-- |-- |-- |-- (Real d_B, 2.0, mm)
##  |-- |-- |-- |-- (Real KqMO_PABT_gain, 1.0, -)
##  |-- |-- |-- |-- (Real KqMI_PABT_gain, 1.0, -)
##  |-- |-- |-- |-- (Real KqMO_PBAT_gain, 1.0, -)
##  |-- |-- |-- |-- (Real KqMI_PBAT_gain, 1.0, -)
##  |-- |-- |-- |-- UnitDefinitions:
##  |-- |-- |-- |-- |-- Unit: mm
##  |-- |-- |-- |-- |-- |-- BaseUnit: m: 1, factor: 1e-3
##  |-- |-- |-- |-- |-- Unit: -
##  |-- |-- |-- |-- |-- |-- BaseUnit: factor: 1, offset: 0
##  |-- Connections:
##  |-- |-- interface_model.pA.in.y -> valve_model.pA
##  |-- |-- interface_model.pLmax.in.y -> valve_model.pLmax
##  |-- |-- interface_model.pLin.in.y -> valve_model.pLin
##  |-- |-- interface_model.p_P.in.y -> valve_model.pP
##  |-- |-- interface_model.p_PS.in.y -> valve_model.p_PS
##  |-- |-- interface_model.p_TA.in.y -> valve_model.pTA
##  |-- |-- interface_model.p_TP.in.y -> valve_model.p_TP
##  |-- |-- interface_model.uA.in.y -> valve_model.uA
##  |-- |-- interface_model.uB.in.y -> valve_model.uB
##  |-- |-- valve_model.qTB -> interface_model.qTB.out.y
##  |-- |-- interface_model.m_dot_TB.in.y -> .m_dot_TB
##  |-- |-- stimuli_model.u.in.y -> interface_model.u.out.y
##  |-- |-- stimuli_model.p1.in.y -> valve_model.pB
##  |-- |-- stimuli_model.p2.in.y -> valve_model.pTB
##  |-- |-- valve_model.Debug -> .Debug
##  |-- |-- stimuli_model.u.in.y -> .u
##  |-- |-- stimuli_model.p1.in.y -> .p1
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
