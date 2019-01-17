#CAPTION#
listUnconnectedConnectors
-------------------------

Lists all unconnected connectors of a given system.
#END#

#LUA#
.. code-block:: lua

  contents, status = oms_listUnconnectedConnectors(cref)

#END#

#PYTHON#
.. code-block:: python

  contents, status = session.oms_listUnconnectedConnectors(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_listUnconnectedConnectors(const char* cref, char** contents);

#END#

#DESCRIPTION#
#END#
