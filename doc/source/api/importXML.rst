#CAPTION#
importXML
---------

This function loads an composite model from a given xml file.
#END#

#LUA#
.. code-block:: lua

  importXML(model, filename)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "filename", "String", ""
#END#

#CAPI#
.. code-block:: c

  void oms_importXML(void* model, const char* filename);
#END#

#DESCRIPTION#
#END#
