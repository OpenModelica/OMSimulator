from OMSimulator import Scope, Types


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
