from OMSimulator import Model, Scope, Types


def newModel(cref):
  status = Scope._capi.newModel(cref)
  if Types.Status(status) == Types.Status.OK:
    Scope._Scope.append(cref)
  else:
    raise Exception('error {}'.format(Types.Status(status)))
  return Model(cref)

def setTempDirectory(dir):
  status = Scope._capi.setTempDirectory(dir)
  if Types.Status(status) != Types.Status.OK:
    raise Exception('error {}'.format(Types.Status(status)))

def importFile(file):
  cref, status = Scope._capi.importFile(file)
  if Types.Status(status) != Types.Status.OK:
    raise Exception('error {}'.format(Types.Status(status)))
  return cref
