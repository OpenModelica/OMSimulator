## status: correct
## linux: yes
## ucrt64: no
## win: no
## mac: no
## asan: no

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# Acausal components connected through their terminals: an ideal voltage source,
# an RC branch and a ground, each a separate FMU, wired as an electrical circuit
# after they were compiled. The FMUs carry OpenModelica's fmi-ls-dae-acausal
# manifest: per terminal flow, the residual that reads flow = 0 while the
# terminal is unconnected. The system replaces those with the equations of the
# connection sets (potentials equal, flows summing to zero) and integrates the
# whole circuit with IDA.
#
# The FMUs are exported by OpenModelica with
# --simCodeTarget=C+Rust --daeMode --fmuAcausalComponent, so they only carry
# x86_64-linux binaries:
#
#   package Circuit
#     connector Pin
#       Real v;
#       flow Real i;
#     end Pin;
#     model RC "R in series with C to ground"
#       parameter Real R = 1, C = 1;
#       Pin p;
#       Real vc(start = 0, fixed = true);
#     equation
#       p.v - vc = R*p.i;
#       C*der(vc) = p.i;
#     end RC;
#     model Source "ideal voltage source"
#       parameter Real V = 1;
#       Pin p, n;
#     equation
#       p.v - n.v = V;
#       p.i + n.i = 0;
#     end Source;
#     model Ground
#       Pin p;
#     equation
#       p.v = 0;
#     end Ground;
#   end Circuit;
#
# Connected: V.p -- RC.p, V.n -- G.p. Then vc(t) = 1 - exp(-t), vc(1) = 0.632121,
# and the current into RC.p is exp(-t) = 0.367879 at t = 1.
model = SSP()
model.addResource('../resources/AcausalSource.fmu', new_name='resources/AcausalSource.fmu')
model.addResource('../resources/AcausalRC.fmu', new_name='resources/AcausalRC.fmu')
model.addResource('../resources/AcausalGround.fmu', new_name='resources/AcausalGround.fmu')

model.addComponent(CRef('default', 'V'), 'resources/AcausalSource.fmu')
model.addComponent(CRef('default', 'RC'), 'resources/AcausalRC.fmu')
model.addComponent(CRef('default', 'G'), 'resources/AcausalGround.fmu')

# terminal to terminal: expanded into one connection per member
model.addConnection(CRef('default', 'V', 'p'), CRef('default', 'RC', 'p'))
model.addConnection(CRef('default', 'V', 'n'), CRef('default', 'G', 'p'))

model.export('DaeMode3.ssp')

model2 = SSP('DaeMode3.ssp')
model2.list()

instantiated_model = model2.instantiate()
instantiated_model.setResultFile('DaeMode3_res.mat')
instantiated_model.initialize()
instantiated_model.simulate()
for name in [('RC', 'vc'), ('RC', 'p.i'), ('RC', 'p.v'), ('V', 'p.i'), ('V', 'n.i'), ('V', 'n.v'), ('G', 'p.i')]:
  print(f"info: {'.'.join(name)} = {instantiated_model.getValue(CRef('default', *name)):.6f}", flush=True)
instantiated_model.terminate()
instantiated_model.delete()
## Result:
## info:    FMU "V" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 3 residuals over 0 states and 3 algebraic variables
## info:    FMU "V" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 2 terminal(s), 2 connector residual(s)
## info:    FMU "RC" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 3 residuals over 1 states and 2 algebraic variables
## info:    FMU "RC" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 1 terminal(s), 1 connector residual(s)
## info:    FMU "G" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 1 residuals over 0 states and 1 algebraic variables
## info:    FMU "G" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 1 terminal(s), 1 connector residual(s)
## info:    system "model.root": 4 acausal connection(s) in 4 connection set(s) replace 4 connector residual(s)
## info:    system "model.root" contains a component in DAE mode (fmi-ls-dae); integrating with IDA instead of cvode
## info:    system "model.root": DAE Jacobian 7x7, 21 structural nonzeros, 5 colour(s) — sparse (KLU), one residual evaluation per colour instead of one per unknown
## info:    Result file: DaeMode3_res.mat (bufferSize=10)
## <class 'OMSimulator.ssp.SSP'>
## |-- Resources:
## |--   resources/AcausalGround.fmu
## |--   resources/AcausalRC.fmu
## |--   resources/AcausalSource.fmu
## |-- Active Variant: default
## |-- <class 'OMSimulator.ssd.SSD'>
## |-- Variant "default": <hidden>
## |-- |-- System: default 'None'
## |-- |-- |-- Connectors:
## |-- |-- |-- Elements:
## |-- |-- |-- |-- FMU: V 'None'
## |-- |-- |-- |-- |-- path: resources/AcausalSource.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (V, Causality.parameter, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (n.v, Causality.unspecified, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (p.i, Causality.unspecified, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (p.v, Causality.unspecified, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (n.i, Causality.unspecified, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- FMU: RC 'None'
## |-- |-- |-- |-- |-- path: resources/AcausalRC.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (C, Causality.parameter, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (R, Causality.parameter, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (p.i, Causality.unspecified, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (p.v, Causality.unspecified, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- FMU: G 'None'
## |-- |-- |-- |-- |-- path: resources/AcausalGround.fmu
## |-- |-- |-- |-- |-- Connectors:
## |-- |-- |-- |-- |-- |-- (p.i, Causality.unspecified, SignalType.Float64, None, 'None')
## |-- |-- |-- |-- |-- |-- (p.v, Causality.unspecified, SignalType.Float64, None, 'None')
## |-- |-- |-- Connections:
## |-- |-- |-- |-- V.p.i -> RC.p.i
## |-- |-- |-- |-- V.p.v -> RC.p.v
## |-- |-- |-- |-- V.n.v -> G.p.v
## |-- |-- |-- |-- V.n.i -> G.p.i
## |-- DefaultExperiment
## |-- |-- startTime: 0.0
## |-- |-- stopTime: 1.0
## info: RC.vc = 0.632121
## info: RC.p.i = 0.367879
## info: RC.p.v = 1.000000
## info: V.p.i = -0.367879
## info: V.n.i = 0.367879
## info: V.n.v = 0.000000
## info: G.p.i = -0.367879
## info:    Final Statistics for 'model.root':
##          NumSteps = 1010 NumResEvals  = 1014 NumLinSolvSetups = 12
##          NumNonlinSolvIters = 1012 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## endResult
