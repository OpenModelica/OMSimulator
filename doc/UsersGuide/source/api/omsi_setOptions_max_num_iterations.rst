#CAPTION#
omsi_setOptions_max_num_iterations
----------------------------------

Set Ceres solver option `Solver::Options::max_num_iterations`.
#END#

#LUA#
.. code-block:: lua

  -- simodel            [inout] SysIdent model as opaque pointer.
  -- max_num_iterations [in] Maximum number of iterations for which the solver
  --                         should run (default: 25).
  -- status             [out] Error status.
  status = omsi_setOptions_max_num_iterations(simodel, max_num_iterations)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t omsi_setOptions_max_num_iterations(void* simodel, size_t max_num_iterations);

#END#

#DESCRIPTION#
#END#
