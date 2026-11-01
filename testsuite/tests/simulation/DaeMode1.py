## status: correct
## linux: yes
## ucrt64: no
## win: no
## mac: no
## asan: no

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# A component in DAE mode (fmi-ls-dae) inside an algebraic loop with an ordinary
# component: DaeLoopA declares its residuals in the fmi-ls-dae manifest, LoopB is
# an explicit FMU, and the two connections close the loop A.y -> B.p, B.q -> A.u.
# The system switches to IDA by itself, lifts the loop into rows of the DAE system
# and factorizes the coloured Jacobian with KLU.
#
# Both FMUs are exported by OpenModelica with --simCodeTarget=C+Rust, so they only
# carry x86_64-linux binaries:
#
#   model DaeLoopA
#     input Real u;
#     output Real y;
#     Real x(start = 1, fixed = true);
#     Real z;
#   equation
#     der(x) = -x + u;
#     z + 2*y = x;
#     z - y = u;
#     annotation(__OpenModelica_commandLineOptions = "--daeMode");
#   end DaeLoopA;
#
#   model LoopB
#     input Real p;
#     output Real q;
#   equation
#     q = 0.3*p + 0.5;
#   end LoopB;
#
# Closed loop: u = (0.1*x + 0.5)/1.1, so x(t) = 0.5 + 0.5*exp(-t/1.1) and
# x(1) = 0.701445.
model = SSP()
model.addResource('../resources/DaeLoopA.fmu', new_name='resources/DaeLoopA.fmu')
model.addResource('../resources/LoopB.fmu', new_name='resources/LoopB.fmu')

model.addComponent(CRef('default', 'A'), 'resources/DaeLoopA.fmu')
model.addComponent(CRef('default', 'B'), 'resources/LoopB.fmu')

model.addConnection(CRef('default', 'A', 'y'), CRef('default', 'B', 'p'))
model.addConnection(CRef('default', 'B', 'q'), CRef('default', 'A', 'u'))

model.export('DaeMode1.ssp')

model2 = SSP('DaeMode1.ssp')
instantiated_model = model2.instantiate()
instantiated_model.setResultFile('DaeMode1_res.mat')
instantiated_model.initialize()
instantiated_model.simulate()
for name in [('A', 'x'), ('A', 'y'), ('A', 'u'), ('B', 'q')]:
  print(f"info: {'.'.join(name)} = {instantiated_model.getValue(CRef('default', *name)):.6f}", flush=True)
instantiated_model.terminate()
instantiated_model.delete()
## Result:
## info:    FMU "A" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 3 residuals over 1 states and 2 algebraic variables
## info:    Alg. loop (size 2/4)
##            model.root.B
##            model.root.A
## info:    Alg. loop (size 2/4)
##            model.root.B
##            model.root.A
## info:    system "model.root": 2 connection(s) of 1 algebraic loop(s) are rows of the DAE system, not an inner solve
## info:    system "model.root" contains a component in DAE mode (fmi-ls-dae); integrating with IDA instead of cvode
## info:    system "model.root": DAE Jacobian 5x5, 16 structural nonzeros, 5 colour(s) — sparse (KLU), one residual evaluation per colour instead of one per unknown
## info:    Result file: DaeMode1_res.mat (bufferSize=10)
## info: A.x = 0.701445
## info: A.y = 0.061044
## info: A.u = 0.518313
## info: B.q = 0.518313
## info:    Final Statistics for 'model.root':
##          NumSteps = 1010 NumResEvals  = 1014 NumLinSolvSetups = 13
##          NumNonlinSolvIters = 1012 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## endResult
