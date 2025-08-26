from OMSimulator.capi import Capi, Status
from OMSimulator.cref import CRef
from OMSimulator.system import System
import json
import tempfile
class InstantiatedModel:
  def __init__(self, json_description, system: System | None = None):
    config = json.loads(json_description)
    self.modelName = "model" ## create random name, but we cannot commits test as jenkins will gerate new model name
    self.apiCall = []
    self.mappedCrefs = {}  # Store mapped CRefs associated with their export names

    status = Capi.setCommandLineOption("--suppressPath=true")
    if status != Status.ok:
      raise RuntimeError(f"Failed to set command line option: {status}")
    status = Capi.setTempDirectory(tempfile.mkdtemp())
    if status != Status.ok:
      raise RuntimeError(f"Failed to set temp directory: {status}")
    # Create a new model
    status = Capi.newModel("model")
    if status != Status.ok:
      raise RuntimeError(f"Failed to create new model: {status}")
    # Add a root system
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
        comp_path = ".".join([solver_path] + [comp["name"][-1]])
        self.apiCall.append(f'oms_addSubModel("{comp_path}", "{comp["path"]}")')
        status = Capi.addSubModel(comp_path, comp["path"])
        if status != Status.ok:
          raise RuntimeError(f"Failed to add oms_addSubModel: {status}")
        export_name = ".".join(comp["name"])
        #print(f"Setting export name for {comp_path} to {export_name}")
        if not export_name in self.mappedCrefs:
          self.mappedCrefs[export_name] = comp_path
        status = Capi.setExportName(comp_path, export_name)  # Set export name if provided
        if status != Status.ok:
          raise RuntimeError(f"Failed to set export name: {status}")


      for conn in unit["connections"]:
        start = ".".join([solver_path] + [conn["start element"][-1]]) + f".{conn['start connector']}"
        end = ".".join([solver_path] + [conn["end element"][-1]]) + f".{conn['end connector']}"
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

  def setValue(self, cref: CRef, value):
    """Sets a value for a specific CRef in the model."""
    name = ".".join(cref.names[:-1])
    if name in self.mappedCrefs:
      mapped_cref = ".".join([self.mappedCrefs[name], cref.names[-1]])
      status = Capi.setReal(mapped_cref, value) # Get the value from the CAPI
      if status != Status.ok:
        raise RuntimeError(f"Failed to set value for {mapped_cref}: {status}")
    else:
      raise ValueError(f"CRef {cref} not found in mapped CRefs.")

  def getValue(self, cref: CRef):
    ##TODO check the var type and call the correct CAPI function
    name = ".".join(cref.names[:-1])
    if name in self.mappedCrefs:
      mapped_cref = ".".join([self.mappedCrefs[name], cref.names[-1]])
      value, status = Capi.getReal(mapped_cref) # Get the value from the CAPI
      if status != Status.ok:
        raise RuntimeError(f"Failed to get value for {mapped_cref}: {status}")
      return value
    else:
      raise ValueError(f"CRef {cref} not found in mapped CRefs.")

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

  def setResultFile(self, filename: str):
    status = Capi.setResultFile("model", filename)
    if status !=Status.ok:
      raise RuntimeError(f"Failed to setResultFile {filename}: {status}")

  def terminate(self):
    status = Capi.terminate(self.modelName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to terminate model: {status}")

  def delete(self):
    status = Capi.delete(self.modelName)
    if status != Status.ok:
      raise RuntimeError(f"Failed to delete model: {status}")
    self.modelName = None  # Clear the model name after deletion
    self.apiCall.clear()  # Clear the API call history
