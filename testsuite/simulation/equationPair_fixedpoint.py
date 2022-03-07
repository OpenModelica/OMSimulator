## status: correct
## teardown_command: rm -rf equationPair_fixedpoint/
## linux: no
## mingw32: no
## mingw64: yes
## win: yes
## mac: no

import OMSimulator as oms

oms.setCommandLineOption("--suppressPath=true --algLoopSolver=fixedpoint --dumpAlgLoops=true --maxLoopIteration=3")
oms.setTempDirectory("./equationPair_fixedpoint/")

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
try:
  model.initialize()
except:
  pass
finally:
  model.terminate()
  model.delete()

## Result:
## info:    Alg. loop (size 2/4)
##            equationPair.root.equation2
##            equationPair.root.equation1
## info:    iteration 1
##          inputs:
##            equation2.x2: 8.4
##            equation1.x1: 4
##          outputs:
##            equation1.x2: 2
##            equation2.x1: 54.4
##          residuals:
##            res[0]: 6.4
##            res[1]: -50.4
## info:    iteration 2
##          inputs:
##            equation2.x2: 2
##            equation1.x1: 54.4
##          outputs:
##            equation1.x2: -1175.34
##            equation2.x1: 16
##          residuals:
##            res[0]: 1177.34
##            res[1]: 38.4
## info:    iteration 3
##          inputs:
##            equation2.x2: -1175.34
##            equation1.x1: 16
##          outputs:
##            equation1.x2: -94
##            equation2.x1: -7048.06
##          residuals:
##            res[0]: -1081.34
##            res[1]: 7064.06
## error:   [fixPointIteration] max. number of iterations (3) exceeded at time = 0.000000
## error:   [initialize] Initialization of system "equationPair.root" failed
## info:    0 warnings
## info:    2 errors
## endResult
