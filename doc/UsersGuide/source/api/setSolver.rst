#CAPTION#
setSolver
---------

Sets the solver method for the given system.
#END#


#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setSolver(const char* cref, oms_solver_enu_t solver);

#END#

#OMC#
.. code-block:: modelica

  status := oms_setSolver(cref, solver);

  The second argument "solver" should be any of the following,

  "OpenModelica.Scripting.oms_solver.oms_solver_none"
  "OpenModelica.Scripting.oms_solver.oms_solver_sc_min"
  "OpenModelica.Scripting.oms_solver.oms_solver_sc_explicit_euler"
  "OpenModelica.Scripting.oms_solver.oms_solver_sc_cvode"
  "OpenModelica.Scripting.oms_solver.oms_solver_sc_max"
  "OpenModelica.Scripting.oms_solver.oms_solver_wc_min"
  "OpenModelica.Scripting.oms_solver.oms_solver_wc_ma"
  "OpenModelica.Scripting.oms_solver.oms_solver_wc_mav"
  "OpenModelica.Scripting.oms_solver.oms_solver_wc_mav2"
  "OpenModelica.Scripting.oms_solver.oms_solver_wc_max"

#END#
#DESCRIPTION#
#END#
