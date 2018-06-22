#CAPTION#
omsi_getParameter
-------------------

Get parameter that should be estimated.
#END#

#LUA#
.. code-block:: lua

  -- simodel        [inout] SysIdent model as opaque pointer.
  -- var            [in] Name of parameter.
  -- startvalue     [out] Start value of parameter.
  -- estimatedvalue [out] Estimated value of parameter.
  -- status         [out] Error status.
  status, startvalue, estimatedvalue = omsi_getParameter(simodel, var)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t omsi_getParameter(void* simodel, const char* var, double* startvalue, double* estimatedvalue);

#END#

#DESCRIPTION#
#END#
