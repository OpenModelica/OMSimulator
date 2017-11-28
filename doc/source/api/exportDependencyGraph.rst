#CAPTION#
exportDependencyGraph
---------------------

This function generates two dot files representing the dependencies of the initialization and simulation system, including local (inner) dependencies.
#END#

#LUA#
.. code-block:: lua

  exportDependencyGraph(model, prefix)

.. csv-table::
  :header: "Input", "Type", "Description"
  :widths: 15, 10, 40

  "model", "Object", "Opaque pointer to composite model."
  "prefix", "String", "Filename prefix."
#END#

#CAPI#
.. code-block:: c

  void oms_exportDependencyGraph(void* model, const char* prefix);
#END#

#DESCRIPTION#
#END#
