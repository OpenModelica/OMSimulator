#CAPTION#
terminate
---------

This function terminates a simulation and resets all FMU instances.
#END#

#LUA#
.. code-block:: lua

  terminate(model)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
#END#

#CAPI#
.. code-block:: c

  oms_status_t oms_terminate(void* model);
#END#

#DESCRIPTION#
#END#
