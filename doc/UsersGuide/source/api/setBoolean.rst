#CAPTION#
setBoolean
----------

Set boolean value of given signal.
#END#

#LUA#
.. code-block:: lua

  status = oms2_setBoolean(signal, value)

#END#

#PYTHON#
.. code-block:: python

  status = session.setBoolean(signal, value)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_setBoolean(const char* signal, bool value);

#END#

#DESCRIPTION#
#END#
