#CAPTION#
stepUntil
---------

Simulates a composite model until a given time value.
#END#

#LUA#
.. code-block:: lua

  status = oms_stepUntil(cref, stopTime)

#END#

#PYTHON#
.. code-block:: python

  status = oms.stepUntil(cref, stopTime)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_stepUntil(const char* cref, double stopTime);

#END#

#DESCRIPTION#
#END#
