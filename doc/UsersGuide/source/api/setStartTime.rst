#CAPTION#
setStartTime
------------

Set the start time of the simulation.
#END#

#LUA#
.. code-block:: lua

  status = oms2_setStartTime(cref, startTime)

#END#

#PYTHON#
.. code-block:: python

  status = session.setStartTime(cref, startTime)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_setStartTime(const char* cref, double startTime);

#END#

#DESCRIPTION#
#END#
