#CAPTION#
deleteConnectorFromTLMBus
-------------------------

Deletes a connector from a given TLM bus.
#END#

#LUA#
.. code-block:: lua

  status = oms3_deleteConnectorFromTLMBus(busCref, connectorCref)

#END#

#PYTHON#
.. code-block:: python

  status = session.oms3_deleteConnectorFromTLMBus(busCref, connectorCref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_deleteConnectorFromTLMBus(const char* busCref, const char* connectorCref);

#END#

#DESCRIPTION#
#END#
