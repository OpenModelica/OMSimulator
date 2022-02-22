## status: correct
## teardown_command: rm -rf equationPair-py/
## linux: yes
## mingw32: no
## mingw64: no
## win: no
## mac: no

import OMSimulator as oms

oms.setCommandLineOption("--suppressPath=true --algLoopSolver=kinsol")
oms.setTempDirectory("./equationPair-py/")

model = oms.newModel("equationPair")
root = model.addSystem('root', oms.Types.System.WC)

# instantiate FMUs
root.addSubModel('equation1', '../resources/equationPair.equation1.fmu')
root.addSubModel('equation2', '../resources/equationPair.equation2.fmu')

# add connections
root.addConnection('equation1.x1', 'equation2.x1')
root.addConnection('equation1.x2', 'equation2.x2')

# simulation settings
model.resultFile = ''
model.stopTime = 0.0

model.instantiate()
model.initialize()
print(f'x1: {model.getReal("root.equation2.x1"):.4f} (res: {model.getReal("root.equation2.x1") - model.getReal("root.equation1.x1"):9.2e})')
print(f'x1: {model.getReal("root.equation1.x2"):.4f} (res: {model.getReal("root.equation1.x2") - model.getReal("root.equation2.x2"):9.2e})')
model.terminate()
model.delete()

## Result:
## warning: Alg. loop (size 2)
## info:    No result file will be created
## x1: 4.5572 (res:  7.54e-13)
## x1: 0.0928 (res: -4.45e-05)
## info:    1 warnings
## info:    0 errors
## endResult
