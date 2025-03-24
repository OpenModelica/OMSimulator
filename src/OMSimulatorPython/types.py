from enum import Enum


class ModelState(Enum):
  'oms_modelState_enu_t'
  VIRGIN = 1
  ENTERINSTANTIATION = 2
  INSTANTIATED = 4
  INITIALIZATION = 8
  SIMULATION = 16
  ERROR = 32

class Solver(Enum):
  "oms_solver_enu_t"
  oms_solver_sc_explicit_euler = 1
  oms_solver_sc_cvode = 2
  oms_solver_wc_ma = 3
  oms_solver_wc_mav = 4
  oms_solver_wc_mav2 = 5
