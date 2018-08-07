#CAPTION#
connectSolver
-------------

Connects a solver instance to a given FMU of a FMI composite model.
#END#

#LUA#
.. code-block:: lua

  status = oms2_connectSolver(model, name, fmu)

#END#

#PYTHON#
.. code-block:: python

  status = session.connectSolver(model, name, fmu)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_connectSolver(const char* model, const char* name, const char* fmu);

#END#

#DESCRIPTION#
#END#
