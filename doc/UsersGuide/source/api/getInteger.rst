#CAPTION#
getInteger
----------

Get integer value of given signal.
#END#

#LUA#
.. code-block:: lua

  value, status = oms_getInteger(cref)

#END#

#PYTHON#
.. code-block:: python

  value, status = oms.getInteger(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getInteger(const char* cref, int* value);

#END#

#DESCRIPTION#
#END#
