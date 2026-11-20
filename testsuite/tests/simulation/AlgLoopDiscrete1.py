## status: correct
## linux: yes
## ucrt64: yes
## win: yes
## mac: yes
## asan: yes

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# Boolean and integer connections closing algebraic loops (issue #1704): the
# Feedthrough FMU passes each input straight to its output, so connecting two of
# them back to back gives one loop over booleans and one over integers. The loop
# solvers only iterate on reals, so these loops must be broken up at their
# non-real connections instead of failing with "[getReal] Unknown signal".
model = SSP()
model.addResource('../resources/Feedthrough.fmu', new_name='resources/Feedthrough.fmu')
model.addComponent(CRef('default', 'A'), 'resources/Feedthrough.fmu')
model.addComponent(CRef('default', 'B'), 'resources/Feedthrough.fmu')

model.addConnection(CRef('default', 'A', 'Boolean_output'), CRef('default', 'B', 'Boolean_input'))
model.addConnection(CRef('default', 'B', 'Boolean_output'), CRef('default', 'A', 'Boolean_input'))
model.addConnection(CRef('default', 'A', 'Int32_output'), CRef('default', 'B', 'Int32_input'))
model.addConnection(CRef('default', 'B', 'Int32_output'), CRef('default', 'A', 'Int32_input'))
model.addConnection(CRef('default', 'A', 'Float64_continuous_output'), CRef('default', 'B', 'Float64_continuous_input'))

model.export('AlgLoopDiscrete1.ssp')

model2 = SSP('AlgLoopDiscrete1.ssp')
instantiated_model = model2.instantiate()
instantiated_model.setResultFile('')
instantiated_model.setValue(CRef('default', 'A', 'Float64_continuous_input'), 3.5)
instantiated_model.initialize()
instantiated_model.simulate()
for name in [('A', 'Boolean_output'), ('B', 'Boolean_output'), ('A', 'Int32_output'), ('B', 'Int32_output'), ('B', 'Float64_continuous_output')]:
  print(f"info:    {'.'.join(name)} = {instantiated_model.getValue(CRef('default', *name))}", flush=True)
instantiated_model.terminate()
instantiated_model.delete()

## Result:
## info:    Alg. loop broken up at its non-real connection(s):
##            A.Boolean_output -> B.Boolean_input
##            B.Boolean_output -> A.Boolean_input
## info:    Alg. loop broken up at its non-real connection(s):
##            A.Int32_output -> B.Int32_input
##            B.Int32_output -> A.Int32_input
## info:    No result file will be created
## info:    Alg. loop broken up at its non-real connection(s):
##            A.Boolean_output -> B.Boolean_input
##            B.Boolean_output -> A.Boolean_input
## info:    Alg. loop broken up at its non-real connection(s):
##            A.Int32_output -> B.Int32_input
##            B.Int32_output -> A.Int32_input
## info:    A.Boolean_output = False
## info:    B.Boolean_output = False
## info:    A.Int32_output = 0
## info:    B.Int32_output = 0
## info:    B.Float64_continuous_output = 3.5
## endResult
