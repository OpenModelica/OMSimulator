#CAPTION#
addSolver
---------

Adds a new solver instance to a given FMI composite model.
#END#

#LUA#
.. code-block:: lua

  status = oms2_addSolver(model, name, solver)

#END#

#PYTHON#
.. code-block:: python

  status = session.addSolver(model, name, solver)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_addSolver(const char* model, const char* name, const char* solver);

#END#

#DESCRIPTION#
#END#
