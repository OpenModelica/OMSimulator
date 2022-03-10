## status: correct
## teardown_command: rm -rf Issue_FMU_update_vars/
## linux: no
## mingw32: no
## mingw64: yes
## win: yes
## mac: no

import OMSimulator as oms

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./nls/")

fmu_location = '../resources/Issue_FMU_update_vars.fmu'
model = oms.newModel("model")
root = model.addSystem('root', oms.Types.System.SC)
fmu = root.addSubModel('fmu', fmu_location)

print(f'Model state: {model.modelState}')
print(f'Initial value: {fmu.getReal("var")}')
fmu.setReal('var', 11.0)
print(f'After setReal: {fmu.getReal("var")}\n')

model.instantiate()
print(f'Model state: {model.modelState}')
print(f'After instantiate: {fmu.getReal("var")}')
fmu.setReal('var', 22.0)
print(f'After setReal: {fmu.getReal("var")}\n')

model.initialize()
print(f'Model state: {model.modelState}')
print(f'After initialisation: {fmu.getReal("var")}')
fmu.setReal('var', 33.0)
print(f'After setReal: {fmu.getReal("var")}')

## Result:
## info:    maximum step size for 'model.root': 0.001000
## info:    Result file: model_res.mat (bufferSize=10)
## Model state: ModelState.VIRGIN
## Initial value: 0.0
## After setReal: 11.0
##
## Model state: ModelState.INSTANTIATED
## After instantiate: 11.0
## After setReal: 22.0
##
## Model state: ModelState.SIMULATION
## After initialisation: 22.0
## After setReal: 33.0
## info:    Final Statistics for 'model.root':
##          NumSteps = 0 NumRhsEvals  = 0 NumLinSolvSetups = 0
##          NumNonlinSolvIters = 0 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## endResult
