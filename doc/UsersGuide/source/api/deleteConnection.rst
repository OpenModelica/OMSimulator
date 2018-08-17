#CAPTION#
deleteConnection
----------------

Deletes the connection between connectors A and B.
This can also be used to unconnect a solver from a given FMU.
#END#

#LUA#
.. code-block:: lua

  status = oms2_deleteConnection(cref, conA, conB)

#END#

#PYTHON#
.. code-block:: python

  status = session.deleteConnection(cref, conA, conB)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_deleteConnection(const char* cref, const char* conA, const char* conB);

#END#

#DESCRIPTION#
The two arguments `from` and `to` get swapped automatically if necessary.
#END#
