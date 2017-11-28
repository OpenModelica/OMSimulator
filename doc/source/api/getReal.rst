#CAPTION#
getReal
-------

This function returns a the value of a real variable.
#END#

#LUA#
.. code-block:: lua

  getReal(model, var)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "var", "String", "Name of a variable."
#END#

#CAPI#
.. code-block:: c

  double oms_getReal(void* model, const char* var);
#END#

#DESCRIPTION#
#END#
