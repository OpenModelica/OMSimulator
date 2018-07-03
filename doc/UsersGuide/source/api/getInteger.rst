#CAPTION#
getInteger
----------

Get integer value of given signal.
#END#

#LUA#
.. code-block:: lua

  value, status = oms2_getInteger(signal)

#END#

#PYTHON#
.. code-block:: python

  value, status = session.getInteger(signal)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_getInteger(const char* signal, int* value);

#END#

#DESCRIPTION#
#END#
