#CAPTION#
setRealInputDerivative
----------------------

Sets the first order derivative of a real input signal.

This can only be used for CS-FMU real input signals.
#END#

#LUA#
.. code-block:: lua

  status = oms_setRealInputDerivative(cref, value)

#END#

#PYTHON#
.. code-block:: python

  status = oms.setRealInputDerivative(cref, value)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setRealInputDerivative(const char* cref, double value);

#END#

#OMC#
.. code-block:: modelica

  status := oms_setRealInputDerivative(cref, value);

#END#

#DESCRIPTION#
#END#
