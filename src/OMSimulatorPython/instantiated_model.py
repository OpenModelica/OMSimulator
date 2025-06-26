from OMSimulator.capi import Capi, Status
import json
class InstantiatedModel:
  def __init__(self, json_description):

    config = json.loads(json_description)
    self.modelName = "model" ## create random name
    self.apiCall = []

    status = Capi.newModel("model")
    if status != Status.ok:
      raise RuntimeError(f"Failed to create new model: {status}")

    status = Capi.addSystem("model.root", 1)  #oms_system_wc
    if status != Status.ok:
      raise RuntimeError(f"Failed to create oms_addSystem: {status}")

    # Begin API generation
    self.apiCall.append(f'oms_newModel("{self.modelName}")')
    self.apiCall.append(f'oms_addSystem("{self.modelName}.root", "oms_system_wc")')

    # Iterate over simulation units
    for unit in config["simulation units"]:
      ## check if unit has solver as key
      solvername = "None"
      if "solver" in unit:
        solvername = unit["solver"]['name']
        solver_path = f"{self.modelName}.root.{solvername}"
        self.apiCall.append(f'oms_addSystem("{solver_path}", "oms_system_sc")')
        status = Capi.addSystem(f"model.root.{solvername}", 2)
        if status != Status.ok:
          raise RuntimeError(f"Failed to add oms_addSystem: {status}")
      else:
        solver_path = f"{self.modelName}.root"

      for comp in unit["components"]:
        comp_path = ".".join([solver_path] + comp["name"][1:])
        self.apiCall.append(f'oms_addSubModel("{comp_path}", "{comp["path"]}")')
        status = Capi.addSubModel(comp_path, comp["path"])
        if status != Status.ok:
          raise RuntimeError(f"Failed to add oms_addSubModel: {status}")


      for conn in unit["connections"]:
        start = ".".join([solver_path] + conn["start element"][1:]) + f".{conn['start connector']}"
        end = ".".join([solver_path] + conn["end element"][1:]) + f".{conn['end connector']}"
        self.apiCall.append(f'oms_addConnection("{start}", "{end}")')
        status = Capi.addConnection(start, end)
        if status != Status.ok:
          raise RuntimeError(f"Failed to add oms_addConnection: {status}")

    self.apiCall.append(f'oms_instantiate("{self.modelName}")')
    status = Capi.instantiate(self.modelName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to instantiate model: {status}")


  def dumpApiCalls(self):
    """Returns the generated API calls as a string."""
    return "\n".join(self.apiCall)

  def setValues(self):
    pass

  def getValues(self):
    pass

  def initialize(self):
    status = Capi.initialize(self.modelName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to initialize model: {status}")

  def simulate(self):
    status = Capi.simulate(self.modelName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to simulate model: {status}")

  def stepUntil(self, stopTime):
    status = Capi.stepUntil(self.modelName, stopTime)
    if status != Status.ok:
      raise RuntimeError(f"Failed to step until {stopTime}: {status}")

  def terminate(self):
    status = Capi.terminate(self.modelName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to terminate model: {status}")
