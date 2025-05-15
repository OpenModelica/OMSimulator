from OMSimulator.capi import Capi, Status

class InstantiatedModel:
  def __init__(self, json_description):
    print(json_description)
    self.model, status = Capi.instantiateFromJson(json_description)
    if status != Status.ok:
      raise RuntimeError(f"Failed to instantiate model: {status}")

  def setValues(self):
    pass

  def getValues(self):
    pass

  def initialize(self):
    status = Capi.initialize(self.model)
    if status != Status.ok:
      raise RuntimeError(f"Failed to initialize model: {status}")

  def simulate(self):
    status = Capi.simulate(self.model)
    if status != Status.ok:
      raise RuntimeError(f"Failed to simulate model: {status}")

  def stepUntil(self, stopTime):
    status = Capi.stepUntil(self.model, stopTime)
    if status != Status.ok:
      raise RuntimeError(f"Failed to step until {stopTime}: {status}")

  def terminate(self):
    status = Capi.terminate(self.model)
    if status != Status.ok:
      raise RuntimeError(f"Failed to terminate model: {status}")
