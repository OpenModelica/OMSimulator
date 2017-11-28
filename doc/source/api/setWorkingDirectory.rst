#CAPTION#
setWorkingDirectory
-------------------

This function defines the working directory, which is the root of all relative paths used later on.
#END#

#LUA#
.. code-block:: lua

  setWorkingDirectory(path);

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "path", "String", "Path to the new working directory. This can either be a absolut or relative path."
#END#

#CAPI#
.. code-block:: c

  void oms_setWorkingDirectory(const char* path);
#END#

#DESCRIPTION#
#END#
