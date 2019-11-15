#CAPTION#
deleteConnection
----------------

Deletes the connection between connectors `crefA` and `crefB`.
#END#

#LUA#
.. code-block:: lua

  status = oms_deleteConnection(crefA, crefB)

#END#

#PYTHON#
.. code-block:: python

  status = oms.deleteConnection(crefA, crefB)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_deleteConnection(const char* crefA, const char* crefB);

#END#

#OMC#
.. code-block:: Modelica

  status := oms_deleteConnection(crefA, crefB);

#END#

#DESCRIPTION#
The two arguments `crefA` and `crefB` get swapped automatically if necessary.
#END#
