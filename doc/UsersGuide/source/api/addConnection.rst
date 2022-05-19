#CAPTION#
addConnection
-------------

Adds a new connection between connectors `A` and `B`. The connectors need to be
specified as fully qualified component references, e.g., `"model.system.component.signal"`.
#END#

#LUA#
.. code-block:: lua

  status = oms_addConnection(crefA, crefB, suppressUnitConversion)

#END#

#PYTHON#
.. code-block:: python

  status = oms.addConnection(crefA, crefB, suppressUnitConversion)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_addConnection(const char* crefA, const char* crefB, bool suppressUnitConversion);

#END#

#OMC#
.. code-block:: modelica

  status := oms_addConnection(crefA, crefB, suppressUnitConversion);

#END#

#DESCRIPTION#
The two arguments `crefA` and `crefB` get swapped automatically if necessary. The third argument suppressUnitConversion is
optional and the default value is `false` which allows automatic unit conversion between connections, if set to `true` then
automatic unit conversion will be disabled.
#END#
