#CAPTION#
setLogFile
----------

This function sets the log file name.
#END#

#LUA#
.. code-block:: lua

  setLogFile(filename)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "filename", "String", ""
#END#

#CAPI#
.. code-block:: c

  void oms_setLogFile(const char* filename);
#END#

#DESCRIPTION#
If the filename is an empty string, then the logging information is dumped to stdout.
#END#
