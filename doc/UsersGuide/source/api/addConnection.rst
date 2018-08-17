#CAPTION#
addConnection
-------------

Adds a new connection between connectors A and B to a given parent component.
This command can also be used to connect a solver instance to a given FMU of a FMI composite model.
#END#

#LUA#
.. code-block:: lua

  status = oms2_addConnection(cref, conA, conB)

#END#

#PYTHON#
.. code-block:: python

  status = session.addConnection(cref, conA, conB)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_addConnection(const char* cref, const char* conA, const char* conB);

#END#

#DESCRIPTION#
The two arguments `from` and `to` get swapped automatically if necessary.
#END#
