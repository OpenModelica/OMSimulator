from OMSimulator import types


class Variable:
  def __init__(self, name: str, valueReference: str = None, causality: str = None, variability: str = None, type: str = None, unit: str = None, start: str = None):
    self.name = name
    self.valueReference = valueReference
    self.causality = causality
    self.variablity = variability
    self.type = type
    self.unit = unit
    self.modelDescriptionStartValue = start
    self.startValue = None ## start values provided by user
    self.unitDefinition = []

  def isInput(self):
    return self.causality == "input"

  def isOutput(self):
    return self.causality == "output"

  def isParameter(self):
    return self.causality == "parameter"

  def setStartValue(self, value):
    self.startValue = value

  def setUnitDefinitions(self, unitDefinition):
    self.unitDefinition.append(unitDefinition)

  def isCalculatedParameter(self):
    return self.causality == "calculatedParameter"

  def getCausalityTypeFromString(self, causality : str):
    causality_map = {
      "input": types.Causality.input,
      "output": types.Causality.output,
      "parameter": types.Causality.parameter,
      "calculatedParameter": types.Causality.calculatedParameter
    }
    return causality_map.get(causality)

  def getSignalTypeFromString(self, type : str):
    signalTypes_map = {
        "Real": types.SignalType.real,
        "Integer": types.SignalType.integer,
        "Boolean" : types.SignalType.boolean,
        "Enumeration" :types.SignalType.enum,
        "String": types.SignalType.string
    }
    return signalTypes_map.get(type)
