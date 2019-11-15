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

  value, status = oms.getReal(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getReal(const char* cref, double* value);

#END#

#OMC#
.. code-block:: Modelica

  (value, status) := oms_getReal(cref);

#END#

#DESCRIPTION#
#END#
