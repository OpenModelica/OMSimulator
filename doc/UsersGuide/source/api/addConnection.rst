#CAPTION#
addConnection
-------------

Adds a new connection to a given parent component.
#END#

#LUA#
.. code-block:: lua

  status = oms2_addConnection(cref, conA, conB)

#END#

#PYTHON#
.. code-block:: python

  # not yet available

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_addConnection(const char* cref, const char* conA, const char* conB);

#END#

#DESCRIPTION#
The two arguments `from` and `to` get swapped automatically if necessary.
#END#
