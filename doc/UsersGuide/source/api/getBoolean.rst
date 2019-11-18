#CAPTION#
getBoolean
----------

Get boolean value of given signal.
#END#

#LUA#
.. code-block:: lua

  value, status = oms_getBoolean(cref)

#END#

#PYTHON#
.. code-block:: python

  value, status = oms.getBoolean(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getBoolean(const char* cref, bool* value);

#END#

#OMC#
.. code-block:: Modelica

  (value, status) := oms_getBoolean(cref);

#END#

#DESCRIPTION#
#END#
