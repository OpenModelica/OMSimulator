#CAPTION#
getStartTime
------------

Get the start time from the model.
#END#

#LUA#
.. code-block:: lua

  startTime, status = oms3_getStartTime(cref)

#END#

#PYTHON#
.. code-block:: python

  startTime, status = session.oms3_getStartTime(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_getStartTime(const char* cref, double* startTime);

#END#

#DESCRIPTION#
#END#
