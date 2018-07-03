#CAPTION#
setLoggingInterval
------------------

Set the logging interval of the simulation.
#END#

#LUA#
.. code-block:: lua

  status = oms2_setLoggingInterval(cref, loggingInterval)

#END#

#PYTHON#
.. code-block:: python

  status = session.setLoggingInterval(cref, loggingInterval)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_setLoggingInterval(const char* cref, double loggingInterval);

#END#

#DESCRIPTION#
#END#
