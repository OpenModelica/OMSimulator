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

  value, status = session.oms_getBoolean(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getBoolean(const char* cref, bool* value);

#END#

#DESCRIPTION#
#END#
