#CAPTION#
omsi_addMeasurement
-------------------

Add measurement values for a fitting variable.
#END#

#LUA#
.. code-block:: lua

  -- simodel [inout] SysIdent model as opaque pointer.
  -- iSeries [in] Index of measurement series.
  -- var     [in] Name of variable.
  -- values  [in] Array of measured values for respective time instants.
  -- status  [out] Error status.
  status = omsi_addMeasurement(simodel, iSeries, var, values)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t omsi_addMeasurement(void* simodel, size_t iSeries, const char* var, const double* values, size_t nValues);

#END#

#DESCRIPTION#
#END#
