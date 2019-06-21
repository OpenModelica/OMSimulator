## status: correct
## teardown_command: rm test_HelloWorld_cs_Fit.log test_HelloWorld_cs_Fit_res.mat test_Lin2DimODE_cs_Fit.log test_Lin2DimODE_cs_Fit_res.mat

import os


ctestpath = '../../build/linux'
logfilename = 'ctest.log'
cwdpath = os.getcwd()

if os.path.exists(logfilename):
  os.remove(logfilename)

os.chdir(ctestpath)
# Redirect stdout into file
myCmd = 'ctest --output-on-failure > ' + cwdpath + '/' + logfilename
os.system(myCmd)
os.chdir(cwdpath)

lineList = [line.rstrip('\n') for line in open(logfilename)]
if lineList[6] == '100% tests passed, 0 tests failed out of 2':
    print(lineList[6])
else:
    for line in lineList:
        print(line)


## Result:
## 100% tests passed, 0 tests failed out of 2
## endResult
