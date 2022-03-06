## status: correct
## teardown_command: rm -rf nls/
## linux: yes
## mingw32: no
## mingw64: yes
## win: yes
## mac: no

import OMSimulator as oms

oms.setCommandLineOption("--suppressPath=true --algLoopSolver=kinsol --dumpAlgLoops=true")
oms.setTempDirectory("./nls/")

model = oms.newModel("nls")
root = model.addSystem('root', oms.Types.System.WC)

# instantiate FMUs
root.addSubModel('fmu', '../resources/nls.cs.fmu')

# add connections
root.addConnection('fmu.u', 'fmu.y')

# simulation settings
model.resultFile = ''
model.stopTime = 4.0

model.instantiate()
model.initialize()
print(f'y: {model.getReal("root.fmu.y"):.4f} (res: {model.getReal("root.fmu.y") - model.getReal("root.fmu.u"):9.2e})')
model.simulate()
print(f'y: {model.getReal("root.fmu.y"):.4f} (res: {model.getReal("root.fmu.y") - model.getReal("root.fmu.u"):9.2e})')
model.terminate()
model.delete()

## Result:
## endResult
