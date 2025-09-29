## status: correct
## teardown_command:
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True


# This example tests whether result files .csv and .mat can be added as submodels using ComponentTable class

model = SSP()

## add csv files
model.addResource('../resources/setpoint.csv', new_name = 'resources/setpoint.csv')
model.addResource('../resources/drivetrain.csv', new_name = 'resources/drivetrain.csv')

model.addComponent(CRef('default', 'setpoint'), 'resources/setpoint.csv')
model.addComponent(CRef('default', 'drivetrain'), 'resources/drivetrain.csv')

## add mat files
model.addResource('../resources/Gain_res.mat', new_name = 'resources/Gain_res.mat')
model.addComponent(CRef('default', 'Gain'), 'resources/Gain_res.mat')


model.list()

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/Gain_res.mat
## |--   resources/drivetrain.csv
## |--   resources/setpoint.csv
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": None
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- Table: setpoint
## |-- |-- |-- |-- |-- path: resources/setpoint.csv
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (speed, Causality.output, SignalType.Real, None, 'None')
## |-- |-- |-- |-- Table: drivetrain
## |-- |-- |-- |-- |-- path: resources/drivetrain.csv
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (w, Causality.output, SignalType.Real, None, 'None')
## |-- |-- |-- |-- Table: Gain
## |-- |-- |-- |-- |-- path: resources/Gain_res.mat
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.output, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'None')
## |-- |-- |-- |-- |-- |-- (k, Causality.output, SignalType.Real, None, 'None')
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
