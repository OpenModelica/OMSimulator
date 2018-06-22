#CAPTION#
omsi_solve
----------

Solve parameter estimation problem.
#END#

#LUA#
.. code-block:: lua

  -- simodel    [inout] SysIdent model as opaque pointer.
  -- reporttype [in] Print report and progress information after call to Ceres solver.
  --                 Supported report types: {"", "BriefReport", "FullReport"},
  --                 where "" denotes no output.
  -- status     [out] Error status.
  status = omsi_solve(simodel, reporttype)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t omsi_solve(void* simodel, const char* reporttype);

#END#

#DESCRIPTION#
#END#
