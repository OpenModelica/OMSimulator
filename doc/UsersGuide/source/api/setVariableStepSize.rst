#CAPTION#
setVariableStepSize
-------------------

Sets the step size parameters for methods with stepsize control.
#END#

#LUA#
.. code-block:: lua

  status = oms_getVariableStepSize(cref, initialStepSize, minimumStepSize, maximumStepSize)

#END#

#PYTHON#
.. code-block:: python

  status = oms.getVariableStepSize(cref, initialStepSize, minimumStepSize, maximumStepSize)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getVariableStepSize(const char* cref, double* initialStepSize, double* minimumStepSize, double* maximumStepSize);

#END#

#OMC#
.. code-block:: Modelica

  status := oms_getVariableStepSize(cref, initialStepSize, minimumStepSize, maximumStepSize);

#END#

#DESCRIPTION#
#END#
