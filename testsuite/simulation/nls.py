## status: correct
## teardown_command: rm -rf nls/
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: no

import OMSimulator as oms

oms.setCommandLineOption("--suppressPath=true")
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

#model.exportDependencyGraphs("nls_init.dot", "nls_event.dot", "nls_sim.dot")

model.instantiate()
print(f'u: {model.getReal("root.fmu.u"):.4f}')
print(f'y: {model.getReal("root.fmu.y"):.4f}')

model.initialize()
print(f'y: {model.getReal("root.fmu.y"):.4f} (res: {model.getReal("root.fmu.y") - model.getReal("root.fmu.u"):9.2e})')
model.simulate()
print(f'y: {model.getReal("root.fmu.y"):.4f} (res: {model.getReal("root.fmu.y") - model.getReal("root.fmu.u"):9.2e})')
model.terminate()
model.delete()

## Result:
## info:    Alg. loop (size 1/2)
##            nls.root.fmu
## info:    No result file will be created
## info:    Alg. loop (size 1/2)
##            nls.root.fmu
## u: 2.0000
## y: 9.0000
## y: 1.2470 (res:  5.37e-05)
## y: 2.8629 (res: -7.01e-09)
## endResult
