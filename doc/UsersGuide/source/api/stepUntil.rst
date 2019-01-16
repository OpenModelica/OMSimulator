#CAPTION#
stepUntil
---------

Simulates a composite model until a given time value.
#END#

#LUA#
.. code-block:: lua

  status = oms3_stepUntil(cref, stopTime)

#END#

#PYTHON#
.. code-block:: python

  status = session.oms3_stepUntil(cref, stopTime)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_stepUntil(const char* cref, double stopTime);

#END#

#DESCRIPTION#
#END#
