#CAPTION#
initialize
----------

This function initializes a given composite model. After this call, the model is in simulation mode.
#END#

#LUA#
.. code-block:: lua

  initialize(model)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
#END#

#CAPI#
.. code-block:: c

  oms_status_t oms_initialize(void* model);
#END#

#DESCRIPTION#
#END#
