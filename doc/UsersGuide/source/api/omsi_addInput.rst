#CAPTION#
omsi_addInput
-------------

Add input values for external model inputs.

If there are several measurement series, all series need to be conducted
with the same external inputs!
#END#

#LUA#
.. code-block:: lua

  -- simodel [inout] SysIdent model as opaque pointer.
  -- var     [in] Name of input variable.
  -- time    [in] Array of input time instants.
  -- values  [in] Array of input values corresponding to respective "time" array entries in omsi_initialize().
  -- status  [out] Error status.
  status = omsi_addInput(simodel, var, time, values)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t omsi_addInput(void* simodel, const char* var, const double* values, size_t nValues);

#END#

#DESCRIPTION#
#END#
