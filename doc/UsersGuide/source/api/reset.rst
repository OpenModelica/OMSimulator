#CAPTION#
reset
-----

Reset the composite model after a simulation run.

Implemented for FMI (no TLM support).

The FMUs go into the same state as after instantiation.
#END#

#LUA#
.. code-block:: lua

  status = oms2_reset(ident)

#END#

#PYTHON#
.. code-block:: python

  status = session.reset(ident)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_reset(const char* ident);

#END#

#DESCRIPTION#
#END#
