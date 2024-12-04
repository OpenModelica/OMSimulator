#CAPTION#
setTolerance
------------

Sets the tolerance for a given model or system.
#END#

#LUA#
.. code-block:: lua

  status = oms_setTolerance(const char* cref, double relativeTolerance)

#END#

#PYTHON#
.. code-block:: python

  status = oms.setTolerance(const char* cref,  double relativeTolerance)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setTolerance(const char* cref, double relativeTolerance);

#END#

#OMC#
.. code-block:: modelica

  status := oms_setTolerance(const char* cref, double relativeTolerance);

#END#

#DESCRIPTION#
Default values are `1e-4` for both relative and absolute tolerances.

A tolerance specified for a model is automatically applied to its root system,
i.e. both calls do exactly the same:


.. code-block:: c

  oms_setTolerance("model", relativeTolerance);
  oms_setTolerance("model.root", relativeTolerance);


Component, e.g. FMUs, pick up the tolerances from there system. That means
it is not possible to define different tolerances for FMUs in the same system
right now.

In a strongly coupled system (`oms_system_sc`), the relative tolerance is used
for CVODE and the absolute tolerance is used to solve algebraic loops.

In a weakly coupled system (`oms_system_wc`), both the relative and absolute
tolerances are used for the adaptive step master algorithms and the absolute
tolerance is used to solve algebraic loops.
#END#
