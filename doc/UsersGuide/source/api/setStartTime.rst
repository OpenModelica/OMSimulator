#CAPTION#
setStartTime
------------

Set the start time of the simulation.
#END#

#LUA#
.. code-block:: lua

  status = oms_setStartTime(cref, startTime)

#END#

#PYTHON#
.. code-block:: python

  status = oms.setStartTime(cref, startTime)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setStartTime(const char* cref, double startTime);

#END#

#OMC#
.. code-block:: Modelica

  status := oms_setStartTime(cref, startTime);

#END#

#DESCRIPTION#
#END#
