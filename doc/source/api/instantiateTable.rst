#CAPTION#
instantiateTable
----------------

This function instantiates a lookup table and makes it available under a given name.
#END#

#LUA#
.. code-block:: lua

  instantiateTable(model, filename, instanceName)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "filename", "String", ""
  "instanceName", "String", ""
#END#

#CAPI#
.. code-block:: c

  void oms_instantiateTable(void* model, const char* filename, const char* instanceName);
#END#

#DESCRIPTION#
mat files and csv files can be used as lookup tables.
#END#
