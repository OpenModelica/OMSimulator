## status: correct
## teardown_command: rm -rf str_hello_world/
## linux: yes
## mingw32: no
## mingw64: yes
## win: yes
## mac: no

import OMSimulator as oms

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./str_hello_world/")

model = oms.newModel("str_hello_world")
root = model.addSystem('root', oms.Types.System.WC)

# instantiate FMUs
root.addSubModel('fmu', '../resources/str_hello_world.fmu')

# simulation settings
model.resultFile = ''
model.stopTime = 0.0

model.instantiate()
print(f'a: {model.getString("root.fmu.a")}')
print(f'b: {model.getString("root.fmu.b")}')
print(f'c: {model.getString("root.fmu.c")}')

model.initialize()
print(f'a: {model.getString("root.fmu.a")}')
print(f'b: {model.getString("root.fmu.b")}')
print(f'c: {model.getString("root.fmu.c")}')

model.terminate()
model.delete()

## Result:
## endResult
