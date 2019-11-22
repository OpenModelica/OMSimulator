#CAPTION#
addExternalModel
----------------

Adds an external model to a TLM system.
#END#

#LUA#
.. code-block:: lua

  status = oms_addExternalModel(cref, path, startscript)

#END#

#PYTHON#
.. code-block:: python

  status = oms.addExternalModel(cref, path, startscript)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_addExternalModel(const char* cref, const char* path, const char* startscript);

#END#

#OMC#
.. code-block:: Modelica

  status := oms_addExternalModel(cref, path, startscript);

#END#

#DESCRIPTION#
#END#
