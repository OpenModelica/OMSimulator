#CAPTION#
addConnectorToTLMBus
--------------------

Adds a connector to a TLM bus.
#END#

#LUA#
.. code-block:: lua

  status = oms3_addConnectorToTLMBus(busCref, connectorCref, type)

#END#

#PYTHON#
.. code-block:: python

  status = session.oms3_addConnectorToTLMBus(busCref, connectorCref, type)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_addConnectorToTLMBus(const char* busCref, const char* connectorCref, const char *type);

#END#

#DESCRIPTION#
#END#
