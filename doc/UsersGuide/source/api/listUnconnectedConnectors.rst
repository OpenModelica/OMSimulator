#CAPTION#
listUnconnectedConnectors
-------------------------

Lists all unconnected connectors of a given system.

Memory is allocated for `contents`. The caller is responsible to free it using
the C-API. The Lua and Python bindings take care of the memory and the caller
doesn't need to call free.
#END#

#LUA#
.. code-block:: lua

  contents, status = oms_listUnconnectedConnectors(cref)

#END#

#PYTHON#
.. code-block:: python

  contents, status = oms.listUnconnectedConnectors(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_listUnconnectedConnectors(const char* cref, char** contents);

#END#

#OMC#
.. code-block:: Modelica

  (contents, status) := oms_listUnconnectedConnectors(cref);

#END#

#DESCRIPTION#
#END#
