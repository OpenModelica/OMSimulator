from enum import Enum
from typing import Union

from OMSimulator.cref import CRef


class Causality(Enum):
  '''Enumeration for signal causality (oms_causality_enu_t).'''
  input = 0
  output = 1
  parameter = 2
  calculatedParameter = 3
  local = 4
  independent = 5


class SignalType(Enum):
  '''Enumeration for signal types (oms_signal_type_enu_t).'''
  Real = 0
  Integer = 1
  Boolean = 2
  String = 3
  Enumeration = 4


class Variable:
  '''Class for storing variable information'''
  def __init__(self, name: Union[str, CRef], description : str, valueReference: Union[str, int], causality, variability, signal_type, unit, start_value):
    self.name = CRef(name)
    self.description = description
    self.valueReference = int(valueReference)
    self.causality = causality if isinstance(causality, Causality) else Causality[causality]
    self.variability = variability
    self.signal_type = signal_type if isinstance(signal_type, SignalType) else SignalType[signal_type]
    self.unit = unit
    self.modelDescriptionStartValue = start_value
    self.unitDefinition = []

  def isInput(self):
    return self.causality == Causality.input

  def isOutput(self):
    return self.causality == Causality.output

  def isParameter(self):
    return self.causality == Causality.parameter

  def isCalculatedParameter(self):
    return self.causality == Causality.calculatedParameter
