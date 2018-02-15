#CAPTION#
loadModel
---------

This function loads an composite model from a given xml file.
#END#

#LUA#
.. code-block:: lua

  -- filename [in]  Path to xml file.
  -- model    [out] Opaque pointer to composite model.
  model = loadModel(filename)
#END#

#CAPI#
.. code-block:: c

  void* oms_loadModel(const char* filename);
#END#

#DESCRIPTION#
#END#
