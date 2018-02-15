#CAPTION#
unload
------

This function releases all resources used for a given composite model.
#END#

#LUA#
.. code-block:: lua

  unload(model)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
#END#

#CAPI#
.. code-block:: c

  void oms_unload(void* model);
#END#

#DESCRIPTION#
#END#
