#CAPTION#
deleteConnection
----------------

Deletes the connection between connectors A and B.
#END#

#LUA#
.. code-block:: lua

  status = oms2_deleteConnection(cref, conA, conB)

#END#

#PYTHON#
.. code-block:: python

  # not yet available

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_deleteConnection(const char* cref, const char* conA, const char* conB);

#END#

#DESCRIPTION#
#END#
