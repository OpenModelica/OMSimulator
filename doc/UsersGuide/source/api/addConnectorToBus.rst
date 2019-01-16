#CAPTION#
addConnectorToBus
-----------------

Adds a connector to a bus.
#END#

#LUA#
.. code-block:: lua

  status = oms3_addConnectorToBus(busCref, connectorCref)

#END#

#PYTHON#
.. code-block:: python

  status = session.oms3_addConnectorToBus(busCref, connectorCref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_addConnectorToBus(const char* busCref, const char* connectorCref);

#END#

#DESCRIPTION#
#END#
