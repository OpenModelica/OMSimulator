#CAPTION#
getVariableStepSize
-------------------

Gets the step size parameters.
#END#

#LUA#
.. code-block:: lua

  initialStepSize, minimumStepSize, maximumStepSize, status = oms.getVariableStepSize(cref)

#END#

#PYTHON#
.. code-block:: python

  initialStepSize, minimumStepSize, maximumStepSize, status = oms.getVariableStepSize(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getVariableStepSize(const char* cref, double* initialStepSize, double* minimumStepSize, double* maximumStepSize);

#END#

#DESCRIPTION#
#END#
