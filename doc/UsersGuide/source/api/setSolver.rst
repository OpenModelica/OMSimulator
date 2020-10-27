#CAPTION#
setSolver
---------

Sets the solver method for the given system.
#END#

#LUA#
.. code-block:: lua

  status = oms_setSolver(cref, solver)

.. csv-table::
  :header: "solver", "Type", "Description"
  :widths: 20, 10, 35

  "oms_solver_sc_explicit_euler", "sc-system", "Explicit euler with fixed step size"
  "oms_solver_sc_cvode", "sc-system", "CVODE with adaptive stepsize"
  "oms_solver_wc_ma", "wc-system", "default master algorithm with fixed step size"
  "oms_solver_wc_mav", "wc-system", "master algorithm with adaptive stepsize"
  "oms_solver_wc_mav2", "wc-system", "master algorithm with adaptive stepsize (double-step)"

#END#

#PYTHON#
.. code-block:: python

  status = oms.setSolver(cref, solver)

.. csv-table::
  :header: "solver", "Type", "Description"
  :widths: 20, 10, 35

  "oms.solver_sc_explicit_euler", "sc-system", "Explicit euler with fixed step size"
  "oms.solver_sc_cvode", "sc-system", "CVODE with adaptive stepsize"
  "oms.solver_wc_ma", "wc-system", "default master algorithm with fixed step size"
  "oms.solver_wc_mav", "wc-system", "master algorithm with adaptive stepsize"
  "oms.solver_wc_mav2", "wc-system", "master algorithm with adaptive stepsize (double-step)"

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setSolver(const char* cref, oms_solver_enu_t solver);

#END#

#OMC#
.. code-block:: Modelica

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
  "OpenModelica.Scripting.oms_solver.oms_solver_wc_assc"
  "OpenModelica.Scripting.oms_solver.oms_solver_wc_mav2"
  "OpenModelica.Scripting.oms_solver.oms_solver_wc_max"

#END#
#DESCRIPTION#
#END#
