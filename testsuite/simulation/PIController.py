## status: correct
## teardown_command: rm -rf PIController.ssp
## linux: no
## ucrt64: yes
## win: yes
## mac: no

from OMSimulator import SSP, Settings, CRef

Settings.suppressPath = True

model = SSP()

model.addResource("../resources/Modelica.Blocks.Math.Add.fmu", new_name = 'resources/addP.fmu')
model.addResource("../resources/Modelica.Blocks.Math.Gain.fmu", new_name = 'resources/P.fmu' )
model.addResource("../resources/Modelica.Blocks.Math.Add3.fmu", new_name = 'resources/addI.fmu')
model.addResource("../resources/Modelica.Blocks.Continuous.Integrator.fmu", new_name = 'resources/I.fmu')
model.addResource("../resources/Modelica.Blocks.Nonlinear.Limiter.fmu", new_name = 'resources/limiter.fmu')

model.addComponent(CRef('default', 'addP'), 'resources/addP.fmu')
model.addComponent(CRef('default', 'P'), 'resources/P.fmu')
model.addComponent(CRef('default', 'addI'), 'resources/addI.fmu')
model.addComponent(CRef('default', 'I'), 'resources/I.fmu')
model.addComponent(CRef('default', 'limiter'), 'resources/limiter.fmu')
model.addComponent(CRef('default', 'addPI'), 'resources/addP.fmu')
model.addComponent(CRef('default', 'gainPI'), 'resources/P.fmu')
model.addComponent(CRef('default', 'addSat'), 'resources/addP.fmu')
model.addComponent(CRef('default', 'gainTrack'), 'resources/P.fmu')

## add csv files
model.addResource('../resources/setpoint.csv', new_name = 'resources/setpoint.csv')
model.addResource('../resources/drivetrain.csv', new_name = 'resources/drivetrain.csv')

model.addComponent(CRef('default', 'setpoint'), 'resources/setpoint.csv')
model.addComponent(CRef('default', 'drivetrain'), 'resources/drivetrain.csv')

## add connections
model.addConnection(CRef('default', 'addP', 'y'), CRef('default', 'P', 'u'))
model.addConnection(CRef('default', 'addI', 'y'), CRef('default', 'I', 'u'))
model.addConnection(CRef('default', 'P', 'y'),    CRef('default', 'addPI', 'u1'))
model.addConnection(CRef('default', 'I', 'y'), CRef('default', 'addPI', 'u2'))
model.addConnection(CRef('default', 'addPI', 'y'), CRef('default', 'gainPI', 'u'))
model.addConnection(CRef('default', 'gainPI', 'y'), CRef('default', 'limiter', 'u'))
model.addConnection(CRef('default', 'gainPI', 'y'), CRef('default', 'addSat', 'u2'))
model.addConnection(CRef('default', 'limiter', 'y'), CRef('default', 'addSat', 'u1'))
model.addConnection(CRef('default', 'addSat', 'y'), CRef('default', 'gainTrack', 'u'))
model.addConnection(CRef('default', 'gainTrack', 'y'), CRef('default', 'addI', 'u3'))

#add connections fro table
model.addConnection(CRef('default', 'setpoint', 'speed'),  CRef('default', 'addP', 'u1'))
model.addConnection(CRef('default', 'setpoint', 'speed'),  CRef('default', 'addI', 'u1'))
model.addConnection(CRef('default', 'driveTrain', 'w')  ,  CRef('default', 'addP', 'u2'))
model.addConnection(CRef('default', 'driveTrain', 'w')  ,  CRef('default', 'addI', 'u2'))

model.export('PIController.ssp')

model2 = SSP('PIController.ssp')
model2.list()
# instantiated_model = model2.instantiate() ## internally generate the json file and also set the model state like virgin,

