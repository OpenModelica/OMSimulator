#CAPTION#
addConnection
-------------

This function adds a new connection to a composite model.
#END#

#LUA#
.. code-block:: lua

  -- model [ref] Opaque pointer to composite model.
  -- from  [in]  Name of an output variable.
  -- to    [in]  Name of an input variable.
  addConnection(model, from, to)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "from", "String", "Name of an output variable."
  "to", "String", "Name of an input variable."
#END#

#CAPI#
.. code-block:: c

  // model [ref] Opaque pointer to composite model.
  // from  [in]  Name of an output variable.
  // to    [in]  Name of an input variable.
  void oms_addConnection(void* model, const char* from, const char* to);
#END#

#DESCRIPTION#
The two arguments `from` and `to` get swapped if necessary.
#END#
