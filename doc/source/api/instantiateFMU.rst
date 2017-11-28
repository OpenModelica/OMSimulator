#CAPTION#
instantiateFMU
--------------

This function instantiates a FMU and makes it available under a given name.
#END#

#LUA#
.. code-block:: lua

  instantiateFMU(model, filename, instanceName)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "filename", "String", ""
  "instanceName", "String", ""
#END#

#CAPI#
.. code-block:: c

  void oms_instantiateFMU(void* model, const char* filename, const char* instanceName);
#END#

#DESCRIPTION#
#END#
