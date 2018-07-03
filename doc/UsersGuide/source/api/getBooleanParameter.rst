#CAPTION#
getBooleanParameter
-------------------

Get boolean value of given parameter.
#END#

#LUA#
.. code-block:: lua

  value, status = oms2_getBooleanParameter(signal)

#END#

#PYTHON#
.. code-block:: python

  value, status = session.getBooleanParameter(signal)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_getBooleanParameter(const char* signal, bool* value);

#END#

#DESCRIPTION#
#END#
