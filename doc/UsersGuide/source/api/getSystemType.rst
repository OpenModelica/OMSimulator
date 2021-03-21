#CAPTION#
getSystemType
-------------

Gets the type of the given system.
#END#

#LUA#
.. code-block:: lua

  type, status = oms_getSystemType(cref)

#END#

#PYTHON#
.. code-block:: python

  type, status = oms.getSystemType(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getSystemType(const char* cref, oms_system_enu_t* type);

#END#

#OMC#
.. code-block:: modelica

  (type, status) := oms_getSystemType(cref);

#END#

#DESCRIPTION#
#END#
