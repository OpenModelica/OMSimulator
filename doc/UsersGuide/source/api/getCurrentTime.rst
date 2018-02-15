#CAPTION#
getCurrentTime
--------------

This function returns the current simulation time value.
#END#

#LUA#
.. code-block:: lua

  getCurrentTime(model)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
#END#

#CAPI#
.. code-block:: c

  oms_status_t oms_getCurrentTime(const void* model, double* time);
#END#

#DESCRIPTION#
#END#
