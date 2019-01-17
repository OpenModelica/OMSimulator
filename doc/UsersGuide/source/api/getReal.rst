#CAPTION#
getReal
-------

Get real value.
#END#

#LUA#
.. code-block:: lua

  value, status = oms_getReal(cref)

#END#

#PYTHON#
.. code-block:: python

  value, status = session.oms_getReal(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getReal(const char* cref, double* value);

#END#

#DESCRIPTION#
#END#
