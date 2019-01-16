#CAPTION#
reset
-----

Reset the composite model after a simulation run.

The FMUs go into the same state as after instantiation.
#END#

#LUA#
.. code-block:: lua

  status = oms3_reset(cref)

#END#

#PYTHON#
.. code-block:: python

  status = session.oms3_reset(cref)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms3_reset(const char* cref);

#END#

#DESCRIPTION#
#END#
