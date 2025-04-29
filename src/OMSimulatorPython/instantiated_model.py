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
    Capi.initialize(self.model)

  def simulate(self):
    Capi.simulate(self.model)
