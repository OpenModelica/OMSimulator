from OMSimulator import Scope, System, Types


class Model:
  def __init__(self, cref: str):
    if cref not in Scope._Scope:
      raise Exception("Model doesn't exists")
    self._cref = cref

  def delete(self):
    status = Scope._capi.delete(self.cref)
    if Types.Status(status) == Types.Status.OK:
      Scope._Scope = [cref for cref in Scope._Scope if cref != self.cref]
    else:
      raise Exception('error {}'.format(Types.Status(status)))

  def addSystem(self, cref: str, type_: Types.System):
    new_cref = self.cref + '.' + cref
    status = Scope._capi.addSystem(new_cref, type_.value)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))
    return System.System(new_cref)

  def instantiate(self):
    status = Scope._capi.instantiate(self.cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))

  def initialize(self):
    status = Scope._capi.initialize(self.cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))

  def simulate(self):
    status = Scope._capi.simulate(self.cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))

  def doStep(self):
    status = Scope._capi.doStep(self.cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))

  def terminate(self):
    status = Scope._capi.terminate(self.cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))

  def reset(self):
    status = Scope._capi.reset(self.cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))

  def getBoolean(self, cref: str):
    value, status = Scope._capi.getBoolean(self.cref + '.' + cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))
    return value

  def getInteger(self, cref: str):
    value, status = Scope._capi.getInteger(self.cref + '.' + cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))
    return value

  def getReal(self, cref: str):
    value, status = Scope._capi.getReal(self.cref + '.' + cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))
    return value

  def setBoolean(self, cref: str, value: bool) -> None:
    status = Scope._capi.setBoolean(self.cref + '.' + cref, value)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))

  def setInteger(self, cref: str, value: int) -> None:
    status = Scope._capi.setInteger(self.cref + '.' + cref, value)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))

  def setReal(self, cref: str, value: float) -> None:
    status = Scope._capi.setReal(self.cref + '.' + cref, value)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))

  def exportSnapshot(self, ident: str = None) -> str:
    contents, status = Scope._capi.exportSnapshot(self.cref + ('.' + ident if ident else ''))
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))
    return contents

  def setLoggingInterval(self, loggingInterval: float) -> None:
    status = Scope._capi.setLoggingInterval(self.cref, loggingInterval)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))

  @property
  def cref(self):
    return self._cref

  @property
  def time(self):
    value, status = Scope._capi.getTime(self.cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))
    return value

  @property
  def startTime(self):
    value, status = Scope._capi.getStartTime(self.cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))
    return value

  @startTime.setter
  def startTime(self, value: float):
    status = Scope._capi.setStartTime(self.cref, value)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))

  @property
  def stopTime(self):
    value, status = Scope._capi.getStopTime(self.cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))
    return value

  @stopTime.setter
  def stopTime(self, value: float):
    status = Scope._capi.setStopTime(self.cref, value)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))

  @property
  def resultFile(self):
    file, _, status = Scope._capi.getResultFile(self.cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))
    return file

  @resultFile.setter
  def resultFile(self, file: str):
    _, bufferSize, status = Scope._capi.getResultFile(self.cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))

    status = Scope._capi.setResultFile(self.cref, file, bufferSize)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))

  @property
  def fixedStepSize(self):
    stepSize, status = Scope._capi.getFixedStepSize(self.cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))
    return stepSize

  @fixedStepSize.setter
  def fixedStepSize(self, stepSize: float):
    status = Scope._capi.setFixedStepSize(self.cref, stepSize)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))

  @property
  def modelState(self):
    modelState, status = Scope._capi.getModelState(self.cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))
    return Types.ModelState(modelState)
