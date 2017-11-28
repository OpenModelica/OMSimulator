#CAPTION#
setInteger
----------

Set parameter and input values of type integer.
#END#

#LUA#
.. code-block:: lua

  setInteger(model, var, value)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "var", "String", ""
  "value", "Number", ""
#END#

#CAPI#
.. code-block:: c

  void oms_setInteger(void* model, const char* var, int value);
#END#

#DESCRIPTION#
#END#
