#CAPTION#
getModelState
-------------

Gets the model state of the given model cref.
#END#

#LUA#
.. code-block:: lua

  modelState, status = oms_getModelState(cref)

#END#

#PYTHON#
.. code-block:: python

  # not available

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getModelState(const char* cref, oms_modelState_enu_t* modelState);

#END#

#OMC#
.. code-block:: Modelica

  (modelState, status) := oms_getModelState(cref);

#END#

#DESCRIPTION#
#END#
