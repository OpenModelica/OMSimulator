#CAPTION#
setReal
-------

Set parameter and input values of type real.
#END#

#LUA#
.. code-block:: lua

  setReal(model, var, value)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "var", "String", ""
  "value", "Number", ""
#END#

#CAPI#
.. code-block:: c

  void oms_setReal(void* model, const char* var, double value);
#END#

#DESCRIPTION#
#END#
