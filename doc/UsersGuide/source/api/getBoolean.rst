#CAPTION#
getBoolean
----------

Get boolean value of given signal.
#END#

#LUA#
.. code-block:: lua

  value, status = oms3_getBoolean(cref)

#END#

#PYTHON#
.. code-block:: python

  value, status = session.oms3_getBoolean(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_getBoolean(const char* cref, bool* value);

#END#

#DESCRIPTION#
#END#
