#CAPTION#
omsi_initialize
---------------

This function initializes a given composite model. After this call, the model is in simulation mode.
#END#

#LUA#
.. code-block:: lua

  -- simodel        [inout] SysIdent model as opaque pointer.
  -- nSeries        [in] Number of measurement series.
  -- time           [in] Array of measurement/input time instants.
  -- inputvars      [in] Array of names of input variables (empty array if none).
  -- measurmentvars [in] Array of names of observed measurement variables.
  -- status         [out] Error status.
  status = omsi_initialize(simodel, nSeries, time, inputvars, measurementvars)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t omsi_initialize(void* simodel, size_t nSeries, const double* time, size_t nTime, char const* const* inputvars, size_t nInputvars, char const* const* measurementvars, size_t nMeasurementvars);

#END#

#DESCRIPTION#
#END#
