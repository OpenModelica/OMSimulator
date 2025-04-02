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
##  |-- |-- System: stimuli_model
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
