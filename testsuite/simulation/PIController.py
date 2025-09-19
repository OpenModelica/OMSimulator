## status: correct
## teardown_command: rm -rf PIController.ssp
## linux: no
## ucrt64: yes
## win: yes
## mac: no

from OMSimulator import SSP, Settings, CRef

Settings.suppressPath = True

model = SSP()
model.addResource('../resources/Modelica.Electrical.Analog.Examples.CauerLowPassAnalog.fmu', new_name='resources/CauerLowPassAnalog.fmu')

model.addResource("../resources/Modelica.Blocks.Math.Add.fmu", new_name = 'resources/addP')
model.addResource("../resources/Modelica.Blocks.Math.Gain.fmu", new_name = 'resources/P' )
model.addResource("../resources/Modelica.Blocks.Math.Add3.fmu", new_name = 'resources/addI')
model.addResource("../resources/Modelica.Blocks.Continuous.Integrator.fmu", new_name = 'resources/I')
model.addResource("../resources/Modelica.Blocks.Nonlinear.Limiter.fmu", new_name = 'resources/limiter')

model.addComponent(CRef('default', 'addP'), 'resources/addP')
model.addComponent(CRef('default', 'P'), 'resources/P')
model.addComponent(CRef('default', 'addI'), 'resources/addI')
model.addComponent(CRef('default', 'I'), 'resources/I')
model.addComponent(CRef('default', 'limiter'), 'resources/limiter')
model.addComponent(CRef('default', 'addPI'), 'resources/addP')
model.addComponent(CRef('default', 'gainPI'), 'resources/P')
model.addComponent(CRef('default', 'addSat'), 'resources/addP')
model.addComponent(CRef('default', 'gainTrack'), 'resources/P')

## add csv files
model.addResource('../resources/setpoint.csv', new_name = 'resources/setpoint.csv')
model.addResource('../resources/drivetrain.csv', new_name = 'resources/drivetrain.csv')

model.addComponent(CRef('default', 'setpoint'), 'resources/setpoint.csv')
model.addComponent(CRef('default', 'drivetrain'), 'resources/drivetrain.csv')

model.list()
model.export('PIController.ssp')


## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/CauerLowPassAnalog.fmu
## |--   resources/I
## |--   resources/P
## |--   resources/addI
## |--   resources/addP
## |--   resources/drivetrain.csv
## |--   resources/limiter
## |--   resources/setpoint.csv
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": None
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: addP 'None'
## |-- |-- |-- |-- |-- path: resources/addP
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- FMU: P 'None'
## |-- |-- |-- |-- |-- path: resources/P
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- |-- FMU: addI 'None'
## |-- |-- |-- |-- |-- path: resources/addI
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (u3, Causality.input, SignalType.Real, None, 'Connector of Real input signal 3')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- |-- (k3, Causality.parameter, SignalType.Real, None, 'Gain of input signal 3')
## |-- |-- |-- |-- FMU: I 'None'
## |-- |-- |-- |-- |-- path: resources/I
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Connector of Real input signal')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Integrator gain')
## |-- |-- |-- |-- |-- |-- (y_start, Causality.parameter, SignalType.Real, None, 'Initial or guess value of output (= state)')
## |-- |-- |-- |-- |-- |-- (initType, Causality.calculatedParameter, SignalType.Enumeration, None, 'Type of initialization (1: no init, 2: steady state, 3,4: initial output)')
## |-- |-- |-- |-- |-- |-- (use_reset, Causality.calculatedParameter, SignalType.Boolean, None, '= true, if reset port enabled')
## |-- |-- |-- |-- |-- |-- (use_set, Causality.calculatedParameter, SignalType.Boolean, None, '= true, if set port enabled and used as reinitialization value when reset')
## |-- |-- |-- |-- FMU: limiter 'None'
## |-- |-- |-- |-- |-- path: resources/limiter
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Connector of Real input signal')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (uMax, Causality.parameter, SignalType.Real, None, 'Upper limits of input signals')
## |-- |-- |-- |-- |-- |-- (uMin, Causality.calculatedParameter, SignalType.Real, None, 'Lower limits of input signals')
## |-- |-- |-- |-- |-- |-- (homotopyType, Causality.calculatedParameter, SignalType.Enumeration, None, 'Simplified model for homotopy-based initialization')
## |-- |-- |-- |-- |-- |-- (strict, Causality.calculatedParameter, SignalType.Boolean, None, '= true, if strict limits with noEvent(..)')
## |-- |-- |-- |-- FMU: addPI 'None'
## |-- |-- |-- |-- |-- path: resources/addP
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- FMU: gainPI 'None'
## |-- |-- |-- |-- |-- path: resources/P
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- |-- FMU: addSat 'None'
## |-- |-- |-- |-- |-- path: resources/addP
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- FMU: gainTrack 'None'
## |-- |-- |-- |-- |-- path: resources/P
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- |-- Table: setpoint
## |-- |-- |-- |-- |-- path: C:/OMDev/tools/msys/tmp/tmpa7lf698b/resources/setpoint.csv
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (speed, Causality.output, SignalType.Real, None, 'None')
## |-- |-- |-- |-- Table: drivetrain
## |-- |-- |-- |-- |-- path: C:/OMDev/tools/msys/tmp/tmpa7lf698b/resources/drivetrain.csv
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (w, Causality.output, SignalType.Real, None, 'None')
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
