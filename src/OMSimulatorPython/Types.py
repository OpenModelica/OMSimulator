from enum import Enum

class Status(Enum):
  'oms_status_enu_t'
  OK = 0
  WARNING = 1
  DISCARD = 2
  ERROR = 3
  FATAL = 4
  PENDING = 5

class System(Enum):
  'oms_system_enu_t'
  NONE = 0
  WC = 1 # Weakly Coupled system
  SC = 2 # Strongly Coupled system

class ModelState(Enum):
  'oms_modelState_enu_t'
  VIRGIN = 1
  ENTERINSTANTIATION = 2
  INSTANTIATED = 4
  INITIALIZATION = 8
  SIMULATION = 16
  ERROR = 32
