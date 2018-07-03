#CAPTION#
doSteps
-------

Simulates a composite model for a given number of steps (works for both FMI and TLM).
#END#

#LUA#
.. code-block:: lua

  status = oms2_doSteps(ident, numberOfSteps)

#END#

#PYTHON#
.. code-block:: python

  status = session.doSteps(ident, numberOfSteps)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_doSteps(const char* ident, const int numberOfSteps);

#END#

#DESCRIPTION#
#END#
