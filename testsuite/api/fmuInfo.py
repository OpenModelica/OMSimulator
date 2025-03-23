## status: correct
## teardown_command:
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

from OMSimulator import FMU

# This test case tests the new python API construct
# FMU which basically loads the FMU and displays the
# fmuinfo state variables

fmu = FMU('../resources/Modelica.Electrical.Analog.Examples.CauerLowPassAnalog.fmu')
for var in sorted(fmu.getStateVariables(), key=lambda x: x['name']):
  print(var)

## Result:
## {'name': 'C1.v', 'valueReference': '0', 'variability': 'continuous', 'causality': 'local'}
## {'name': 'C3.v', 'valueReference': '1', 'variability': 'continuous', 'causality': 'local'}
## {'name': 'C5.v', 'valueReference': '2', 'variability': 'continuous', 'causality': 'local'}
## {'name': 'L1.i', 'valueReference': '3', 'variability': 'continuous', 'causality': 'local'}
## {'name': 'L2.i', 'valueReference': '4', 'variability': 'continuous', 'causality': 'local'}
## endResult
