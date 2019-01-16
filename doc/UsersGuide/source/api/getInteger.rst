#CAPTION#
getInteger
----------

Get integer value of given signal.
#END#

#LUA#
.. code-block:: lua

  value, status = oms3_getInteger(cref)

#END#

#PYTHON#
.. code-block:: python

  value, status = session.oms3_getInteger(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_getInteger(const char* cref, int* value);

#END#

#DESCRIPTION#
#END#
