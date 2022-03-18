from OMSimulator import Scope, Types


class System:
  def __init__(self, cref: str):
    self._cref = cref

  @property
  def cref(self):
    return self._cref

  @property
  def type(self):
    Scope.clearLoggingBuffer()
    type_, status = Scope._capi.getSystemType(self.cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception(f'status={Types.Status(status).name}: trace={Scope._Logging}')
    return Types.System(type_)

  def addSystem(self, cref: str, type_: Types.System):
    Scope.clearLoggingBuffer()
    new_cref = self.cref + '.' + cref
    status = Scope._capi.addSystem(new_cref, type_.value)
    if Types.Status(status) != Types.Status.OK:
      raise Exception(f'status={Types.Status(status).name}: trace={Scope._Logging}')
    return System(new_cref)

  def addSubModel(self, cref: str, path: str):
    Scope.clearLoggingBuffer()
    new_cref = self.cref + '.' + cref
    status = Scope._capi.addSubModel(new_cref, path)
    if Types.Status(status) != Types.Status.OK:
      raise Exception(f'status={Types.Status(status).name}: trace={Scope._Logging}')
    return SubModel(new_cref)

  def addConnection(self, conA: str, conB: str):
    Scope.clearLoggingBuffer()
    new_conA = self.cref + '.' + conA
    new_conB = self.cref + '.' + conB
    status = Scope._capi.addConnection(new_conA, new_conB)
    if Types.Status(status) != Types.Status.OK:
      raise Exception(f'status={Types.Status(status).name}: trace={Scope._Logging}')

  def getBoolean(self, cref: str):
    Scope.clearLoggingBuffer()
    value, status = Scope._capi.getBoolean(self.cref + '.' + cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception(f'status={Types.Status(status).name}: trace={Scope._Logging}')
    return value

  def getInteger(self, cref: str):
    Scope.clearLoggingBuffer()
    value, status = Scope._capi.getInteger(self.cref + '.' + cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception(f'status={Types.Status(status).name}: trace={Scope._Logging}')
    return value

  def getReal(self, cref: str):
    Scope.clearLoggingBuffer()
    value, status = Scope._capi.getReal(self.cref + '.' + cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception(f'status={Types.Status(status).name}: trace={Scope._Logging}')
    return value

  def setBoolean(self, cref: str, value: bool) -> None:
    Scope.clearLoggingBuffer()
    status = Scope._capi.setBoolean(self.cref + '.' + cref, value)
    if Types.Status(status) != Types.Status.OK:
      raise Exception(f'status={Types.Status(status).name}: trace={Scope._Logging}')

  def setInteger(self, cref: str, value: int) -> None:
    Scope.clearLoggingBuffer()
    status = Scope._capi.setInteger(self.cref + '.' + cref, value)
    if Types.Status(status) != Types.Status.OK:
      raise Exception(f'status={Types.Status(status).name}: trace={Scope._Logging}')

  def setReal(self, cref: str, value: float) -> None:
    Scope.clearLoggingBuffer()
    status = Scope._capi.setReal(self.cref + '.' + cref, value)
    if Types.Status(status) != Types.Status.OK:
      raise Exception(f'status={Types.Status(status).name}: trace={Scope._Logging}')


class SubModel:
  def __init__(self, cref: str):
    self._cref = cref

  @property
  def cref(self):
    return self._cref

  def getBoolean(self, cref: str):
    Scope.clearLoggingBuffer()
    value, status = Scope._capi.getBoolean(self.cref + '.' + cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception(f'status={Types.Status(status).name}: trace={Scope._Logging}')
    return value

  def getInteger(self, cref: str):
    Scope.clearLoggingBuffer()
    value, status = Scope._capi.getInteger(self.cref + '.' + cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception(f'status={Types.Status(status).name}: trace={Scope._Logging}')
    return value

  def getReal(self, cref: str):
    Scope.clearLoggingBuffer()
    value, status = Scope._capi.getReal(self.cref + '.' + cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception(f'status={Types.Status(status).name}: trace={Scope._Logging}')
    return value

  def setBoolean(self, cref: str, value: bool) -> None:
    Scope.clearLoggingBuffer()
    status = Scope._capi.setBoolean(self.cref + '.' + cref, value)
    if Types.Status(status) != Types.Status.OK:
      raise Exception(f'status={Types.Status(status).name}: trace={Scope._Logging}')

  def setInteger(self, cref: str, value: int) -> None:
    Scope.clearLoggingBuffer()
    status = Scope._capi.setInteger(self.cref + '.' + cref, value)
    if Types.Status(status) != Types.Status.OK:
      raise Exception(f'status={Types.Status(status).name}: trace={Scope._Logging}')

  def setReal(self, cref: str, value: float) -> None:
    Scope.clearLoggingBuffer()
    status = Scope._capi.setReal(self.cref + '.' + cref, value)
    if Types.Status(status) != Types.Status.OK:
      raise Exception(f'status={Types.Status(status).name}: trace={Scope._Logging}')
