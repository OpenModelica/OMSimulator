## status: correct
## teardown_command: rm -rf equationPair/
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: no

import OMSimulator as oms

oms.setCommandLineOption("--suppressPath=true --algLoopSolver=kinsol --dumpAlgLoops=true")
oms.setTempDirectory("./equationPair/")

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
## info:    Alg. loop (size 2/4)
##            equationPair.root.equation2
##            equationPair.root.equation1
## info:    Solving system 1 to within tolerance 0.0001
## info:    iteration 1
##          inputs:
##            equation2.x2: 8.4
##            equation1.x1: 4
##          outputs:
##            equation1.x2: 2
##            equation2.x1: 54.4
##          residuals:
##            res[0]: -6.4
##            res[1]: 50.4
## info:    iteration 2
##          inputs:
##            equation2.x2: 0.0990099
##            equation1.x1: 4.59406
##          outputs:
##            equation1.x2: -0.0421527
##            equation2.x1: 4.59406
##          residuals:
##            res[0]: -0.141163
##            res[1]: -9.76996e-15
## info:    iteration 3
##          inputs:
##            equation2.x2: 0.0920217
##            equation1.x1: 4.55213
##          outputs:
##            equation1.x2: 0.111245
##            equation2.x1: 4.55213
##          residuals:
##            res[0]: 0.0192237
##            res[1]: 0
## info:    iteration 4
##          inputs:
##            equation2.x2: 0.0929733
##            equation1.x1: 4.55784
##          outputs:
##            equation1.x2: 0.0904381
##            equation2.x1: 4.55784
##          residuals:
##            res[0]: -0.00253517
##            res[1]: 8.88178e-16
## info:    iteration 5
##          inputs:
##            equation2.x2: 0.0928478
##            equation1.x1: 4.55709
##          outputs:
##            equation1.x2: 0.0931836
##            equation2.x1: 4.55709
##          residuals:
##            res[0]: 0.000335825
##            res[1]: -8.88178e-16
## info:    iteration 6
##          inputs:
##            equation2.x2: 0.0928644
##            equation1.x1: 4.55719
##          outputs:
##            equation1.x2: 0.09282
##            equation2.x1: 4.55719
##          residuals:
##            res[0]: -4.44595e-05
##            res[1]: 0
## info:    iteration 7
##          inputs:
##            equation2.x2: 0.0928644
##            equation1.x1: 4.55719
##          outputs:
##            equation1.x2: 0.09282
##            equation2.x1: 4.55719
##          residuals:
##            res[0]: -4.44595e-05
##            res[1]: 0
## info:    2-norm of residual of solution: 4.44595e-05 <= 0.0001
## info:    No result file will be created
## x1: 4.5572 (res:  0.00e+00)
## x1: 0.0928 (res: -4.45e-05)
## endResult
