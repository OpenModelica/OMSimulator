## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: yes

from OMSimulator import FMU

# The continuous states of FMI 3 FMUs. In FMI 3 the derivative attribute of a
# state derivative is the value reference of the state (in FMI 2 it is the
# index of the state in the list of variables), so the states are looked up
# by value reference.
for path in ['../resources/BouncingBall3.fmu', '../resources/VanDerPol3.fmu']:
  fmu = FMU(path)
  print(fmu.modelName, 'fmi version', fmu.fmiVersion)
  for var in fmu.states:
    print('  state', var.name, 'valueReference', var.valueReference)

## Result:
## BouncingBall fmi version 3.0
##   state h valueReference 1
##   state v valueReference 3
## vanderPoloscillator fmi version 3.0
##   state x0 valueReference 1
##   state x1 valueReference 3
## endResult
