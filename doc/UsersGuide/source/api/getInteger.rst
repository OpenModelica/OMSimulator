#CAPTION#
getInteger
----------

This function returns a the value of an integer variable.
#END#

#LUA#
.. code-block:: lua

  getInteger(model, var)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "var", "String", "Name of a variable."
#END#

#CAPI#
.. code-block:: c

  double oms_getInteger(void* model, const char* var);
#END#

#DESCRIPTION#
#END#
