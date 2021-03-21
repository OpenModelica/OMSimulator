#CAPTION#
setInteger
----------

Sets the value of a given integer signal.
#END#

#LUA#
.. code-block:: lua

  status = oms_setInteger(cref, value)

#END#

#PYTHON#
.. code-block:: python

  status = oms.setInteger(cref, value)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setInteger(const char* cref, int value);

#END#

#OMC#
.. code-block:: modelica

  status := oms_setInteger(cref, value);

#END#

#DESCRIPTION#
#END#
