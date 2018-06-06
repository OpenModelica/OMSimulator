#CAPTION#
omsi_getState
-------------

Get state of SysIdent model object.
#END#

#LUA#
.. code-block:: lua

  -- simodel [inout] SysIdent model as opaque pointer.
  -- state   [out] State of SysIdent model.
  -- status  [out] Error status.
  status, state = omsi_getState(simodel)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t omsi_getState(void* simodel, omsi_simodelstate_t* state);

#END#

#DESCRIPTION#
#END#
