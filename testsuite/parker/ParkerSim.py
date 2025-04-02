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
##  |-- |-- (theta, Causality.output, SignalType.Real)
##  |-- |-- (theta_ref, Causality.input, SignalType.Real)
##  |-- |-- (Debug, Causality.output, SignalType.Real)
##  |-- Elements:
##  |-- |-- FMU: (interface_model)
##  |-- |-- |-- path: resources/FMU_16_interface.fmu
##  |-- |-- |-- Connectors:
##  |-- |-- |-- |-- (pA.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (pB.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (pLin.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (pLmax.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (pP.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (pPS.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (pTA.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (pTB.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (p_TP.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (qA.out.y, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (qB.out.y, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (theta.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (theta_ref.out.y, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (uA.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (uB.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- (Real timestep, 0.001, s)
##  |-- |-- |-- |-- (Real s_cyl, 1.0, m)
##  |-- |-- |-- |-- (Real th_min, -15.0, deg)
##  |-- |-- |-- |-- (Real th_max, 15.0, deg)
##  |-- |-- |-- |-- (Real k_p, 130.0, -)
##  |-- |-- |-- |-- (Real pT, 1.0, bar)
##  |-- |-- |-- |-- (Real pPump, 100.0, bar)
##  |-- |-- FMU: (valve_model)
##  |-- |-- |-- path: resources/L90LS_UD_OS_OS.fmu
##  |-- |-- |-- Connectors:
##  |-- |-- |-- |-- (uA, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (uB, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (pP, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (pTA, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (pTB, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (pA, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (pB, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (pLin, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (pLmax, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (qA, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (qB, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (qP, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (qTA, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (qTB, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (pL, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (p_TP, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (p_PS, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (Debug, Causality.output, SignalType.Real)
##  |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- (Real d_A, 2.0, mm)
##  |-- |-- |-- |-- (Real d_B, 2.0, mm)
##  |-- |-- |-- |-- (Real KqMO_PABT_gain, 1.0, -)
##  |-- |-- |-- |-- (Real KqMI_PABT_gain, 1.0, -)
##  |-- |-- |-- |-- (Real KqMO_PBAT_gain, 1.0, -)
##  |-- |-- |-- |-- (Real KqMI_PBAT_gain, 1.0, -)
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
##  |-- |-- A: 1
##  |-- |-- factor: 1e-3
##  |-- Unit: bar
##  |-- |-- kg: 1
##  |-- |-- m: -1
##  |-- |-- s: -2
##  |-- |-- factor: 1e5
##  |-- |-- offset: 0
##  |-- Unit: L/min
##  |-- |-- m: 3
##  |-- |-- s: -1
##  |-- |-- factor: 6e-4
##  |-- Unit: deg
##  |-- |-- rad: 1
##  |-- |-- factor: 57.29746
##  |-- |-- offset: 0.0
##  |-- Unit: -
##  |-- |-- factor: 1
##  |-- |-- offset: 0.0
##  DefaultExperiment
##  |-- startTime: 0.0
##  |-- stopTime: 10.0
##
## Variant D16_aero_stimuli:
##  <class 'OMSimulator.ssd.SSD'>
##  Inactive variant "D16_aero_stimuli": <hidden>
##  |-- System: D16_aero_stimuli
##  |-- Connectors:
##  |-- |-- (m_dot_TB, Causality.output, SignalType.Real)
##  |-- |-- (u, Causality.output, SignalType.Real)
##  |-- |-- (p1, Causality.output, SignalType.Real)
##  |-- |-- (Debug, Causality.output, SignalType.Real)
##  |-- Elements:
##  |-- |-- FMU: (stimuli_model)
##  |-- |-- |-- path: resources/FMU_16_aero_stimuli.fmu
##  |-- |-- |-- Connectors:
##  |-- |-- |-- |-- (p1.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (p2.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (u.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- (Real timestep, 0.001, s)
##  |-- |-- |-- |-- (Real u_0, 0.0, -)
##  |-- |-- |-- |-- (Real u_max, 1.0, -)
##  |-- |-- |-- |-- (Real p1_max, 100.0, bar)
##  |-- |-- |-- |-- (Real p2_const, 1.0, bar)
##  |-- |-- FMU: (interface_model)
##  |-- |-- |-- path: resources/FMU_16_aero_interface.fmu
##  |-- |-- |-- Connectors:
##  |-- |-- |-- |-- (m_dot_TB.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (pA.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (pLin.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (pLmax.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (p_P.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (p_PS.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (p_TA.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (p_TP.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (qTB.out.y, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (u.out.y, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (uA.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (uB.in.y, Causality.output, SignalType.Real)
##  |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- (Real timestep, 0.001, s)
##  |-- |-- |-- |-- (Real pT, 1.0, bar)
##  |-- |-- |-- |-- (Real pP, 1.0, bar)
##  |-- |-- |-- |-- (Real rho, 1000.0, kg/m3)
##  |-- |-- |-- |-- (Real uB_const, 0.0, mA)
##  |-- |-- |-- |-- (Real p_PS_const, 35.0, bar)
##  |-- |-- FMU: (valve_model)
##  |-- |-- |-- path: resources/L90LS_UD_OS_OS.fmu
##  |-- |-- |-- Connectors:
##  |-- |-- |-- |-- (uA, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (uB, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (pP, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (pTA, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (pTB, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (pA, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (pB, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (pLin, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (pLmax, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (qA, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (qB, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (qP, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (qTA, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (qTB, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (pL, Causality.output, SignalType.Real)
##  |-- |-- |-- |-- (p_TP, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (p_PS, Causality.input, SignalType.Real)
##  |-- |-- |-- |-- (Debug, Causality.output, SignalType.Real)
##  |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- (Real d_A, 2.0, mm)
##  |-- |-- |-- |-- (Real d_B, 2.0, mm)
##  |-- |-- |-- |-- (Real KqMO_PABT_gain, 1.0, -)
##  |-- |-- |-- |-- (Real KqMI_PABT_gain, 1.0, -)
##  |-- |-- |-- |-- (Real KqMO_PBAT_gain, 1.0, -)
##  |-- |-- |-- |-- (Real KqMI_PBAT_gain, 1.0, -)
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
##  |-- |-- A: 1
##  |-- |-- factor: 1e-3
##  |-- Unit: bar
##  |-- |-- kg: 1
##  |-- |-- m: -1
##  |-- |-- s: -2
##  |-- |-- factor: 1e5
##  |-- |-- offset: 0
##  |-- Unit: L/min
##  |-- |-- m: 3
##  |-- |-- s: -1
##  |-- |-- factor: 6e-4
##  |-- Unit: kgps
##  |-- |-- kg: 1
##  |-- |-- s: -1
##  |-- |-- factor: 1
##  |-- |-- offset: 0.0
##  |-- Unit: -
##  |-- |-- factor: 1
##  |-- |-- offset: 0.0
##  DefaultExperiment
##  |-- startTime: 0.0
##  |-- stopTime: 20.0
## INFO:OMSimulator.ssp:Temporary directory removed: <hidden>
## endResult
