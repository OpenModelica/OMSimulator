#CAPTION#
addConnectorToTLMBus
--------------------

Adds a connector to a TLM bus.
#END#

#LUA#
.. code-block:: lua

  status = oms_addConnectorToTLMBus(busCref, connectorCref, type)

#END#

#PYTHON#
.. code-block:: python

  status = oms.addConnectorToTLMBus(busCref, connectorCref, type)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_addConnectorToTLMBus(const char* busCref, const char* connectorCref, const char *type);

#END#

#OMC#
.. code-block:: Modelica

  status := oms_addConnectorToTLMBus(busCref, connectorCref, type);

#END#

#DESCRIPTION#
#END#
