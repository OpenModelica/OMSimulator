#CAPTION#
setBoolean
----------

Set boolean value of given signal.
#END#

#LUA#
.. code-block:: lua

  status = oms_setBoolean(cref, value)

#END#

#PYTHON#
.. code-block:: python

  status = session.oms_setBoolean(cref, value)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms_setBoolean(const char* cref, bool value);

#END#

#DESCRIPTION#
#END#
