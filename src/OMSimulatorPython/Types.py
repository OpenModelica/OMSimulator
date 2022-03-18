from enum import Enum

class Status(Enum):
  '''oms_status_enu_t'''
  OK = 0
  WARNING = 1
  DISCARD = 2
  ERROR = 3
  FATAL = 4
  PENDING = 5

class System(Enum):
  '''oms_system_enu_t'''
  NONE = 0
  TLM = 1 # TLM system
  WC = 2  # Weakly Coupled system
  SC = 3  # Strongly Coupled system

class ModelState(Enum):
  '''oms_modelState_enu_t'''
  VIRGIN = 1
  ENTERINSTANTIATION = 2
  INSTANTIATED = 4
  INITIALIZATION = 8
  SIMULATION = 16
  ERROR = 32

class MessageType(Enum):
  '''oms_message_type_enu_t'''
  INFO = 0    # Informative message
  WARNING = 1 # Warning; Non-critical issues
  ERROR = 2   # Error; Potentially critical issues
  DEBUG = 3   # Debug message (only in debug configuration)
  TRACE = 4   # Trace message for detailed debug logging (only in debug configuration)
