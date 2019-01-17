#CAPTION#
setStopTime
-----------

Set the stop time of the simulation.
#END#

#LUA#
.. code-block:: lua

  status = oms_setStopTime(cref, stopTime)

#END#

#PYTHON#
.. code-block:: python

  status = session.oms_setStopTime(cref, stopTime)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setStopTime(const char* cref, double stopTime);

#END#

#DESCRIPTION#
#END#
