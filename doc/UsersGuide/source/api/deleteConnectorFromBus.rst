#CAPTION#
deleteConnectorFromBus
----------------------

Deletes a connector from a given bus.
#END#

#LUA#
.. code-block:: lua

  status = oms_deleteConnectorFromBus(busCref, connectorCref)

#END#

#PYTHON#
.. code-block:: python

  status = session.oms_deleteConnectorFromBus(busCref, connectorCref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_deleteConnectorFromBus(const char* busCref, const char* connectorCref);

#END#

#DESCRIPTION#
#END#
