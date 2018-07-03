#CAPTION#
getBoolean
----------

Get boolean value of given signal.
#END#

#LUA#
.. code-block:: lua

  value, status = oms2_getBoolean(signal)

#END#

#PYTHON#
.. code-block:: python

  value, status = session.getBoolean(signal)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_getBoolean(const char* signal, bool* value);

#END#

#DESCRIPTION#
#END#
