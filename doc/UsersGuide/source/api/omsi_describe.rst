#CAPTION#
omsi_describe
----------------------

Print summary of SysIdent model.
#END#

#LUA#
.. code-block:: lua

  -- simodel [inout] SysIdent model as opaque pointer.
  -- status  [out] Error status.
  status = omsi_describe(simodel)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t omsi_describe(void* simodel);

#END#

#DESCRIPTION#
#END#
