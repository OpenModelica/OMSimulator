#CAPTION#
getFixedStepSize
----------------

Gets the fixed step size. Can be used for the communication step size of
co-simulation systems and also for the integrator step size in model exchange
systems.
#END#


#CAPI#
.. code-block:: c

  oms_status_enu_t oms_getFixedStepSize(const char* cref, double* stepSize);

#END#

#OMC#
.. code-block:: modelica

  (stepSize, status) := oms_getFixedStepSize(cref);

#END#

#DESCRIPTION#
#END#
