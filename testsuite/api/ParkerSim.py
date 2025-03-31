## status: correct
## teardown_command: rm -rf tmp-ParkerSim/
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

import logging
logging.basicConfig(level=logging.INFO)

from OMSimulator import SSD, SSP, Settings

Settings.suppressPath = True

# This example creates a new SSP file containing a default SSD file.
# It then creates another SSD file and adds it to the SSP file using the add method.
# Finally, the SSP file is exported to a new file and re-imported.

model = SSP('../resources/ParkerSimPack.ssp', temp_dir="./tmp-ParkerSim/")

print('\n')
print('Available variants:')
print(model.variants.keys())

print('\n')
print('Active variant:')
model.activeVariant.list()

print('\n')
print('Variant D16_aero_stimuli:')
model.variants['D16_aero_stimuli'].list()

## Result:
## INFO:OMSimulator.ssp:Temporary directory created: <hidden>
##
##
## Available variants:
## dict_keys(['D16_auto', 'D16_aero_stimuli', 'D16_auto_stimuli', 'D16_aero'])
##
##
## Active variant:
##  <class 'OMSimulator.ssd.SSD'>
##  Active variant "D16_auto": <hidden>
##  |-- System: D16_auto
##  |-- Connectors:
##  |-- |-- (theta, output, Real)
##  |-- |-- (theta_ref, input, Real)
##  |-- |-- (Debug, output, Real)
##  |-- Elements:
##  |-- |-- FMU: (interface_model)
##  |-- |-- |-- path: resources/FMU_16_interface.fmu
##  |-- |-- Connectors:
##  |-- |-- |-- (pA.in.y, output, Real)
##  |-- |-- |-- (pB.in.y, output, Real)
##  |-- |-- |-- (pLin.in.y, output, Real)
##  |-- |-- |-- (pLmax.in.y, output, Real)
##  |-- |-- |-- (pP.in.y, output, Real)
##  |-- |-- |-- (pPS.in.y, output, Real)
##  |-- |-- |-- (pTA.in.y, output, Real)
##  |-- |-- |-- (pTB.in.y, output, Real)
##  |-- |-- |-- (p_TP.in.y, output, Real)
##  |-- |-- |-- (qA.out.y, input, Real)
##  |-- |-- |-- (qB.out.y, input, Real)
##  |-- |-- |-- (theta.in.y, output, Real)
##  |-- |-- |-- (theta_ref.out.y, input, Real)
##  |-- |-- |-- (uA.in.y, output, Real)
##  |-- |-- |-- (uB.in.y, output, Real)
##  |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- (Real timestep, 0.001, None)
##  |-- |-- |-- (Real s_cyl, 1.0, None)
##  |-- |-- |-- (Real th_min, -15.0, None)
##  |-- |-- |-- (Real th_max, 15.0, None)
##  |-- |-- |-- (Real k_p, 130.0, None)
##  |-- |-- |-- (Real pT, 1.0, None)
##  |-- |-- |-- (Real pPump, 100.0, None)
##  |-- |-- FMU: (valve_model)
##  |-- |-- |-- path: resources/L90LS_UD_OS_OS.fmu
##  |-- |-- Connectors:
##  |-- |-- |-- (uA, input, Real)
##  |-- |-- |-- (uB, input, Real)
##  |-- |-- |-- (pP, input, Real)
##  |-- |-- |-- (pTA, input, Real)
##  |-- |-- |-- (pTB, input, Real)
##  |-- |-- |-- (pA, input, Real)
##  |-- |-- |-- (pB, input, Real)
##  |-- |-- |-- (pLin, input, Real)
##  |-- |-- |-- (pLmax, input, Real)
##  |-- |-- |-- (qA, output, Real)
##  |-- |-- |-- (qB, output, Real)
##  |-- |-- |-- (qP, output, Real)
##  |-- |-- |-- (qTA, output, Real)
##  |-- |-- |-- (qTB, output, Real)
##  |-- |-- |-- (pL, output, Real)
##  |-- |-- |-- (p_TP, input, Real)
##  |-- |-- |-- (p_PS, input, Real)
##  |-- |-- |-- (Debug, output, Real)
##  |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- (Real d_A, 2.0, None)
##  |-- |-- |-- (Real d_B, 2.0, None)
##  |-- |-- |-- (Real KqMO_PABT_gain, 1.0, None)
##  |-- |-- |-- (Real KqMI_PABT_gain, 1.0, None)
##  |-- |-- |-- (Real KqMO_PBAT_gain, 1.0, None)
##  |-- |-- |-- (Real KqMI_PBAT_gain, 1.0, None)
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
##  |-- |-- None.theta_ref -> interface_model.theta_ref.out.y
##  |-- |-- valve_model.Debug -> None.Debug
##  |-- |-- interface_model.theta.in.y -> None.theta
##  DefaultExperiment
##  |-- startTime: 0.0
##  |-- stopTime: 10.0
##
##
## Variant D16_aero_stimuli:
##  <class 'OMSimulator.ssd.SSD'>
##  Inactive variant "D16_aero_stimuli": <hidden>
##  |-- System: D16_aero_stimuli
##  |-- Connectors:
##  |-- |-- (m_dot_TB, output, Real)
##  |-- |-- (u, output, Real)
##  |-- |-- (p1, output, Real)
##  |-- |-- (Debug, output, Real)
##  |-- Elements:
##  |-- |-- FMU: (stimuli_model)
##  |-- |-- |-- path: resources/FMU_16_aero_stimuli.fmu
##  |-- |-- Connectors:
##  |-- |-- |-- (p1.in.y, output, Real)
##  |-- |-- |-- (p2.in.y, output, Real)
##  |-- |-- |-- (u.in.y, output, Real)
##  |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- (Real timestep, 0.001, None)
##  |-- |-- |-- (Real u_0, 0.0, None)
##  |-- |-- |-- (Real u_max, 1.0, None)
##  |-- |-- |-- (Real p1_max, 100.0, None)
##  |-- |-- |-- (Real p2_const, 1.0, None)
##  |-- |-- FMU: (interface_model)
##  |-- |-- |-- path: resources/FMU_16_aero_interface.fmu
##  |-- |-- Connectors:
##  |-- |-- |-- (m_dot_TB.in.y, output, Real)
##  |-- |-- |-- (pA.in.y, output, Real)
##  |-- |-- |-- (pLin.in.y, output, Real)
##  |-- |-- |-- (pLmax.in.y, output, Real)
##  |-- |-- |-- (p_P.in.y, output, Real)
##  |-- |-- |-- (p_PS.in.y, output, Real)
##  |-- |-- |-- (p_TA.in.y, output, Real)
##  |-- |-- |-- (p_TP.in.y, output, Real)
##  |-- |-- |-- (qTB.out.y, input, Real)
##  |-- |-- |-- (u.out.y, input, Real)
##  |-- |-- |-- (uA.in.y, output, Real)
##  |-- |-- |-- (uB.in.y, output, Real)
##  |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- (Real timestep, 0.001, None)
##  |-- |-- |-- (Real pT, 1.0, None)
##  |-- |-- |-- (Real pP, 1.0, None)
##  |-- |-- |-- (Real rho, 1000.0, None)
##  |-- |-- |-- (Real uB_const, 0.0, None)
##  |-- |-- |-- (Real p_PS_const, 35.0, None)
##  |-- |-- FMU: (valve_model)
##  |-- |-- |-- path: resources/L90LS_UD_OS_OS.fmu
##  |-- |-- Connectors:
##  |-- |-- |-- (uA, input, Real)
##  |-- |-- |-- (uB, input, Real)
##  |-- |-- |-- (pP, input, Real)
##  |-- |-- |-- (pTA, input, Real)
##  |-- |-- |-- (pTB, input, Real)
##  |-- |-- |-- (pA, input, Real)
##  |-- |-- |-- (pB, input, Real)
##  |-- |-- |-- (pLin, input, Real)
##  |-- |-- |-- (pLmax, input, Real)
##  |-- |-- |-- (qA, output, Real)
##  |-- |-- |-- (qB, output, Real)
##  |-- |-- |-- (qP, output, Real)
##  |-- |-- |-- (qTA, output, Real)
##  |-- |-- |-- (qTB, output, Real)
##  |-- |-- |-- (pL, output, Real)
##  |-- |-- |-- (p_TP, input, Real)
##  |-- |-- |-- (p_PS, input, Real)
##  |-- |-- |-- (Debug, output, Real)
##  |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- (Real d_A, 2.0, None)
##  |-- |-- |-- (Real d_B, 2.0, None)
##  |-- |-- |-- (Real KqMO_PABT_gain, 1.0, None)
##  |-- |-- |-- (Real KqMI_PABT_gain, 1.0, None)
##  |-- |-- |-- (Real KqMO_PBAT_gain, 1.0, None)
##  |-- |-- |-- (Real KqMI_PBAT_gain, 1.0, None)
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
##  |-- |-- interface_model.m_dot_TB.in.y -> None.m_dot_TB
##  |-- |-- stimuli_model.u.in.y -> interface_model.u.out.y
##  |-- |-- stimuli_model.p1.in.y -> valve_model.pB
##  |-- |-- stimuli_model.p2.in.y -> valve_model.pTB
##  |-- |-- valve_model.Debug -> None.Debug
##  |-- |-- stimuli_model.u.in.y -> None.u
##  |-- |-- stimuli_model.p1.in.y -> None.p1
##  DefaultExperiment
##  |-- startTime: 0.0
##  |-- stopTime: 20.0
## INFO:OMSimulator.ssp:Temporary directory removed: <hidden>
## endResult
