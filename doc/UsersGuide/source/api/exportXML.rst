#CAPTION#
exportXML
---------

This function generates a xml file representing the given model. The xml file contains the FMU instances, connections, parameter and simulation settings.
#END#

#LUA#
.. code-block:: lua

  exportXML(model, filename)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "filename", "String", ""
#END#

#CAPI#
.. code-block:: c

  void oms_exportXML(void* model, const char* filename);
#END#

#DESCRIPTION#
#END#
