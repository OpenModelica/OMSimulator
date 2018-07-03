#CAPTION#
setIntegerParameter
-------------------

Set integer value of given parameter.
#END#

#LUA#
.. code-block:: lua

  status = oms2_setIntegerParameter(signal, value)

#END#

#PYTHON#
.. code-block:: python

  status = session.setIntegerParameter(signal, value)

#END#

#CAPI#
.. code-block:: c

  oms_status_enu_t oms2_setIntegerParameter(const char* signal, int value);

#END#

#DESCRIPTION#
#END#
