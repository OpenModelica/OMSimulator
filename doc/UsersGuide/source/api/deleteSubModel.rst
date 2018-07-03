#CAPTION#
deleteSubModel
--------------

Deletes a sub-model instance, e.g. FMU, from a given FMI composite model.
#END#

#LUA#
.. code-block:: lua

  status = oms2_deleteSubModel(modelIdent, subModelIdent)

#END#

#PYTHON#
.. code-block:: python

  status = session.deleteSubModel(modelIdent, subModelIdent)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_deleteSubModel(const char* modelIdent, const char* subModelIdent);

#END#

#DESCRIPTION#
#END#
