#CAPTION#
addExternalModel
----------------

Adds an external model to a TLM composite model.
#END#

#LUA#
.. code-block:: lua

  status = oms2_addExternalModel(cref, name, modelfile, startscript)

#END#

#PYTHON#
.. code-block:: python

  status = session.addExternalModel(cref, name, modelfile, startscript)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_addExternalModel(const char* cref, const char* name, const char* modelfile, const char* startscript);

#END#

#DESCRIPTION#
#END#
