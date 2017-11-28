#CAPTION#
describe
--------

This command prints a summary of the given composite model to stdout.
#END#

#LUA#
.. code-block:: lua

  -- model: Opaque pointer to composite model
  describe(model)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "String", "Opaque pointer to composite model."
#END#

#CAPI#
.. code-block:: c

  // model [in] Opaque pointer to composite model.
  void oms_describe(void* model);
#END#

#DESCRIPTION#
#END#
