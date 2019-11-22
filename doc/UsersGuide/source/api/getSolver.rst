#CAPTION#
getSolver
---------

Gets the selected solver method of the given system.
#END#

#LUA#
.. code-block:: lua

  solver, status = oms_getSolver(cref)

#END#

#PYTHON#
.. code-block:: python

  solver, status = oms.getSolver(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getSolver(const char* cref, oms_solver_enu_t* solver);

#END#

#OMC#
.. code-block:: Modelica

  (solver, status) := oms_getSolver(cref);

#END#

#DESCRIPTION#
#END#
