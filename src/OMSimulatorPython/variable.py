from enum import Enum
from sqlite3 import Binary
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
  ## fmi3 types
  Float32   = 5
  Float64   = 6
  Int8      = 7
  UInt8     = 8
  Int16     = 9
  UInt16    = 10
  Int32     = 11
  UInt32    = 12
  Int64     = 13
  UInt64    = 14
  Binary    = 15

class FMI3Type:
  """Base class for all FMI3 types."""
  def __init__(self, value):
    self.value = value

  def __repr__(self):
    return f"{self.__class__.__name__}({self.value})"


# Float types
class Float64(FMI3Type): pass
class Float32(FMI3Type): pass

# Integer types
class Int64(FMI3Type): pass
class Int32(FMI3Type): pass
class Int16(FMI3Type): pass
class Int8(FMI3Type): pass

# Unsigned Integer types
class UInt64(FMI3Type): pass
class UInt32(FMI3Type): pass
class UInt16(FMI3Type): pass
class UInt8(FMI3Type): pass

class Binary(FMI3Type): pass

class Variable:
  '''Class for storing variable information'''
  def __init__(self, name: Union[str, CRef], description : str, valueReference: Union[str, int], causality, variability, signal_type, unit, start_value, declaredType):
    self.name = CRef(name)
    self.description = description
    self.valueReference = int(valueReference)
    self.causality = causality if isinstance(causality, Causality) else Causality[causality]
    self.variability = variability
    self.signal_type = signal_type if isinstance(signal_type, SignalType) else SignalType[signal_type]
    self.unit = unit
    self.modelDescriptionStartValue = start_value
    self.declaredType = declaredType
    self.unitDefinition = []
    self.enumerationDefinition = []

  def isInput(self):
    return self.causality == Causality.input

  def isOutput(self):
    return self.causality == Causality.output

  def isParameter(self):
    return self.causality == Causality.parameter

  def isCalculatedParameter(self):
    return self.causality == Causality.calculatedParameter
