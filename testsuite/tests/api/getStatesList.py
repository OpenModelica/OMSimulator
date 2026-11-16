## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: yes

from OMSimulator import FMU

# FMI 2.0 refers to a state by its ScalarVariable index, FMI 3.0 by its valueReference
for path in ['../resources/VanDerPol.fmu', '../resources/VanDerPol3.fmu', '../resources/BouncingBall3.fmu']:
  fmu = FMU(path)
  print(fmu.modelName, fmu.fmiVersion, flush=True)
  for name, var in fmu.get_states_list().items():
    print(' ', name, var.valueReference, var.modelDescriptionStartValue, flush=True)

## Result:
## VanderPoloscillator 2.0
##   x0 1 2
##   x1 3 0
## vanderPoloscillator 3.0
##   x0 1 2
##   x1 3 0
## BouncingBall 3.0
##   h 1 1
##   v 3 0
## endResult
