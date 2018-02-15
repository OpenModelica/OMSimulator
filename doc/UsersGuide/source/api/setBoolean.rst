#CAPTION#
setBoolean
----------

Set parameter and input values of type boolean.
#END#

#LUA#
.. code-block:: lua

  setBoolean(model, var, value)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "var", "String", ""
  "value", "Number", ""
#END#

#CAPI#
.. code-block:: c

  void oms_setBoolean(void* model, const char* var, int value);
#END#

#DESCRIPTION#
#END#
