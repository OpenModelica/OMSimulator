## status: correct
## linux: yes
## ucrt64: no
## win: no
## mac: no
## asan: no

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# Acausal components with stream connectors, each a separate FMU, connected
# through their terminals. Besides the residuals that stand for the flows, the
# fmi-ls-dae-acausal manifest names, per stream variable h_outflow of a terminal,
# the residual that reads inStream(h_outflow) = h_outflow while the terminal is
# unconnected; the terminal's TerminalStreamMemberVariable names the variable
# 'inStream(port.h_outflow)' that stands for inStream. The system replaces that
# residual by the mixing rule of the connection set:
#   inStream(h_i) = sum_{j<>i} max(-m_j, eps)*h_j / sum_{j<>i} max(-m_j, eps)
# which is h_j for a set of two.
#
# The FMUs are exported by OpenModelica with
# --simCodeTarget=C+Rust --daeMode --fmuAcausalComponent, so they only carry
# x86_64-linux binaries:
#
#   package Fluid
#     connector Port
#       Real p;
#       flow Real m_flow;
#       stream Real h_outflow;
#     end Port;
#     model Boundary "a fixed pressure and specific enthalpy"
#       parameter Real p = 1, h = 0;
#       Port port;
#       Real hIn = inStream(port.h_outflow);
#     equation
#       port.p = p;
#       port.h_outflow = h;
#     end Boundary;
#     model Volume "an incompressible mixing volume behind a linear flow resistance"
#       parameter Real m = 1, R = 1;
#       Port a, b;
#       Real h(start = 1, fixed = true);
#     equation
#       a.m_flow + b.m_flow = 0;
#       a.p - b.p = R*a.m_flow;
#       m*der(h) = a.m_flow*actualStream(a.h_outflow) + b.m_flow*actualStream(b.h_outflow);
#       a.h_outflow = h;
#       b.h_outflow = h;
#     end Volume;
#   end Fluid;
#
# Two cases:
#  1. S (p = 2, h = 2) -> V -> K (p = 1): 1 kg/s, der(h) = 2 - h, so
#     h(1) = 2 - exp(-1) = 1.632121, and K sees inStream = h;
#  2. S1 (p = 2, h = 2) -> V1 and S2 (p = 2, h = 4) -> V2, both into V3 -> K
#     (p = 1): a set of three terminals V1.b, V2.b, V3.a at p = 5/3, with 1/3 kg/s
#     through V1 and V2 and 2/3 kg/s through V3. V3 gets the mix
#     (h1 + h2)/2 = 3 - 2*exp(-t/3), so h3(1) = 3 - 4*exp(-1/3) + 2*exp(-2/3)
#     = 1.160709; V1.b sees the mix of the others, where only V2 flows into the
#     node: h2(1) = 4 - 3*exp(-1/3) = 1.850406.
def run(model, name, values):
  model.export(f'{name}.ssp')
  instantiated_model = SSP(f'{name}.ssp').instantiate()
  for (cref, value) in values:
    instantiated_model.setValue(cref, value)
  instantiated_model.setResultFile(f'{name}_res.mat')
  instantiated_model.initialize()
  instantiated_model.simulate()
  return instantiated_model

def show(instantiated_model, names):
  for name in names:
    print(f"info: {'.'.join(name)} = {instantiated_model.getValue(CRef('default', *name)):.6f}", flush=True)
  instantiated_model.terminate()
  instantiated_model.delete()

def fluidModel(boundaries, volumes):
  model = SSP()
  model.addResource('../resources/AcausalStreamBoundary.fmu', new_name='resources/AcausalStreamBoundary.fmu')
  model.addResource('../resources/AcausalStreamVolume.fmu', new_name='resources/AcausalStreamVolume.fmu')
  for name in boundaries:
    model.addComponent(CRef('default', name), 'resources/AcausalStreamBoundary.fmu')
  for name in volumes:
    model.addComponent(CRef('default', name), 'resources/AcausalStreamVolume.fmu')
  return model

# 1. a source, a volume and a sink
model = fluidModel(['S', 'K'], ['V'])
model.addConnection(CRef('default', 'S', 'port'), CRef('default', 'V', 'a'))
model.addConnection(CRef('default', 'V', 'b'), CRef('default', 'K', 'port'))
show(run(model, 'DaeMode5a', [(CRef('default', 'S', 'p'), 2.0), (CRef('default', 'S', 'h'), 2.0)]),
     [('V', 'h'), ('V', 'a.m_flow'), ('K', 'hIn'), ('S', 'hIn'), ('V', "'inStream(a.h_outflow)'")])

