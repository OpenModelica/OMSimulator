#CAPTION#
setTLMInitialValues
-------------------

Sets initial signal value or effort variables for a TLM interface.

One value argument for signals and 1D physical connections, or six values for 3D physical connecitons.
#END#

#LUA#
.. code-block:: lua
  
  status oms2_setTLMInitialValues(cref, ifc, value1, value2...))

#END#

#PYTHON#
.. code-block:: python

  # not yet available

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_setTLMInitialValues(const char *cref, const char *ifc, const double values[], int nvalues);
 
#END#

#DESCRIPTION#
#END#
