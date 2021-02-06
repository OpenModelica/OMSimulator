from OMSimulator import Scope, Types
from OMSimulator.NewAPI import *


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

  def terminate(self):
    status = Scope._capi.terminate(self.cref)
    if Types.Status(status) != Types.Status.OK:
      raise Exception('error {}'.format(Types.Status(status)))

  @property
  def cref(self):
    return self._cref

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
