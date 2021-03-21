#CAPTION#
setTLMSocketData
----------------

Sets data for TLM socket communication.
#END#

#LUA#
.. code-block:: lua

  status = oms_setTLMSocketData(cref, address, managerPort, monitorPort)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setTLMSocketData(const char* cref, const char* address, int managerPort, int monitorPort);

#END#

#OMC#
.. code-block:: modelica

  status := oms_setTLMSocketData(cref, address, managerPort, monitorPort);

#END#

#DESCRIPTION#
#END#
