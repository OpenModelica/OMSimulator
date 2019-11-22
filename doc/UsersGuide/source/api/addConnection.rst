#CAPTION#
addConnection
-------------

Adds a new connection between connectors `A` and `B`. The connectors need to be
specified as fully qualified component references, e.g., `"model.system.component.signal"`.
#END#

#LUA#
.. code-block:: lua

  status = oms_addConnection(crefA, crefB)

#END#

#PYTHON#
.. code-block:: python

  status = oms.addConnection(crefA, crefB)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_addConnection(const char* crefA, const char* crefB);

#END#

#OMC#
.. code-block:: Modelica

  status := oms_addConnection(crefA, crefB);

#END#

#DESCRIPTION#
The two arguments `crefA` and `crefB` get swapped automatically if necessary.
#END#