## Result:
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/I.fmu
## |--   resources/P.fmu
## |--   resources/addI.fmu
## |--   resources/addP.fmu
## |--   resources/drivetrain.csv
## |--   resources/limiter.fmu
## |--   resources/setpoint.csv
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: addP 'None'
## |-- |-- |-- |-- |-- path: resources/addP.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- FMU: P 'None'
## |-- |-- |-- |-- |-- path: resources/P.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- |-- FMU: addI 'None'
## |-- |-- |-- |-- |-- path: resources/addI.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (u3, Causality.input, SignalType.Real, None, 'Connector of Real input signal 3')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- |-- |-- (k3, Causality.parameter, SignalType.Real, None, 'Gain of input signal 3')
## |-- |-- |-- |-- FMU: I 'None'
## |-- |-- |-- |-- |-- path: resources/I.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Connector of Real input signal')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Integrator gain')
## |-- |-- |-- |-- |-- |-- (y_start, Causality.parameter, SignalType.Real, None, 'Initial or guess value of output (= state)')
## |-- |-- |-- |-- |-- |-- (initType, Causality.calculatedParameter, SignalType.Enumeration, None, 'Type of initialization (1: no init, 2: steady state, 3,4: initial output)')
## |-- |-- |-- |-- |-- |-- (use_reset, Causality.calculatedParameter, SignalType.Boolean, None, '= true, if reset port enabled')
## |-- |-- |-- |-- |-- |-- (use_set, Causality.calculatedParameter, SignalType.Boolean, None, '= true, if set port enabled and used as reinitialization value when reset')
## |-- |-- |-- |-- FMU: limiter 'None'
## |-- |-- |-- |-- |-- path: resources/limiter.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Connector of Real input signal')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (uMax, Causality.parameter, SignalType.Real, None, 'Upper limits of input signals')
## |-- |-- |-- |-- |-- |-- (uMin, Causality.calculatedParameter, SignalType.Real, None, 'Lower limits of input signals')
## |-- |-- |-- |-- |-- |-- (homotopyType, Causality.calculatedParameter, SignalType.Enumeration, None, 'Simplified model for homotopy-based initialization')
## |-- |-- |-- |-- |-- |-- (strict, Causality.calculatedParameter, SignalType.Boolean, None, '= true, if strict limits with noEvent(..)')
## |-- |-- |-- |-- FMU: addPI 'None'
## |-- |-- |-- |-- |-- path: resources/addP.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- FMU: gainPI 'None'
## |-- |-- |-- |-- |-- path: resources/P.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- |-- FMU: addSat 'None'
## |-- |-- |-- |-- |-- path: resources/addP.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u1, Causality.input, SignalType.Real, None, 'Connector of Real input signal 1')
## |-- |-- |-- |-- |-- |-- (u2, Causality.input, SignalType.Real, None, 'Connector of Real input signal 2')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Connector of Real output signal')
## |-- |-- |-- |-- |-- |-- (k1, Causality.parameter, SignalType.Real, None, 'Gain of input signal 1')
## |-- |-- |-- |-- |-- |-- (k2, Causality.parameter, SignalType.Real, None, 'Gain of input signal 2')
## |-- |-- |-- |-- FMU: gainTrack 'None'
## |-- |-- |-- |-- |-- path: resources/P.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (u, Causality.input, SignalType.Real, None, 'Input signal connector')
## |-- |-- |-- |-- |-- |-- (y, Causality.output, SignalType.Real, None, 'Output signal connector')
## |-- |-- |-- |-- |-- |-- (k, Causality.parameter, SignalType.Real, 1, 'Gain value multiplied with input signal')
## |-- |-- |-- |-- Table: setpoint
## |-- |-- |-- |-- |-- path: resources/setpoint.csv
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (speed, Causality.output, SignalType.Real, None, 'None')
## |-- |-- |-- |-- Table: drivetrain
## |-- |-- |-- |-- |-- path: resources/drivetrain.csv
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (w, Causality.output, SignalType.Real, None, 'None')
## |-- |-- |-- Connections:
## |-- |-- |-- |-- addP.y -> P.u
## |-- |-- |-- |-- addI.y -> I.u
## |-- |-- |-- |-- P.y -> addPI.u1
## |-- |-- |-- |-- I.y -> addPI.u2
## |-- |-- |-- |-- addPI.y -> gainPI.u
## |-- |-- |-- |-- gainPI.y -> limiter.u
## |-- |-- |-- |-- gainPI.y -> addSat.u2
## |-- |-- |-- |-- limiter.y -> addSat.u1
## |-- |-- |-- |-- addSat.y -> gainTrack.u
## |-- |-- |-- |-- gainTrack.y -> addI.u3
## |-- |-- |-- |-- setpoint.speed -> addP.u1
## |-- |-- |-- |-- setpoint.speed -> addI.u1
## |-- |-- |-- |-- driveTrain.w -> addP.u2
## |-- |-- |-- |-- driveTrain.w -> addI.u2
## |-- EnumerationDefinitions:
## |-- |-- EnumName: Modelica.Blocks.Types.Init
## |-- |-- |-- EnumItems: NoInit: 1, SteadyState: 2, InitialState: 3, InitialOutput: 4
## |-- |-- EnumName: Modelica.Blocks.Types.LimiterHomotopy
## |-- |-- |-- EnumItems: NoHomotopy: 1, Linear: 2, UpperLimit: 3, LowerLimit: 4
## |-- UnitDefinitions:
## |-- |-- Unit: 1
## |-- |-- |-- BaseUnit:
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## endResult
