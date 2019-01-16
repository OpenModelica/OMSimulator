#CAPTION#
getStopTime
-----------

Get the stop time from the model.
#END#

#LUA#
.. code-block:: lua

  stopTime, status = oms3_getStopTime(cref)

#END#

#PYTHON#
.. code-block:: python

  stopTime, status = session.oms3_getStopTime(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_getStopTime(const char* cref, double* stopTime);

#END#

#DESCRIPTION#
#END#
