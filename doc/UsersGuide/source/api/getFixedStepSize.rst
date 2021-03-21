#CAPTION#
getFixedStepSize
----------------

Gets the fixed step size. Can be used for the communication step size of
co-simulation systems and also for the integrator step size in model exchange
systems.
#END#

#LUA#
.. code-block:: lua

  stepSize, status = oms_setFixedStepSize(cref)

#END#

#PYTHON#
.. code-block:: python

  stepSize, status = oms.getFixedStepSize(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getFixedStepSize(const char* cref, double* stepSize);

#END#

#OMC#
.. code-block:: modelica

  (stepSize, status) := oms_setFixedStepSize(cref);

#END#

#DESCRIPTION#
#END#
