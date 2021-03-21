#CAPTION#
deleteConnectorFromTLMBus
-------------------------

Deletes a connector from a given TLM bus.
#END#

#LUA#
.. code-block:: lua

  status = oms_deleteConnectorFromTLMBus(busCref, connectorCref)

#END#

#PYTHON#
.. code-block:: python

  status = oms.deleteConnectorFromTLMBus(busCref, connectorCref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_deleteConnectorFromTLMBus(const char* busCref, const char* connectorCref);

#END#

#OMC#
.. code-block:: modelica

  status := oms_deleteConnectorFromTLMBus(busCref, connectorCref);

#END#

#DESCRIPTION#
#END#
