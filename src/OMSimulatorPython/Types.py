from enum import Enum

class Status(Enum):
  'oms_status_enu_t'
  OK = 0
  WARNING = 1
  DISCARD = 2
  ERROR = 3
  FATAL = 4
  PENDING = 5
