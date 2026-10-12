## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## asan: yes

from OMSimulator import SSP, CRef, Settings, Capi
Settings.suppressPath = True
Capi.setCommandLineOption("--stripRoot=true")

# Stair3.fmu has an event every second. With event mode, OMSimulator writes the values
# before and after each event at the same time stamp to the result file.

model = SSP()
model.addResource('../../../resources/Stair3.fmu', new_name='resources/Stair3.fmu')

model.addComponent(CRef('default', 'Stair'), 'resources/Stair3.fmu')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("Stair-cs-event.csv")

instantiated_model.setFixedStepSize(0.2)
instantiated_model.setStopTime(9.0)
instantiated_model.setTolerance(1e-5)

instantiated_model.initialize()
instantiated_model.simulate()

instantiated_model.terminate()
instantiated_model.delete()

# print the result file
with open("Stair-cs-event.csv") as f:
  for line in f:
    print(line.rstrip(), flush=True)

## Result:
## info:    Result file: Stair-cs-event.csv (bufferSize=10)
## info:    FMU "model.root.Stair" requested to terminate the simulation
## "time","default.Stair.time","default.Stair.counter"
## 0, 0, 1
## 0.2, 0.2, 1
## 0.4, 0.4, 1
## 0.6, 0.6, 1
## 0.8, 0.8, 1
## 1, 1, 1
## 1, 1, 2
## 1.2, 1.2, 2
## 1.4, 1.4, 2
## 1.6, 1.6, 2
## 1.8, 1.8, 2
## 2, 2, 2
## 2, 2, 3
## 2.2, 2.2, 3
## 2.4, 2.4, 3
## 2.6, 2.6, 3
## 2.8, 2.8, 3
## 3, 3, 3
## 3, 3, 4
## 3.2, 3.2, 4
## 3.4, 3.4, 4
## 3.6, 3.6, 4
## 3.8, 3.8, 4
## 4, 4, 4
## 4, 4, 5
## 4.2, 4.2, 5
## 4.4, 4.4, 5
## 4.6, 4.6, 5
## 4.8, 4.8, 5
## 5, 5, 5
## 5, 5, 6
## 5.2, 5.2, 6
## 5.4, 5.4, 6
## 5.6, 5.6, 6
## 5.8, 5.8, 6
## 6, 6, 6
## 6, 6, 7
## 6.2, 6.2, 7
## 6.4, 6.4, 7
## 6.6, 6.6, 7
## 6.8, 6.8, 7
## 7, 7, 7
## 7, 7, 8
## 7.2, 7.2, 8
## 7.4, 7.4, 8
## 7.6, 7.6, 8
## 7.8, 7.8, 8
## 8, 8, 8
## 8, 8, 9
## 8.2, 8.2, 9
## 8.4, 8.4, 9
## 8.6, 8.6, 9
## 8.8, 8.8, 9
## 9, 9, 9
## 9, 9, 10
## endResult
