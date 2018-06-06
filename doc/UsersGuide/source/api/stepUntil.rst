#CAPTION#
stepUntil
---------

Simulates a composite model until a given time value (works for both FMI and TLM).
#END#

#LUA#
.. code-block:: lua

  status = oms2_stepUntil(ident, timeValue)

#END#

#PYTHON#
.. code-block:: python

  # not yet available

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_stepUntil(const char* ident, const double timeValue);

#END#

#DESCRIPTION#
#END#
