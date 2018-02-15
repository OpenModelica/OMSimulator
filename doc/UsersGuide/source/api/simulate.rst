#CAPTION#
simulate
--------

This function performs the simulation from the current time value to the defined stop time.
#END#

#LUA#
.. code-block:: lua

  simulate(model)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
#END#

#CAPI#
.. code-block:: c

  oms_status_t oms_simulate(void* model);
#END#

#DESCRIPTION#
#END#
