#CAPTION#
getDirectionalDerivative
------------------------
This function computes the directional derivatives of an FMU.
#END#

#LUA#
.. code-block:: lua

  value, status = oms_getDirectionalDerivative(cref)

#END#

#PYTHON#
.. code-block:: python

  value, status = oms.getDirectionalDerivative(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getDirectionalDerivative(const char* cref, double* value);

#END#

#END#

#DESCRIPTION#
#END#
