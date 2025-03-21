from enum import Enum


class ModelState(Enum):
  'oms_modelState_enu_t'
  VIRGIN = 1
  ENTERINSTANTIATION = 2
  INSTANTIATED = 4
  INITIALIZATION = 8
  SIMULATION = 16
  ERROR = 32

class Causality(Enum):
  "oms_causality_enu_t"
  input = 0
  output = 1
  parameter = 2
  calculatedParameter = 3
  bidir = 4
  undefined = 5

class SignalType(Enum):
  "oms_signal_type_enu_t"
  real = 0
  integer = 1
  boolean = 2
  string = 3
  enum = 4

class Solver(Enum):
  "oms_solver_enu_t"
  oms_solver_sc_explicit_euler = 1
  oms_solver_sc_cvode = 2
  oms_solver_wc_ma = 3
  oms_solver_wc_mav = 4
  oms_solver_wc_mav2 = 5
