#CAPTION#
setFixedStepSize
----------------

Sets the fixed step size. Can be used for the communication step size of
co-simulation systems and also for the integrator step size in model exchange
systems.
#END#

#LUA#
.. code-block:: lua

  status = oms_setFixedStepSize(cref, stepSize)

#END#

#PYTHON#
.. code-block:: python

  status = oms.setFixedStepSize(cref, stepSize)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setFixedStepSize(const char* cref, double stepSize);

#END#

#DESCRIPTION#
#END#
