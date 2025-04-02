## status: correct
## teardown_command: rm -rf dcmotor.ssp tmp-dcmotor/
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

## This example demonstrates how to export an SSD file from an SSP file.
## the main purpose of this example is to parse all the elements in the SSD file
## and export it to a file.

import logging
logging.basicConfig(level=logging.INFO)

from OMSimulator import SSD, SSP, Settings

Settings.suppressPath = True

model = SSP('../resources/dcmotor.ssp', temp_dir="./tmp-dcmotor/")

print('\nActive variant:')
model.activeVariant.list()

model.export('dcmotor.ssp')

## Result:
## INFO:OMSimulator.ssp:Temporary directory created: <hidden>
## INFO:OMSimulator.ssd:SSD 'DC-Motor' exported to <hidden>
## INFO:OMSimulator.ssp:SSP file 'dcmotor.ssp' successfully exported!
##
## Active variant:
##  <class 'OMSimulator.ssd.SSD'>
##  Active variant "DC-Motor": <hidden>
##  |-- System: DC-Motor
##  |-- Connectors:
##  |-- |-- (M_mot, Causality.output, SignalType.Real, Nm)
##  |-- |-- |-- ConnectorGeometry: (1.0, 0.472716)
##  |-- |-- (wB, Causality.output, SignalType.Real, None)
##  |-- |-- |-- ConnectorGeometry: (1.0, 0.575601)
##  |-- |-- (U, Causality.output, SignalType.Real, None)
##  |-- |-- |-- ConnectorGeometry: (1.0, 0.802381)
##  |-- |-- (I, Causality.output, SignalType.Real, None)
##  |-- |-- |-- ConnectorGeometry: (1.0, 0.391695)
##  |-- |-- (M_load, Causality.output, SignalType.Real, Nm)
##  |-- |-- |-- ConnectorGeometry: (1.0, 0.236601)
##  |-- Elements:
##  |-- |-- System: SuT
##  |-- |-- Connectors:
##  |-- |-- |-- (wB, Causality.output, SignalType.Real, rad/s)
##  |-- |-- |-- |-- ConnectorGeometry: (1.0, 0.741903)
##  |-- |-- |-- (M_load, Causality.input, SignalType.Real, Nm)
##  |-- |-- |-- |-- ConnectorGeometry: (0.0, 0.434438)
##  |-- |-- |-- (phiB, Causality.output, SignalType.Real, rad)
##  |-- |-- |-- |-- ConnectorGeometry: (1.0, 0.589313)
##  |-- |-- |-- (U, Causality.input, SignalType.Real, V)
##  |-- |-- |-- |-- ConnectorGeometry: (0.0, 0.666667)
##  |-- |-- |-- (M_mot, Causality.output, SignalType.Real, Nm)
##  |-- |-- |-- |-- ConnectorGeometry: (1.0, 0.413207)
##  |-- |-- |-- (I, Causality.output, SignalType.Real, A)
##  |-- |-- |-- |-- ConnectorGeometry: (1.0, 0.151703)
##  |-- |-- Elements:
##  |-- |-- |-- FMU: (edrive_mass)
##  |-- |-- |-- |-- path: resources/edrive_mass.fmu
##  |-- |-- |-- |-- Connectors:
##  |-- |-- |-- |-- |-- (M_A, Causality.input, SignalType.Real, N.m)
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (0.0, 0.586667)
##  |-- |-- |-- |-- |-- (phiB, Causality.output, SignalType.Real, rad)
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (1.0, 0.2)
##  |-- |-- |-- |-- |-- (wB, Causality.output, SignalType.Real, rad/s)
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (1.0, 0.4)
##  |-- |-- |-- |-- |-- (wA, Causality.output, SignalType.Real, rad/s)
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (1.0, 0.8)
##  |-- |-- |-- |-- |-- (M_B, Causality.input, SignalType.Real, N.m)
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (0.0, 0.333333)
##  |-- |-- |-- |-- |-- (phiA, Causality.output, SignalType.Real, rad)
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (1.0, 0.6)
##  |-- |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- |-- (Real inertia.J, 0.002, kg.m2)
##  |-- |-- |-- |-- |-- (Real damper.d, 0.001, N.m.s/rad)
##  |-- |-- |-- FMU: (emachine_model)
##  |-- |-- |-- |-- path: resources/emachine_model.fmu
##  |-- |-- |-- |-- Connectors:
##  |-- |-- |-- |-- |-- (M, Causality.output, SignalType.Real, N.m)
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (1.0, 0.6)
##  |-- |-- |-- |-- |-- (U, Causality.input, SignalType.Real, V)
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (0.0, 0.25)
##  |-- |-- |-- |-- |-- (w, Causality.input, SignalType.Real, rad/s)
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (0.0, 0.75)
##  |-- |-- |-- |-- |-- (phi, Causality.input, SignalType.Real, rad)
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (0.0, 0.5)
##  |-- |-- |-- |-- |-- (I, Causality.output, SignalType.Real, A)
##  |-- |-- |-- |-- |-- |-- ConnectorGeometry: (1.0, 0.1725)
##  |-- |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- |-- (Real resistor.T_ref, 300.15, K)
##  |-- |-- |-- |-- |-- (Real resistor.T, 300.15, K)
##  |-- |-- |-- |-- |-- (Real resistor.alpha, 0.0, 1/K)
##  |-- |-- |-- |-- |-- (Real inductor.L, 0.001, H)
##  |-- |-- |-- |-- |-- (Real emf.k, 0.1, N.m/A)
##  |-- |-- |-- |-- |-- (Real resistor.R, 0.5, Ohm)
##  |-- |-- Connections:
##  |-- |-- |-- edrive_mass.wB -> .wB
##  |-- |-- |-- .M_load -> edrive_mass.M_B
##  |-- |-- |-- edrive_mass.phiB -> .phiB
##  |-- |-- |-- .U -> emachine_model.U
##  |-- |-- |-- emachine_model.M -> .M_mot
##  |-- |-- |-- emachine_model.I -> .I
##  |-- |-- |-- emachine_model.M -> edrive_mass.M_A
##  |-- |-- |-- edrive_mass.wA -> emachine_model.w
##  |-- |-- |-- edrive_mass.phiA -> emachine_model.phi
##  |-- |-- FMU: (stimuli_model)
##  |-- |-- |-- path: resources/stimuli_model.fmu
##  |-- |-- |-- Connectors:
##  |-- |-- |-- |-- (U, Causality.output, SignalType.Real, V)
##  |-- |-- |-- |-- |-- ConnectorGeometry: (1.0, 0.544915)
##  |-- |-- |-- |-- (M_load, Causality.output, SignalType.Real, N.m)
##  |-- |-- |-- |-- |-- ConnectorGeometry: (1.0, 0.164406)
##  |-- |-- |-- Inline Parameter Bindings:
##  |-- |-- |-- |-- (Real Voltage_step.offset, 0.0, None)
##  |-- |-- |-- |-- (Real Voltage_step.startTime, 0.0, s)
##  |-- |-- |-- |-- (Real Voltage_step.height, 12.0, V)
##  |-- |-- |-- |-- (Real MLoad.k, -0.5, N.m)
##  |-- Connections:
##  |-- |-- SuT.M_mot -> .M_mot
##  |-- |-- SuT.wB -> .wB
##  |-- |-- stimuli_model.U -> .U
##  |-- |-- SuT.I -> .I
##  |-- |-- stimuli_model.M_load -> SuT.M_load
##  |-- |-- stimuli_model.U -> SuT.U
##  |-- |-- .M_load -> stimuli_model.M_load
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
## INFO:OMSimulator.ssp:Temporary directory removed: <hidden>
## endResult
