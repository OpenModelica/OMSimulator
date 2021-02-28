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
  TLM = 1 # TLM system
  WC = 2 # Weakly Coupled system
  SC = 3 # Strongly Coupled system
