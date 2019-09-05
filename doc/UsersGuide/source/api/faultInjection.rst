#CAPTION#
faultInjection
--------------

Defines a new fault injection block.
#END#

#LUA#
.. code-block:: lua

  status = oms_faultInjection(cref, type, value)

#END#

#PYTHON#
.. code-block:: python

  status = oms.faultInjection(cref, type, value)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_faultInjection(const char* signal, oms_fault_type_enu_t faultType, double faultValue);

#END#

#DESCRIPTION#

.. csv-table::
  :header: "type", ""Description"
  :widths: 15, 50

  "oms_fault_type_bias", "y = y.$original + faultValue"
  "oms_fault_type_gain", "y = y.$original * faultValue"
  "oms_fault_type_const", "y = faultValue"

#END#
