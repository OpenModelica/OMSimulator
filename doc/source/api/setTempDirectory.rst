#CAPTION#
setTempDirectory
----------------

This function defines the temp directory that is used for temporary files, e.g. extracted FMUs.
#END#

#LUA#
.. code-block:: lua

  setTempDirectory(filename)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "filename", "String", ""
#END#

#CAPI#
.. code-block:: c

  void oms_setTempDirectory(const char* filename);
#END#

#DESCRIPTION#
#END#
