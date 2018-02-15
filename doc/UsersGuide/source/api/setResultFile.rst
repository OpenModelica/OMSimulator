#CAPTION#
setResultFile
-------------

This functions defines the result file name of the given composite model.
#END#

#LUA#
.. code-block:: lua

  setResultFile(model, filename)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "filename", "String", ""
#END#

#CAPI#
.. code-block:: c

  void oms_setResultFile(void* model, const char* filename);
#END#

#DESCRIPTION#
mat files and csv files are supported.
#END#
