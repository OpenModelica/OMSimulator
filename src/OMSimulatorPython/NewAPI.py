from OMSimulator import Scope, Types
from OMSimulator.Model import Model


def newModel(cref):
  status = Scope._capi.newModel(cref)
  if Types.Status(status) == Types.Status.OK:
    Scope._Scope.append(cref)
  else:
    raise Exception('error {}'.format(Types.Status(status)))
  return Model(cref)