# 2. two branches mixing in a node of three terminals
model = fluidModel(['S1', 'S2', 'K'], ['V1', 'V2', 'V3'])
model.addConnection(CRef('default', 'S1', 'port'), CRef('default', 'V1', 'a'))
model.addConnection(CRef('default', 'S2', 'port'), CRef('default', 'V2', 'a'))
model.addConnection(CRef('default', 'V1', 'b'), CRef('default', 'V3', 'a'))
model.addConnection(CRef('default', 'V2', 'b'), CRef('default', 'V3', 'a'))
model.addConnection(CRef('default', 'V3', 'b'), CRef('default', 'K', 'port'))
show(run(model, 'DaeMode5b', [(CRef('default', 'S1', 'p'), 2.0), (CRef('default', 'S1', 'h'), 2.0),
                              (CRef('default', 'S2', 'p'), 2.0), (CRef('default', 'S2', 'h'), 4.0)]),
     [('V1', 'h'), ('V2', 'h'), ('V3', 'h'), ('V3', 'a.m_flow'), ('V3', 'a.p'),
      ('V3', "'inStream(a.h_outflow)'"), ('V1', "'inStream(b.h_outflow)'"), ('K', 'hIn')])
## Result:
## info:    FMU "S" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 2 residuals over 0 states and 2 algebraic variables
## info:    FMU "S" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 1 terminal(s), 1 connector residual(s), 1 stream residual(s)
## info:    FMU "K" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 2 residuals over 0 states and 2 algebraic variables
## info:    FMU "K" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 1 terminal(s), 1 connector residual(s), 1 stream residual(s)
## info:    FMU "V" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 6 residuals over 1 states and 5 algebraic variables
## info:    FMU "V" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 2 terminal(s), 2 connector residual(s), 2 stream residual(s)
## info:    system "model.root": 6 acausal connection(s) in 6 connection set(s) replace 4 connector residual(s) and 4 stream residual(s)
## info:    system "model.root" contains a component in DAE mode (fmi-ls-dae); integrating with IDA instead of cvode
## info:    system "model.root": DAE Jacobian 10x10, 55 structural nonzeros, 6 colour(s) — sparse (KLU), one residual evaluation per colour instead of one per unknown
## info:    Result file: DaeMode5a_res.mat (bufferSize=10)
## info: V.h = 1.632121
## info: V.a.m_flow = 1.000000
## info: K.hIn = 1.632121
## info: S.hIn = 1.632121
## info: V.'inStream(a.h_outflow)' = 2.000000
## info:    Final Statistics for 'model.root':
##          NumSteps = 1010 NumResEvals  = 1015 NumLinSolvSetups = 12
##          NumNonlinSolvIters = 1013 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## info:    FMU "S1" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 2 residuals over 0 states and 2 algebraic variables
## info:    FMU "S1" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 1 terminal(s), 1 connector residual(s), 1 stream residual(s)
## info:    FMU "S2" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 2 residuals over 0 states and 2 algebraic variables
## info:    FMU "S2" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 1 terminal(s), 1 connector residual(s), 1 stream residual(s)
## info:    FMU "K" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 2 residuals over 0 states and 2 algebraic variables
## info:    FMU "K" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 1 terminal(s), 1 connector residual(s), 1 stream residual(s)
## info:    FMU "V1" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 6 residuals over 1 states and 5 algebraic variables
## info:    FMU "V1" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 2 terminal(s), 2 connector residual(s), 2 stream residual(s)
## info:    FMU "V2" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 6 residuals over 1 states and 5 algebraic variables
## info:    FMU "V2" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 2 terminal(s), 2 connector residual(s), 2 stream residual(s)
## info:    FMU "V3" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 6 residuals over 1 states and 5 algebraic variables
## info:    FMU "V3" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 2 terminal(s), 2 connector residual(s), 2 stream residual(s)
## info:    system "model.root": 15 acausal connection(s) in 12 connection set(s) replace 9 connector residual(s) and 9 stream residual(s)
## info:    system "model.root" contains a component in DAE mode (fmi-ls-dae); integrating with IDA instead of cvode
## info:    system "model.root": DAE Jacobian 24x24, 157 structural nonzeros, 12 colour(s) — sparse (KLU), one residual evaluation per colour instead of one per unknown
## info:    Result file: DaeMode5b_res.mat (bufferSize=10)
## info: V1.h = 1.283469
## info: V2.h = 1.850406
## info: V3.h = 1.160709
## info: V3.a.m_flow = 0.666667
## info: V3.a.p = 1.666667
## info: V3.'inStream(a.h_outflow)' = 1.566937
## info: V1.'inStream(b.h_outflow)' = 1.850406
## info: K.hIn = 1.160709
## info:    Final Statistics for 'model.root':
##          NumSteps = 1010 NumResEvals  = 1013 NumLinSolvSetups = 12
##          NumNonlinSolvIters = 1011 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## endResult
