#CAPTION#
setTLMSocketData
----------------

Sets data for TLM socket communication.
#END#

#LUA#
.. code-block:: lua

  status = oms2_setTLMSocketData(cref, address, managerPort, monitorPort)

#END#

#PYTHON#
.. code-block:: python

  # not yet available

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_setTLMSocketData(const char* cref, const char* address, int managerPort, int monitorPort);

#END#

#DESCRIPTION#
#END#
