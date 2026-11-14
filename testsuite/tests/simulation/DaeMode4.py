## status: correct
## linux: yes
## ucrt64: no
## win: no
## mac: no
## asan: no

from OMSimulator import SSP, CRef, Settings

Settings.suppressPath = True

# Acausal components (see DaeMode3.py for the models), three cases:
#  1. two RC branches in parallel on one source: a node of three terminals, whose
#     potentials give two equations and whose flows one, in place of the three
#     residuals that stood for the connected flows;
#  2. an RC branch alone: its terminal stays unconnected, so p.i = 0;
#  3. a flow connected to a potential, which is refused.
#
# 1: vc1(t) = 1 - exp(-t), vc2(t) = 1 - exp(-t/2) with R2 = 2, and the source
#    delivers exp(-t) + exp(-t/2)/2, i.e. V.p.i = -0.671145 at t = 1.
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

# 1. a node of three terminals
model = SSP()
model.addResource('../resources/AcausalSource.fmu', new_name='resources/AcausalSource.fmu')
model.addResource('../resources/AcausalRC.fmu', new_name='resources/AcausalRC.fmu')
model.addResource('../resources/AcausalGround.fmu', new_name='resources/AcausalGround.fmu')
model.addComponent(CRef('default', 'V'), 'resources/AcausalSource.fmu')
model.addComponent(CRef('default', 'RC1'), 'resources/AcausalRC.fmu')
model.addComponent(CRef('default', 'RC2'), 'resources/AcausalRC.fmu')
model.addComponent(CRef('default', 'G'), 'resources/AcausalGround.fmu')
model.addConnection(CRef('default', 'V', 'p'), CRef('default', 'RC1', 'p'))
model.addConnection(CRef('default', 'V', 'p'), CRef('default', 'RC2', 'p'))
model.addConnection(CRef('default', 'V', 'n'), CRef('default', 'G', 'p'))
show(run(model, 'DaeMode4a', [(CRef('default', 'RC2', 'R'), 2.0)]),
     [('RC1', 'vc'), ('RC2', 'vc'), ('RC1', 'p.i'), ('RC2', 'p.i'), ('V', 'p.i'), ('G', 'p.i')])

# 2. unconnected
model = SSP()
model.addResource('../resources/AcausalRC.fmu', new_name='resources/AcausalRC.fmu')
model.addComponent(CRef('default', 'RC'), 'resources/AcausalRC.fmu')
show(run(model, 'DaeMode4b', []), [('RC', 'vc'), ('RC', 'p.i')])

# 3. a flow connected to a potential
model = SSP()
model.addResource('../resources/AcausalSource.fmu', new_name='resources/AcausalSource.fmu')
model.addResource('../resources/AcausalRC.fmu', new_name='resources/AcausalRC.fmu')
model.addComponent(CRef('default', 'V'), 'resources/AcausalSource.fmu')
model.addComponent(CRef('default', 'RC'), 'resources/AcausalRC.fmu')
model.addConnection(CRef('default', 'V', 'p.i'), CRef('default', 'RC', 'p.v'))
try:
  run(model, 'DaeMode4c', [])
except Exception as e:
  print(f"info: refused: {e}", flush=True)
## Result:
## info:    FMU "V" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 3 residuals over 0 states and 3 algebraic variables
## info:    FMU "V" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 2 terminal(s), 2 connector residual(s)
## info:    FMU "RC1" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 3 residuals over 1 states and 2 algebraic variables
## info:    FMU "RC1" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 1 terminal(s), 1 connector residual(s)
## info:    FMU "RC2" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 3 residuals over 1 states and 2 algebraic variables
## info:    FMU "RC2" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 1 terminal(s), 1 connector residual(s)
## info:    FMU "G" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 1 residuals over 0 states and 1 algebraic variables
## info:    FMU "G" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 1 terminal(s), 1 connector residual(s)
## info:    system "model.root": 6 acausal connection(s) in 4 connection set(s) replace 5 connector residual(s)
## info:    system "model.root" contains a component in DAE mode (fmi-ls-dae); integrating with IDA instead of cvode
## info:    system "model.root": DAE Jacobian 10x10, 32 structural nonzeros, 7 colour(s) — sparse (KLU), one residual evaluation per colour instead of one per unknown
## info:    Result file: DaeMode4a_res.mat (bufferSize=10)
## info: RC1.vc = 0.632121
## info: RC2.vc = 0.393469
## info: RC1.p.i = 0.367879
## info: RC2.p.i = 0.303265
## info: V.p.i = -0.671145
## info: G.p.i = -0.671145
## info:    Final Statistics for 'model.root':
##          NumSteps = 1010 NumResEvals  = 1014 NumLinSolvSetups = 12
##          NumNonlinSolvIters = 1012 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## info:    FMU "RC" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 3 residuals over 1 states and 2 algebraic variables
## info:    FMU "RC" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 1 terminal(s), 1 connector residual(s)
## info:    system "model.root": no acausal connections; the terminals of its acausal components stay unconnected (flow = 0)
## info:    system "model.root" contains a component in DAE mode (fmi-ls-dae); integrating with IDA instead of cvode
## info:    system "model.root": DAE Jacobian 3x3, 7 structural nonzeros, 3 colour(s) — sparse (KLU), one residual evaluation per colour instead of one per unknown
## info:    Result file: DaeMode4b_res.mat (bufferSize=10)
## info: RC.vc = 0.000000
## info: RC.p.i = 0.000000
## info:    Final Statistics for 'model.root':
##          NumSteps = 1010 NumResEvals  = 1012 NumLinSolvSetups = 12
##          NumNonlinSolvIters = 1010 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
## info:    FMU "V" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 3 residuals over 0 states and 3 algebraic variables
## info:    FMU "V" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 2 terminal(s), 2 connector residual(s)
## info:    FMU "RC" carries a DAE formulation (fmi-ls-dae 1.0.0-alpha.1): 3 residuals over 1 states and 2 algebraic variables
## info:    FMU "RC" is an acausal component (org.openmodelica.fmi-ls-dae-acausal 0.1.0): 1 terminal(s), 1 connector residual(s)
## error:   [collectAcausalConnections] system "model.root": acausal connection between a flow and a potential: V.p.i -> RC.p.v
## error:   [initialize] Initialization of system "model.root" failed
## info: refused: Failed to initialize model: Status.error
## info:    0 warnings
## info:    2 errors
## endResult
