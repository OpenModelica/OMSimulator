## status: correct
## teardown_command: rm -rf Stair-me-events.csv
## linux: yes
## ucrt64: yes
## win: yes
## asan: no

# Every time event has to be stored as exactly two data points, the value before
# and the value after the event. The output grid is accumulated in floating-point
# arithmetic though, so a grid point can end up a few ulp before or after the
# event time and used to be written to the result file on top of those two.
# A CSV result file shows this, because it rounds the time stamps and the
# redundant data point becomes a third row for the same time.
# At stop time we have an additional event.

import csv

from OMSimulator import SSP, CRef, Settings
Settings.suppressPath = True

model = SSP()
model.addResource('../../../resources/Stair.fmu', new_name='resources/Stair.fmu')

model.addComponent(CRef('default', 'Stair'), 'resources/Stair.fmu')
solver = {'name' : 'solver',  'method': 'cvode', 'relativeTolerance': 1e-5, 'maximumStepSize': 0.2}
model.newSolver(solver)
model.setSolver(CRef('default', 'Stair'), 'solver')

instantiated_model = model.instantiate()
instantiated_model.setResultFile("Stair-me-events.csv")

instantiated_model.setStopTime(10.0)

instantiated_model.initialize()
instantiated_model.simulate()

instantiated_model.terminate()
instantiated_model.delete()

with open("Stair-me-events.csv") as file:
  rows = list(csv.reader(file, skipinitialspace=True))

counter = rows[0].index('default.Stair.counter')

# group the data points by their time stamp
groups = []
for row in rows[1:]:
  if groups and groups[-1][0] == row[0]:
    groups[-1][1].append(row[counter])
  else:
    groups.append((row[0], [row[counter]]))

print("data points per event:", flush=True)
for time, values in groups:
  if len(values) > 1:
    print("  time=%s: counter = %s" % (time, " -> ".join(values)), flush=True)

## Result:
## info:    model doesn't contain any continuous state
## info:    maximum step size for 'model.root': 0.200000
## info:    Result file: Stair-me-events.csv (bufferSize=10)
## info:    Simulation terminated by FMU model.root.Stair at time 9.000000
## info:    Final Statistics for 'model.root':
##          NumSteps = 0 NumRhsEvals  = 0 NumLinSolvSetups = 0
##          NumNonlinSolvIters = 0 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## data points per event:
##   time=1: counter = 1 -> 2
##   time=2: counter = 2 -> 3
##   time=3: counter = 3 -> 4
##   time=4: counter = 4 -> 5
##   time=5: counter = 5 -> 6
##   time=6: counter = 6 -> 7
##   time=7: counter = 7 -> 8
##   time=8: counter = 8 -> 9
##   time=9: counter = 9 -> 10
## endResult
