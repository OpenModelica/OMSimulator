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

#END#

#PYTHON#
.. code-block:: python

  status = oms.setSolver(cref, solver)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setSolver(const char* cref, oms_solver_enu_t solver);

#END#

#DESCRIPTION#
#END#
