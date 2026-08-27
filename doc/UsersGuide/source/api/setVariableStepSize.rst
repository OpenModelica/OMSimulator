#CAPTION#
setVariableStepSize
-------------------

Sets the step size parameters for methods with stepsize control.
#END#


#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getVariableStepSize(const char* cref, double* initialStepSize, double* minimumStepSize, double* maximumStepSize);

#END#

#OMC#
.. code-block:: modelica

  status := oms_getVariableStepSize(cref, initialStepSize, minimumStepSize, maximumStepSize);

#END#

#DESCRIPTION#
#END#
