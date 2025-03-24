## status: correct
## teardown_command:
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

from OMSimulator import FMU, Variable, SignalType, Causality

# This test case tests the new python API construct
# FMU which basically loads the FMU and displays the
# fmuinfo state variables

fmu = FMU('../resources/Modelica.Electrical.Analog.Examples.CauerLowPassAnalog.fmu')

print('name', fmu.modelName)
print('guid', fmu.guid)
print('fmi version', fmu.fmiVersion)

print('States:')
for var in sorted(fmu.states, key=lambda x: x.name):
  print({'name': var.name, 'signal_type': var.signal_type.name, 'valueReference': var.valueReference, 'variability': var.variability, 'causality': var.causality.name})

## Result:
## name Modelica.Electrical.Analog.Examples.CauerLowPassAnalog
## guid {9a0b11b0-ed40-42af-81be-e0fdf8b841a9}
## fmi version 2.0
## States:
## {'name': CRef['C1.v'], 'signal_type': 'Real', 'valueReference': 0, 'variability': 'continuous', 'causality': 'local'}
## {'name': CRef['C3.v'], 'signal_type': 'Real', 'valueReference': 1, 'variability': 'continuous', 'causality': 'local'}
## {'name': CRef['C5.v'], 'signal_type': 'Real', 'valueReference': 2, 'variability': 'continuous', 'causality': 'local'}
## {'name': CRef['L1.i'], 'signal_type': 'Real', 'valueReference': 3, 'variability': 'continuous', 'causality': 'local'}
## {'name': CRef['L2.i'], 'signal_type': 'Real', 'valueReference': 4, 'variability': 'continuous', 'causality': 'local'}
## endResult
