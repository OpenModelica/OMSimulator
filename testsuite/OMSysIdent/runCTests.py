## status: correct
## teardown_command: rm -rf test_HelloWorld_cs_Fit/ test_HelloWorld_cs_Fit.log test_HelloWorld_cs_Fit_res.mat test_Lin2DimODE_cs_Fit/ test_Lin2DimODE_cs_Fit.log test_Lin2DimODE_cs_Fit_res.mat

# Python Wrapper for executing ctest, which in turn executes test examples.
# As of 2019-06-21 there are two examples which are executed:
# - ../../src/OMSysIdentLib/test_HelloWorld_cs_Fit.c
# - ../../src/OMSysIdentLib/test_Lin2DimODE_cs_Fit.cpp

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
