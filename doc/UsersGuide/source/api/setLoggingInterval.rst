#CAPTION#
setLoggingInterval
------------------

Set the logging interval of the simulation.
#END#

#LUA#
.. code-block:: lua

  status = oms_setLoggingInterval(cref, loggingInterval)

#END#

#PYTHON#
.. code-block:: python

  status = oms.setLoggingInterval(cref, loggingInterval)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setLoggingInterval(const char* cref, double loggingInterval);

#END#

#OMC#
.. code-block:: modelica

  status := oms_setLoggingInterval(cref, loggingInterval);

#END#

#DESCRIPTION#
#END#
