#CAPTION#
addConnectorToBus
-----------------

Adds a connector to a bus.
#END#

#LUA#
.. code-block:: lua

  status = oms_addConnectorToBus(busCref, connectorCref)

#END#

#PYTHON#
.. code-block:: python

  status = oms.addConnectorToBus(busCref, connectorCref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_addConnectorToBus(const char* busCref, const char* connectorCref);

#END#

#OMC#
.. code-block:: Modelica

  status := oms_addConnectorToBus(busCref, connectorCref);

#END#

#DESCRIPTION#
#END#
