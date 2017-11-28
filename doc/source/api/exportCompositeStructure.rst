#CAPTION#
exportCompositeStructure
------------------------

This function generates a dot file representing the composite structure of a given model.
#END#

#LUA#
.. code-block:: lua

  exportCompositeStructure(model, prefix)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "prefix", "String", "Filename prefix."
#END#

#CAPI#
.. code-block:: c

  void oms_exportCompositeStructure(void* model, const char* prefix);
#END#

#DESCRIPTION#
The dot file can be translated to an png file using this command:

`dot -Gsplines=none <model.dot> | neato -n -Gsplines=ortho -Tpng -o<model.png>`
#END#
