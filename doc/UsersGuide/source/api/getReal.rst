#CAPTION#
getReal
-------

Get real value.
#END#

#LUA#
.. code-block:: lua

  value, status = oms2_getReal(signal)

#END#

#PYTHON#
.. code-block:: python

  value, status = session.getReal(signal)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_getReal(const char* signal, double* value);

#END#

#DESCRIPTION#
#END#
