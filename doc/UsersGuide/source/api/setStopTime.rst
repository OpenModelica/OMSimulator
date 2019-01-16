#CAPTION#
setStopTime
-----------

Set the stop time of the simulation.
#END#

#LUA#
.. code-block:: lua

  status = oms3_setStopTime(cref, stopTime)

#END#

#PYTHON#
.. code-block:: python

  status = session.oms3_setStopTime(cref, stopTime)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_setStopTime(const char* cref, double stopTime);

#END#

#DESCRIPTION#
#END#
