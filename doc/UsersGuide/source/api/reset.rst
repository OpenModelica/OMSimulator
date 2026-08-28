#CAPTION#
reset
-----

Reset the composite model after a simulation run.

The FMUs go into the same state as after instantiation.
#END#


#CAPI#
.. code-block:: c

  oms_status_enu_t oms_reset(const char* cref);

#END#

#OMC#
.. code-block:: modelica

  status := oms_reset(cref);

#END#

#DESCRIPTION#
#END#
